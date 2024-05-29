// 함수 호출
long read (void);
void waiting(void);
void digital_pin_init(void);
long read_average(unsigned char);
double get_value(unsigned char);
float get_units(unsigned char);
void set_scale(float);
void tare(unsigned char);
void set_offset(long);

const int LOADCELL_DOUT_PIN = 53;
const int LOADCELL_SCK_PIN = 50;

#define PORTE_IRSENSOR 4      // IR Sensor는 PORTE의 4번 핀
#define EXTERNAL_INT0 0 // INT0은 PORTD의 0번핀 노란색
#define EXTERNAL_INT1 1// INT1은 PORTD의 1번핀 빨강색
#define EXTERNAL_INT2 2 // INT2은 PORTD의 2번핀 흰색
#define EXTERNAL_INT3 3 // INT3은 PORTD의 3번핀 초록색

#define PULSE_PIN0 0 // PULSE_PIN0은 PORTA의 0번핀 
#define PULSE_PIN1 1 // PULSE_PIN1은 PORTA의 1번핀
#define PULSE_PIN2 2 // PULSE_PIN2은 PORTA의 2번핀
#define PULSE_PIN3 3 // PULSE_PIN3은 PORTA의 3번핀

int Number = 0;
bool DiffNum = false;
int previrVal = 1;
volatile int cost = 0;
volatile int change =0;

volatile float weight = 0.0;  // 전역 변수로 선언하여 ISR과 loop에서 모두 접근 가능
volatile float current_weight = 0.0;
volatile bool interruptTriggered = false;  // 인터럽트가 발생했는지 여부를 저장하는 플래그
volatile float difference = 0.0; 

void setup() {
  DDRE &= ~(1 << PORTE_IRSENSOR);   // IR 센서를 input으로 설정
  DDRD &= ~((1 << EXTERNAL_INT0) | (1 << EXTERNAL_INT1) | (1 << EXTERNAL_INT2) | (1 << EXTERNAL_INT3)); //INT0 ~ INT3을 INPUT으로 설정
  EICRA |= 0xFF; // INT0 ~ INT3을 RISING_EDGE INTERRUPT모드로 설정

  // PULSE_PIN을 출력으로 설정
  DDRA |= (1 << PULSE_PIN0) | (1 << PULSE_PIN1) | (1 << PULSE_PIN2) | (1 << PULSE_PIN3);

  Serial.begin(38400);
  Serial.println("HX711 Demo");

  // Initialize with data output pin, clock input pin
  digital_pin_init(); // DOUT-INPUT설정(DDR_ &= ~XXXX) SCK-OUTPUT설정(DDR_ |= XXXX)

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(read());         // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(get_value(5));    // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(get_units(5), 1);    // print the average of 5 readings from the ADC minus tare weight (not set) divided
                        // by the SCALE parameter (not set yet)

  set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  tare(5);                 // reset the scale to 0

  Serial.println("SETTING COMPLETE:");
}

void loop() {
  int irVal = PINE & (1<<PORTE_IRSENSOR);  // 센서값 읽어옴

  if ((irVal == 0) & (previrVal != 0)) {
    Number = Number + 1;
    DiffNum = true;
  }
  previrVal = irVal;
  Serial.print("Number: ");
  Serial.println(Number);
  Serial.print("change: ");
  Serial.println(change);
  Serial.print("weigth: ");
  Serial.println(weight);

  while (DiffNum) {
    delay(1000);
    long Load_Cell_Data = read();
    if (interruptTriggered) {  // 인터럽트가 발생했을 때만 실행
      current_weight = get_units(1);
      difference = get_units(1) - current_weight;
      Serial.println("RESET");
      interruptTriggered = false;  // 플래그 리셋
    }

    Serial.print("one reading:\t");
    Serial.print(get_units(1), 1);
    difference = get_units(1) - current_weight;
    if ((difference < 1.0) && (difference > -1.0)) {
      weight = 0.0;  // 현재 무게를 읽어 weight 변수에 저장
    }
    else{
      weight = get_units(1) - current_weight;
   }
    Serial.print("\t weight:\t");
    Serial.print("\t| average:\t");
    Serial.println(get_units(1), 1);
    Serial.print(weight, 1);

    //  ADC 휴식모드
    digitalWrite(LOADCELL_SCK_PIN, LOW);
	  digitalWrite(LOADCELL_SCK_PIN, HIGH);
    delay(10);
    digitalWrite(LOADCELL_SCK_PIN, LOW);
    DiffNum = false;
  }

  //  Number와 weigth를 통해 cost설정
  // 500원짜리 1개
  if ((Number == 1) && (weight > 5.5) && (weight < 6.0)) {
    cost = 500;
  } 

  // 100원짜리 5개
  if ((Number == 5) && (weight > 19.0) && (weight < 21.0)) {
    cost = 500;
  } 

  // 500원짜리 2개
  if ((Number == 2) && (weight > 11.0) && (weight < 12.0)) {
    cost = 1000;
  } 

  // 500원짜리 1개 + 100원짜리 5개
  if ((Number == 6) && (weight > 24.5) && (weight < 27.0)) {
    cost = 1000;
  } 

  //  100원짜리 10개
  if ((Number == 10) && (weight > 38.0) && (weight < 42.0)) {
    cost = 1000;
  } 

  switch (cost) {
    case 0:     // LCD "INSERT COIN"
      EIMSK &= ~((1 << EXTERNAL_INT0) | (1 << EXTERNAL_INT1) | (1 << EXTERNAL_INT2) | (1 << EXTERNAL_INT3)); // INT0 ~ INT3 INTERRUPT DISABLE
      break;

    case 500:
     EIMSK |= (1 << EXTERNAL_INT2) | (1 << EXTERNAL_INT3);   // INT2 ~ INT3 INTERRUPT ENABLE
     change = 0;
     break;

    case 1000:   // LCD "MAX COIN"
      EIMSK |= (1 << EXTERNAL_INT0) | (1 << EXTERNAL_INT1) | (1 << EXTERNAL_INT2) | (1 << EXTERNAL_INT3);   // INT0 ~ INT3 INTERRUPT ENABLE
      break;
  }

  Serial.print("Number");
  Serial.println(Number);  // Number 값 출력
  Serial.print("cost: ");
  Serial.println(cost);
}

// // 인터럽트 서비스 루틴
// ISR(INT0_vect){
//   // 인터럽트가 발생했을 때의 동작 정의
//   interruptTriggered = true;  // 인터럽트 발생 플래그 설정
// }

ISR(INT0_vect){
  EIMSK &= ~((1 << EXTERNAL_INT0) | (1 << EXTERNAL_INT1) | (1 << EXTERNAL_INT2) | (1 << EXTERNAL_INT3)); // INT0 ~ INT3 INTERRUPT DISABLE
  
  // 반시계 방향으로 360도 회전 (2ms 펄스)
  for (int i = 0; i < 380 ; i++) {  // 스프링을 달게 되면 370의 값을 증가시켜야 할 수도 있음. -> 실험적으로 수정해보기
    PORTA |= (1 << PULSE_PIN0);
    delayMicroseconds(2000); // 2ms 펄스
    PORTA &= ~(1 << PULSE_PIN0);
    delayMicroseconds(18000); // 18ms 대기 (총 주기 20ms)
  }

  // 서보 모터를 멈추기 위한 중립 펄스 (예: 1.5ms)
  for (int i = 0; i < 50; i++) { // 50 * 20ms = 1초 동안 중립 상태 유지
    PORTA |= (1 << PULSE_PIN0);
    delayMicroseconds(1500); // 1.5ms 펄스
    PORTA &= ~(1 << PULSE_PIN0);
    delayMicroseconds(18500); // 18.5ms 대기 (총 주기 20ms)
  }

  //피에조 부저
  interruptTriggered = true;
  DiffNum = true;
  Number = 0;
  cost = 0;
}

ISR(INT1_vect){
  EIMSK &= ~((1 << EXTERNAL_INT0) | (1 << EXTERNAL_INT1) | (1 << EXTERNAL_INT2) | (1 << EXTERNAL_INT3)); // INT0 ~ INT3 INTERRUPT DISABLE
  
  // 반시계 방향으로 360도 회전 (2ms 펄스)
  for (int i = 0; i < 380 ; i++) {  // 스프링을 달게 되면 370의 값을 증가시켜야 할 수도 있음. -> 실험적으로 수정해보기
    PORTA |= (1 << PULSE_PIN1);
    delayMicroseconds(2000); // 2ms 펄스
    PORTA &= ~(1 << PULSE_PIN1);
    delayMicroseconds(18000); // 18ms 대기 (총 주기 20ms)
  }

  // 서보 모터를 멈추기 위한 중립 펄스 (예: 1.5ms)
  for (int i = 0; i < 50; i++) { // 50 * 20ms = 1초 동안 중립 상태 유지
    PORTA |= (1 << PULSE_PIN1);
    delayMicroseconds(1500); // 1.5ms 펄스
    PORTA &= ~(1 << PULSE_PIN1);
    delayMicroseconds(18500); // 18.5ms 대기 (총 주기 20ms)
  }
  
  //피에조 부저
  interruptTriggered = true;
  DiffNum = true;
  Number = 0;
  cost = 0;
}

ISR(INT2_vect){
  if (cost > 0) {
    if (cost == 1000){
      Number = 1;
      cost = 500;
      change = 1;
    }
     // 반시계 방향으로 360도 회전 (2ms 펄스)
    for (int i = 0; i < 380 ; i++) {  // 스프링을 달게 되면 370의 값을 증가시켜야 할 수도 있음. -> 실험적으로 수정해보기
      PORTA |= (1 << PULSE_PIN2);
      delayMicroseconds(2000); // 2ms 펄스
      PORTA &= ~(1 << PULSE_PIN2);
      delayMicroseconds(18000); // 18ms 대기 (총 주기 20ms)
    }

    // 서보 모터를 멈추기 위한 중립 펄스 (예: 1.5ms)
    for (int i = 0; i < 50; i++) { // 50 * 20ms = 1초 동안 중립 상태 유지
      PORTA |= (1 << PULSE_PIN2);
      delayMicroseconds(1500); // 1.5ms 펄스
      PORTA &= ~(1 << PULSE_PIN2);
      delayMicroseconds(18500); // 18.5ms 대기 (총 주기 20ms)
    }
    EIMSK &= ~((1 << EXTERNAL_INT0) | (1 << EXTERNAL_INT1)); // INT0 ~ INT1 INTERRUPT DISABLE 
  }

  if (change != 1){
    //피에조 부저
    EIMSK &= ~((1 << EXTERNAL_INT2) | (1 << EXTERNAL_INT3)); // INT2 ~ INT3 INTERRUPT DISABLE
    interruptTriggered = true;
    DiffNum = true;
    Number = 0;
    cost = 0;
    change = 0;
  }
}

ISR(INT3_vect){
  if (cost > 0) {
    if (cost == 1000){
      Number = 1;
      cost = 500;
      change = 1;
    }
     // 반시계 방향으로 360도 회전 (2ms 펄스)
    for (int i = 0; i < 380 ; i++) {  // 스프링을 달게 되면 370의 값을 증가시켜야 할 수도 있음. -> 실험적으로 수정해보기
      PORTA |= (1 << PULSE_PIN3);
      delayMicroseconds(2000); // 2ms 펄스
      PORTA &= ~(1 << PULSE_PIN3);
      delayMicroseconds(18000); // 18ms 대기 (총 주기 20ms)
    }

    // 서보 모터를 멈추기 위한 중립 펄스 (예: 1.5ms)
    for (int i = 0; i < 50; i++) { // 50 * 20ms = 1초 동안 중립 상태 유지
      PORTA |= (1 << PULSE_PIN3);
      delayMicroseconds(1500); // 1.5ms 펄스
      PORTA &= ~(1 << PULSE_PIN3);
      delayMicroseconds(18500); // 18.5ms 대기 (총 주기 20ms)
    }
    EIMSK &= ~((1 << EXTERNAL_INT0) | (1 << EXTERNAL_INT1)); // INT0 ~ INT1 INTERRUPT DISABLE 
  }

  if (change != 1){
    //피에조 부저
    EIMSK &= ~((1 << EXTERNAL_INT2) | (1 << EXTERNAL_INT3)); // INT2 ~ INT3 INTERRUPT DISABLE
    interruptTriggered = true;
    DiffNum = true;
    Number = 0;
    cost = 0;
  }
}

// ISR(INT3_vect){
//   if (cost == 1000) {
//     cost = 500;
//     change = 1;

  
//     // 반시계 방향으로 360도 회전 (2ms 펄스)
//     for (int i = 0; i < 380 ; i++) {  // 스프링을 달게 되면 370의 값을 증가시켜야 할 수도 있음. -> 실험적으로 수정해보기
//       PORTA |= (1 << PULSE_PIN3);
//       delayMicroseconds(2000); // 2ms 펄스
//       PORTA &= ~(1 << PULSE_PIN3);
//       delayMicroseconds(18000); // 18ms 대기 (총 주기 20ms)
//     }

//     // 서보 모터를 멈추기 위한 중립 펄스 (예: 1.5ms)
//     for (int i = 0; i < 50; i++) { // 50 * 20ms = 1초 동안 중립 상태 유지
//       PORTA |= (1 << PULSE_PIN3);
//       delayMicroseconds(1500); // 1.5ms 펄스
//       PORTA &= ~(1 << PULSE_PIN3);
//       delayMicroseconds(18500); // 18.5ms 대기 (총 주기 20ms)
//     }
//     EIMSK &= ~((1 << EXTERNAL_INT0) | (1 << EXTERNAL_INT1)); // INT0 ~ INT1 INTERRUPT DISABLE 
//   }

//   if (change != 1) {
//     if (cost == 500) {
//       // 반시계 방향으로 360도 회전 (2ms 펄스)
//       for (int i = 0; i < 380 ; i++) {  // 스프링을 달게 되면 370의 값을 증가시켜야 할 수도 있음. -> 실험적으로 수정해보기
//         PORTA |= (1 << PULSE_PIN3);
//         delayMicroseconds(2000); // 2ms 펄스
//         PORTA &= ~(1 << PULSE_PIN3);
//         delayMicroseconds(18000); // 18ms 대기 (총 주기 20ms)
//       }

//       // 서보 모터를 멈추기 위한 중립 펄스 (예: 1.5ms)
//       for (int i = 0; i < 50; i++) { // 50 * 20ms = 1초 동안 중립 상태 유지
//         PORTA |= (1 << PULSE_PIN3);
//         delayMicroseconds(1500); // 1.5ms 펄스
//         PORTA &= ~(1 << PULSE_PIN3);
//         delayMicroseconds(18500); // 18.5ms 대기 (총 주기 20ms)
//       }
//       //피에조 부저
//       interruptTriggered = true;
//       Number = 0;
//       cost = 0;
//     }
//   }
//   EIMSK &= ~((1 << EXTERNAL_INT2) | (1 << EXTERNAL_INT3)); // INT2 ~ INT3 INTERRUPT DISABLE
// }

// pinMode 설정
void digital_pin_init(void) {
  pinMode(LOADCELL_DOUT_PIN, INPUT);
  pinMode(LOADCELL_SCK_PIN, OUTPUT);
}

// CONVERSION을 하기 위한 조건 DOUT=LOW -> CONVERSION ENABLE
void waiting (void) {
  while (1) {
    if(digitalRead(LOADCELL_DOUT_PIN) == LOW) break;
  }
}

// Gain을 128로 고정, long read(unsigned int Gain) -> long read(void)로 변경
// 로드셀이 측정한 아날로그 데이터를 HX711이 디지털 데이터로 변환한 값을 읽어옴
// Gain=128: SCK HIGH -> LOW --> 1DATA 24DATA(analog data -> digital data) + 25th DATA
long read(void) {
  waiting();  // DOUT이 LOW상태가 될 때까지 기다리는 함수
  noInterrupts(); // 아두이노 인터럽트를 비활성화(데이터 수신할 때 방해 가능성 있음)
  unsigned long real_data = 0;  // 실제 DOUT에서 받은 DATA가 저장되는 변수

  for(int i = 0; i < 24; i++) {   // 총 24번 반복(0~23) 24Bits의 데이터를 받기 위함
    digitalWrite(LOADCELL_SCK_PIN, HIGH);      // SCK를 HIGH상태로 만들어 CLOCK발생 준비
    delayMicroseconds(1);         // SCK가 적어도 0.1us동안 HIGH상태를 유지해야 해서 1us 기다림 (또한 DOUT의 상태가 변하기까지 0.1us 기다려야함)

    real_data |= (unsigned long)digitalRead(LOADCELL_DOUT_PIN) << (23 - i);  // DOUT의 상태를 읽어 real_data의 23번 비트부터 0번 비트까지 채움
    digitalWrite(LOADCELL_SCK_PIN, LOW);                     // SCK를 LOW상태로 만들어 CLOCK발생
    delayMicroseconds(1);                       // SCK가 적어도 0.2us동안 LOW상태를 유지해야 해서 1us 기다림
  }
  
  // Gain=128이기 때문에 추가로 1clock 발생시켜야 함
  digitalWrite(LOADCELL_SCK_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(LOADCELL_SCK_PIN, LOW);
  delayMicroseconds(1);

  interrupts();                  // 모든 데이터를 수신했기에, 다시 아두이노의 인터럽트 활성화
  
  // Convert 24-bit value to signed 32-bit integer
  if (real_data & 0x800000) {
    real_data |= 0xFF000000;
  }

  return (long)real_data;  
}

long OFFSET = 0;
float SCALE = 1.0;

//   여러 번 데이터를 읽어 평균 값을 계산하는 함수
long read_average(unsigned char times){
  long sum = 0;
  for (unsigned char i = 0; i < times; i++) {
    sum += read();
    delay(0); // 과부화 방지
  }
  return sum / times;
}

//   평균 값을 구하고, 그 값에서 보정 값(OFFSET)을 뺀 결과를 반환하는 함수
double get_value(unsigned char times){
  return read_average(times) - OFFSET;
}

//  get_value 값을 스케일 팩터로 나누어 실제 단위로 변환된 측정 값을 반환
float get_units(unsigned char times){
  return get_value(times) / SCALE;
}

void set_scale(float scale){
  SCALE = scale;        // SCALE 설정
}

// 영점 조절
void tare(unsigned char times){
  double sum = read_average(times);
  set_offset(sum);
}

void set_offset(long offset){
  OFFSET = offset;      // OFFSET 설정
}

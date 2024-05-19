// 함수 호출
long read (void);
void waiting(void);
void digital_pin_init(void);

// DOUT과 SCK를 연결한 PIN NUMBER
#define DOUT 3
#define SCK 4

void setup() {
  Serial.begin(9600);
  digital_pin_init(); // DOUT-INPUT설정(DDR_ &= ~XXXX) SCK-OUTPUT설정(DDR_ |= XXXX)
}

void loop() {
  long Load_Cell_Data = read(); 
  Serial.println(Load_Cell_Data);
}

// pinMode 설정
void digital_pin_init(void) {
  pinMode(DOUT, INPUT);
  pinMode(SCK, OUTPUT);
}

// CONVERSION을 하기 위한 조건 DOUT=LOW -> CONVERSION ENABLE
void waiting (void) {
  while (1) {
    if(digitalRead(DOUT) == LOW) break;
  }
}

// Gain을 128로 고정, long read(unsigned int Gain) -> long read(void)로 변경
// 로드셀이 측정한 아날로그 데이터를 HX711이 디지털 데이터로 변환한 값을 읽어옴
// Gain=128: SCK HIGH -> LOW --> 1DATA 24DATA(analog data -> digital data) + 25th DATA
long read(void) {
  long real_data = 0;  // 실제 DOUT에서 받은 DATA가 저장되는 변수
  waiting();  // DOUT이 LOW상태가 될 때까지 기다리는 함수
  noInterrupts(); // 아두이노 인터럽트를 비활성화(데이터 수신할 때 방해 가능성 있음)

  for(int i = 0; i < 24; i++) {   // 총 24번 반복(0~23) 24Bits의 데이터를 받기 위함
    digitalWrite(SCK, HIGH);      // SCK를 HIGH상태로 만들어 CLOCK발생 준비
    delayMicroseconds(1);         // SCK가 적어도 0.1us동안 HIGH상태를 유지해야 해서 1us 기다림 (또한 DOUT의 상태가 변하기까지 0.1us 기다려야함)

    real_data |= digitalRead(DOUT) << (24 - i);  // DOUT의 상태를 읽어 real_data의 23번 비트부터 0번 비트까지 채움
    digitalWrite(SCK, LOW);                     // SCK를 LOW상태로 만들어 CLOCK발생
    delayMicroseconds(1);                       // SCK가 적어도 0.2us동안 LOW상태를 유지해야 해서 1us 기다림
  }
  
  // Gain=128이기 때문에 추가로 1clock 발생시켜야 함
  digitalWrite(SCK, HIGH);
  delayMicroseconds(1);
  digitalWrite(SCK, LOW);
  delayMicroseconds(1);

  interrupts();                  // 모든 데이터를 수신했기에, 다시 아두이노의 인터럽트 활성화
  
  // Convert 24-bit value to signed 32-bit integer
  if (real_data & 0x800000) {
    real_data |= 0xFF000000;
  }

  return (long)real_data;  
}


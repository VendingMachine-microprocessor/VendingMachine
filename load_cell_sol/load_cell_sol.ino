#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 4;

HX711 scale;
volatile float weight = 0.0;  // 전역 변수로 선언하여 ISR과 loop에서 모두 접근 가능
volatile float current_weight = 0.0;
volatile bool interruptTriggered = false;  // 인터럽트가 발생했는지 여부를 저장하는 플래그
volatile float difference = 0.0; 

void setup() {
  // 인터럽트 핀 설정 (핀 2 사용)
  DDRD &= ~0x04;
  EICRA |= 0x02;
  EIMSK |= 0x01;

  Serial.begin(38400);
  Serial.println("HX711 Demo");

  Serial.println("Initializing the scale");

  // Initialize library with data output pin, clock input pin and gain factor.
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());         // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));    // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));    // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);    // print the average of 5 readings from the ADC minus tare weight (not set) divided
                        // by the SCALE parameter (not set yet)

  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();                 // reset the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));    // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
                        // by the SCALE parameter set with set_scale

  Serial.println("Readings:");
}

void loop() {
  if (interruptTriggered) {  // 인터럽트가 발생했을 때만 실행
    current_weight = scale.get_units();
    difference = scale.get_units() - current_weight;
    Serial.println("RESET");
    interruptTriggered = false;  // 플래그 리셋
  }

  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  difference = scale.get_units() - current_weight;
  if ((difference < 1.0) && (difference > -1.0)) {
    weight = 0.0;  // 현재 무게를 읽어 weight 변수에 저장
  }
  else{
    weight = scale.get_units() - current_weight;
  }
  Serial.print("\t weight:\t");
  Serial.print(weight, 1);
  Serial.print("\t| average:\t");
  Serial.println(scale.get_units(10), 1);

  scale.power_down();             // put the ADC in sleep mode
  delay(500);
  scale.power_up();
}

// 인터럽트 서비스 루틴
ISR(INT0_vect){
  // 인터럽트가 발생했을 때의 동작 정의
  interruptTriggered = true;  // 인터럽트 발생 플래그 설정
}

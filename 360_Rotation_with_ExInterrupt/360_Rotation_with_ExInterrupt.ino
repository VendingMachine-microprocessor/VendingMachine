#define PULSE_PIN 0x02

void setup() {
  // PULSE_PIN을 출력으로 설정
  DDRB |= PULSE_PIN;
  
  // 외부 인터럽트 설정 (INT0, PD2 핀)
  EIMSK |= (1 << INT0);   // INT0 인터럽트 활성화
  EICRA |= (1 << ISC01);  // falling에서 interrupt 발생

  sei(); // 전역 인터럽트 활성화
}

void loop() {
}


ISR(INT0_vect) {
  // 반시계 방향으로 360도 회전 (2ms 펄스)
  for (int i = 0; i < 380 ; i++) {  // 스프링을 달게 되면 370의 값을 증가시켜야 할 수도 있음. -> 실험적으로 수정해보기
    PORTB |= PULSE_PIN;
    delayMicroseconds(2000); // 2ms 펄스
    PORTB &= ~PULSE_PIN;
    delayMicroseconds(18000); // 18ms 대기 (총 주기 20ms)
  }

  // 서보 모터를 멈추기 위한 중립 펄스 (예: 1.5ms)
  for (int i = 0; i < 50; i++) { // 50 * 20ms = 1초 동안 중립 상태 유지
    PORTB |= PULSE_PIN;
    delayMicroseconds(1500); // 1.5ms 펄스
    PORTB &= ~PULSE_PIN;
    delayMicroseconds(18500); // 18.5ms 대기 (총 주기 20ms)
  }
}

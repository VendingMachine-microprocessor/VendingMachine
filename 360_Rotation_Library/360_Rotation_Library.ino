#include <Servo.h>

Servo myservo;  // 서보 객체 생성

void setup() {
  myservo.attach(9);  // 서보를 9번 핀에 연결
}

void loop() {
  // 서보를 시계 방향으로 회전
  myservo.writeMicroseconds(2000);  // 최대 속도로 시계 방향 회전

  delay(1450);  // 1초 동안 회전 (모터의 속도에 따라 조정 필요)

  // 서보 멈춤
  myservo.writeMicroseconds(1500);  // 중립 위치 (멈춤)

  while(1);  // 프로그램 종료 (또는 다른 동작을 추가할 수 있음)
}

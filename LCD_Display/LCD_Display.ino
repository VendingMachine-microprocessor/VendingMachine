#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define UNO_from_MEGA0 0 // UNO와 MEGA연결 1번핀 PORBD의 0번핀
#define UNO_from_MEGA1 1 // UNO와 MEGA연결 2번핀 PORBD의 1번핀
#define UNO_from_MEGA2 2 // UNO와 MEGA연결 3번핀 PORBD의 2번핀

void setup() {
  Serial.begin(9600);
  DDRB &= ~(1 << UNO_from_MEGA0);  //pinMode(8, LOW);
  DDRB &= ~(1 << UNO_from_MEGA1);  //pinMode(9, LOW);
  DDRB &= ~(1 << UNO_from_MEGA2);  //pinMode(10, LOW);    
  lcd.init();
  lcd.backlight();
  lcd.print("INSERT COIN");
}

void loop() {

  //Debuging PIN8 ~ PIN10
  // Serial.print("PB0 :");
  // Serial.println(digitalRead(8));

  // Serial.print("PB1 :");
  // Serial.println(digitalRead(9));

  // Serial.print("PB2 :");
  // Serial.println(digitalRead(10));

  bool pinValue0 = PINB & (1 << UNO_from_MEGA0);
  bool pinValue1 = PINB & (1 << UNO_from_MEGA1);
  bool pinValue2 = PINB & (1 << UNO_from_MEGA2);

  if ((pinValue0 == true) && (pinValue1 == false) && (pinValue2 == false)) {
    lcd.clear();
    lcd.print("INSERT COIN");
    delay(500);
  }

  if ((pinValue0 == false) && (pinValue1 == true) && (pinValue2 == false)) {
    lcd.clear();
    lcd.print("PRESS THE BUTTON");
    delay(500);
  }

  if ((pinValue0 == false) && (pinValue1 == false) && (pinValue2 == true)) {
    lcd.clear();
    lcd.print("MAX COIN");
    delay(500);
  }
}
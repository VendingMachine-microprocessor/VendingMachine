#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  pinMode(8, LOW);
  pinMode(9, LOW);
  pinMode(10, LOW);    
  lcd.init();
  lcd.backlight();
  lcd.print("INSERT COIN");
}

void loop() {

  Serial.print("PB0 :");
  Serial.println(digitalRead(8));

  Serial.print("PB1 :");
  Serial.println(digitalRead(9));

  Serial.print("PB2 :");
  Serial.println(digitalRead(10));

  if ((digitalRead(8) == 1) && (digitalRead(9) == 0) && (digitalRead(10) == 0)) {
    lcd.clear();
    lcd.print("INSERT COIN");
    delay(500);
  }

  if ((digitalRead(8) == 0) && (digitalRead(9) == 1) && (digitalRead(10) == 0)) {
    lcd.clear();
    lcd.print("PRESS THE BUTTON");
    delay(500);
  }

  if ((digitalRead(8) == 0) && (digitalRead(9) == 0) && (digitalRead(10) == 1)) {
    lcd.clear();
    lcd.print("MAX COIN");
    delay(500);
  }
}

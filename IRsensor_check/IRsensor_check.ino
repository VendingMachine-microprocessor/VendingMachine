#define PORTD_LED_RED 2       //LED RED는 PORTD
#define PORTD_LED_GREEN 3     //LED GREEN는 PORTD
#define PORTB_IRSENSOR 1      //IR Sensor는 PORTB

void setup () {
  DDRD = (1<<PORTD_LED_RED)|(1<<PORTD_LED_GREEN);  //LED가 2번(PD2) 3번(PD3) 핀에 있기 때문에 출력으로 설정
  DDRB &= ~(1<<PORTB_IRSENSOR);   //IR센서는 0또는 1의 값을 읽어옴. input으로 설정
  Serial.begin(9600);
}
 
void loop () {

  int irVal = PINB & (1<<PORTB_IRSENSOR);  // 센서값 읽어옴


  if (irVal==0) {          // 장애물 감지되면
    PORTD |= (1<<PORTD_LED_RED);      //빨간색 LED 켜짐
    PORTD &= ~(1<<PORTD_LED_GREEN);     //초록색 LED 꺼짐
    Serial.println("Object detected!!");
  } 
  
  else if (irVal!=0) {                    // 장애물이 감지 안되면
    PORTD |= (1<<PORTD_LED_GREEN);      //빨간색 LED 꺼짐
    PORTD &= ~(1<<PORTD_LED_RED);      //초록색 LED 켜짐
    Serial.println("Object not detected");
  }

  delay(100);

}
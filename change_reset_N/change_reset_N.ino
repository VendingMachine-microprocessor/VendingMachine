#define PORTB_IRSENSOR 1      //IR Sensor는 PORTB
#define PORTB_SWITCH 2        //SWITCH는 PORTB

int N = 0;
int previrVal = 1;

void setup () {
  DDRB &= ~((1<<PORTB_IRSENSOR) | (1<<PORTB_SWITCH));   //IR센서는 0또는 1의 값을 읽어옴. IR센서와 Switch를 input으로 설정
  Serial.begin(9600);

}
 
void loop () {

  int irVal = PINB & (1<<PORTB_IRSENSOR);  // 센서값 읽어옴
  int piezo = PINB & (1<<PORTB_SWITCH);     // 스위치값 읽어옴





  if ((irVal == 0) & (previrVal != 0)) {
    N = N + 1;
  }
  previrVal = irVal;

  Serial.println(N);


  if(piezo!=0){
    N = 0;
  }



}


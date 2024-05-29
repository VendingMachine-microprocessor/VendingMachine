#define OC2B       0x40
#define NUM_FREQ   14

const float frequencies[NUM_FREQ] = {
  329.63,  // E4
  293.66,  // D4
  261.63,  // C4
  293.66,  // D4
  329.63,  // E4
  329.63,  // E4
  329.63,  // E4
  293.66,  // D4
  293.66,  // D4
  293.66,  // D4
  329.63,  // E4
  329.63,  // E4
  329.63,  // E4
  329.63   // E4
};

uint8_t freq_count = 0;

void playMelody() {
  for(freq_count = 0; freq_count < NUM_FREQ; freq_count++){
    float freq_target = frequencies[freq_count];
    
    OCR2A = F_CPU / 256 / freq_target - 1;
    OCR2B = OCR2A / 2;

    TCCR2A |= (1 << COM2B1); // Enable OC2B output

    for(uint16_t j = 0; j < 20; j++){
      for(uint16_t k = 0; k < 64000; k++){
        asm("nop");
      }
    }

    TCCR2A &= ~(1 << COM2B1); // OC2B output disable -> no piezo sound

    for(uint16_t j = 0; j < 5; j++){ // 한 음 출력된 소리 끄고 다음 잠깐 딜레이
      for(uint16_t k = 0; k < 64000; k++){
        asm("nop");
      }
    }
  }
  TCCR2A &= ~(1 << COM2B1); // 모든 주파수를 출력한 후 OC2B 출력을 비활성화
}

ISR(INT0_vect) {
  playMelody();
}

void loop(){
}

void setup() {
  // Disable global interrupt
  cli();

  // Register settings for INT0 interrupt
  EICRA |= 0x03;  // Rising edge triggers interrupt
  EIMSK |= 0x01;  // Enable INT0

  // Setting output port
  DDRD |= OC2B;

  // Timer/Counter2 settings
  TCCR2A |= (1 << WGM21) | (1 << WGM20); // Fast PWM mode
  TCCR2B |= (1 << WGM22);
  TCCR2B |= (1 << CS22) | (1 << CS21) | (0 << CS20); // Prescaler: 256

  // OC2B output disable -> no piezo sound before interrupt
  TCCR2A &= ~(1 << COM2B1); 

  // Calculate OCRA (Frequency)
  float freq_target = frequencies[freq_count];
  OCR2A = F_CPU / 256 / freq_target - 1;
  OCR2B = OCR2A / 2;

  // Enable global interrupt
  sei();
}

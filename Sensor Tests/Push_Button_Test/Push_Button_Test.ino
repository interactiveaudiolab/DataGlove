bool init_state = 0;

//#include <Servo.h>
//
//Servo servo1;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  //servo1.attach(0);

  // Set D0 to pullup mode
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
}

void loop() {
  bool push0 = digitalRead(0);
  bool push1 = digitalRead(1);
  bool push2 = digitalRead(2);
  bool push3 = digitalRead(3);

  int flex_value0 = analogRead(0);
  int flex_value1 = analogRead(1);

  if (init_state == push0) {
    Serial.println("Pin 0 pressed");

    BeanMidi.enable();
    
    BeanMidi.noteOn(CHANNEL1, NOTE_C4, 60);
    Bean.setLedBlue(50);
    delay(2000);
    BeanMidi.noteOff(CHANNEL1, NOTE_C4, 60);
    Bean.setLedBlue(0);
  }
  
  if (init_state == push1) {
    Serial.println("Pin 1 pressed");

    BeanMidi.enable();
   
    BeanMidi.noteOn(CHANNEL1, NOTE_E4, 60);
    Bean.setLedGreen(50);
    delay(2000);
    BeanMidi.noteOff(CHANNEL1, NOTE_E4, 60);
    Bean.setLedGreen(0);
  }

    if (init_state == push2) {
    Serial.println("Pin 2 pressed");

    BeanMidi.enable();
    
    BeanMidi.noteOn(CHANNEL1, NOTE_G4, 60);
    Bean.setLedRed(50);
    delay(2000);
    BeanMidi.noteOff(CHANNEL1, NOTE_G4, 60);
    Bean.setLedRed(0);
  }
  
  if (init_state == push3) {
    Serial.println("Pin 3 pressed");

    BeanMidi.enable();
   
    BeanMidi.noteOn(CHANNEL1, NOTE_C5, 60);
    Bean.setLed(0,60,60);
    delay(2000);
    BeanMidi.noteOff(CHANNEL1, NOTE_C5, 60);
    Bean.setLed(0,0,0);
  }
    
  
  // Sleep for a tenth of a second before checking the pins again
  Bean.sleep(100);
}

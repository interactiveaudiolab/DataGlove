// Initialize variables
int mode = 0;   // different modes of operation for accelerometer.

uint8_t x_raw, y_raw;

long duration;
long i_duration = 0.0;

// Initialize Functions
void mode_check();
void flex_check();
void button_check();
bool button_read(int n);
bool flex_read(int k);

void setup() {

  // Allow Bean to communicate through MIDI messages
  BeanMidi.enable();
  Bean.setAccelerationRange(8);

  // Set digital inputs to high for buttons
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
}

void loop() {
  mode_check();
  flex_check();
  button_check();
}

void mode_check(){
  // Button 2 is configured to switch accel mode
  if (button_read(2)){
    mode += 1;
    if (mode > 3){
      mode = 0;
    }
    delay(100);
  }
  // mode 0 has both sensors on
  if (mode == 0){
    x_raw = abs(Bean.getAccelerationY() + 60);
    y_raw = 127 - abs(Bean.getAccelerationX() + 60);
    
    Bean.setLed(10,10,10); //White LED
    BeanMidi.sendMessage(CONTROLCHANGE, 18, x_raw);
    BeanMidi.sendMessage(CONTROLCHANGE, 19, y_raw);
  }
  // mode 1 has horizontal movement on only
  if (mode == 1){
    x_raw = abs(Bean.getAccelerationY() + 60);
    
    Bean.setLed(0,0,15);  //Blue LED
    BeanMidi.sendMessage(CONTROLCHANGE, 18, x_raw);
  }
  // mode 2 has vertical movement on only
  if (mode == 2){
    y_raw = 127 - abs(Bean.getAccelerationX() + 60);  
    //Accel. appears inverted, so this flips all values for proper orientation.
     
    Bean.setLed(0,18,0);  //Green LED
    BeanMidi.sendMessage(CONTROLCHANGE, 19, y_raw);
  }  
  // mode 3 does not transmit any accelerometer data reading.
  if (mode == 3){
    Bean.setLed(50,0,0);  //Red LED
  }
}

// buttons are used as switches by pushing them
void button_check(){
  if(button_read(0)){
    BeanMidi.noteOn(CHANNEL2, NOTE_A3, 100);
    delay(20);
    BeanMidi.noteOff(CHANNEL2, NOTE_A3, 100);
  }
  
  if(button_read(1)){
    BeanMidi.noteOn(CHANNEL2, NOTE_B3, 100);
    delay(20);
    BeanMidi.noteOff(CHANNEL2, NOTE_B3, 100);
  }

  if(button_read(3)){
    BeanMidi.noteOn(CHANNEL2, NOTE_C4, 100);
    delay(20);
    BeanMidi.noteOff(CHANNEL2, NOTE_C4, 100);
  }
}

// Reads digital button value (button reads 0 if pressed). This function allows for
// switch functionality instead of a continuous output.
bool button_read(int n){
  bool button = digitalRead(n);
  if (button == 0){
    return 1;
  }
  else {return 0;}
}

 // Flex data is used as switches by bending the specified finger.
void flex_check(){
  // index finger
  if(flex_read(0)){
    BeanMidi.noteOn(CHANNEL1, NOTE_C3, 100);
    delay(20);
    BeanMidi.noteOff(CHANNEL1, NOTE_C3, 100);
  }
  // middle finger
  if(flex_read(1)){
    BeanMidi.noteOn(CHANNEL1, NOTE_D3, 100);
    delay(20);
    BeanMidi.noteOff(CHANNEL1, NOTE_D3, 100);
  }
  // ring finger
  if(flex_read(2)){
    BeanMidi.noteOn(CHANNEL1, NOTE_E3, 100);
    delay(20);
    BeanMidi.noteOff(CHANNEL1, NOTE_E3, 100);
  }
//  // pinky finger
//  if(flex_read(3)){
//    BeanMidi.noteOn(CHANNEL7, NOTE_F3, 100);
//    delay(20);
//    BeanMidi.noteOff(CHANNEL7, NOTE_F3, 100);
//  }
}

// Reads analog flex sensor data. If the bend value is greater than a threshold, return 1
// ** Flex sensors act as switches as opposed to a range of values.
bool flex_read(int k){
  int flex = analogRead(k);
  if (flex > 900){
    return 1;
  }
  else {return 0;}
}


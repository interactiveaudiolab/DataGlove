// Initialize variables
int mode = 0;   // different modes of operation for accelerometer.

uint8_t x_raw, y_raw;

long duration;
long i_duration = 0.0;

// Initialize Functions
void mode_check();
bool button_read(int n);

void setup() {
  // Initialize serial communication
  //Serial.begin(115200);

  // Allow Bean to communicate through MIDI messages
  BeanMidi.enable();
  Bean.setAccelerationRange(8);

  // Set digital inputs to high for buttons
  pinMode(2, INPUT_PULLUP);
}

void loop() {
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

// Reads digital button value (button reads 0 if pressed). This function allows for
// switch functionality instead of a continuous output.
bool button_read(int n){
  bool button = digitalRead(n);
  if (button == 0){
    return 1;
  }
  else {return 0;}
}


#define N 3       // smoothing window size (must be odd)
#define THRESHOLD 50   // detects if the movement is too eratic
#define FLEX 2    // number of buttons programmed

int mode = 0;   // different modes of operation for accelerometer.

int x_box[N];  // smoothing arrays
int y_box[N];

bool flex_state[FLEX];   //state of current flex sensors

int x_raw, x_smooth;  // accelerometer variables
int y_raw, y_smooth;
//int z_raw, z_smooth;

long x_total = 0;    // smoothing variables
long y_total = 0;
int i = 0;
int j = 0;
int x_av = 0;
int y_av = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Set digital inputs to high for buttons
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

}

void loop() {
  
  x_smooth = x_smoothing();   // smoothing algorithm
  y_smooth = y_smoothing();
 
  BeanMidi.enable();   // allows bean to send midi data through bluetooth

  // different modes allow for accelerometer control to turn on or off.
  // mode changed through cycling button pushes.
  
  // mode 0 has both sensors on
  if (mode == 0){
    BeanMidi.sendMessage(CONTROLCHANGE, 18, y_smooth);
    BeanMidi.sendMessage(CONTROLCHANGE, 19, x_smooth);
  }

  // mode 1 has horizontal movement on only
  if (mode == 1){
    BeanMidi.sendMessage(CONTROLCHANGE, 18, y_smooth);
    delay(50);
  }

  // mode 2 has vertical movement on only
  if (mode == 2){
    BeanMidi.sendMessage(CONTROLCHANGE, 19, x_smooth);
    delay(50);
  }
// mode 3 does not transmit any accelerometer data reading (no code for mode 3).


  // Flex data is used as switches by bending the specified finger.
  
  // index finger
  if(flex_read(0)){
    BeanMidi.noteOn(CHANNEL4, NOTE_C3, 100);
    delay(20);
    BeanMidi.noteOff(CHANNEL4, NOTE_C3, 100);
  }

  // middle finger
  if(flex_read(1)){
    BeanMidi.noteOn(CHANNEL5, NOTE_D3, 100);
    delay(20);
    BeanMidi.noteOff(CHANNEL5, NOTE_D3, 100);
  }

  // ring finger
  if(flex_read(2)){
    BeanMidi.noteOn(CHANNEL6, NOTE_E3, 100);
    delay(20);
    BeanMidi.noteOff(CHANNEL6, NOTE_E3, 100);
  }

  // pinky finger
  if(flex_read(3)){
    BeanMidi.noteOn(CHANNEL7, NOTE_F3, 100);
    delay(20);
    BeanMidi.noteOff(CHANNEL7, NOTE_F3, 100);
  }

  // buttons are used as switches by pushing them
  if(button_read(0)){
    BeanMidi.noteOn(CHANNEL1, NOTE_A3, 100);
    delay(20);
    BeanMidi.noteOff(CHANNEL1, NOTE_A3, 100);
  }
  
  if(button_read(1)){
    BeanMidi.noteOn(CHANNEL2, NOTE_B3, 100);
    delay(20);
    BeanMidi.noteOff(CHANNEL2, NOTE_B3, 100);
  }

  // Button 2 is configured to switch accel mode
  if (button_read(2)){
    mode += 1;
    if (mode > 3){
      mode = 0;
    }
    delay(50);
  }

  if(button_read(3)){
    BeanMidi.noteOn(CHANNEL3, NOTE_C4, 100);
    delay(20);
    BeanMidi.noteOff(CHANNEL3, NOTE_C4, 100);
  }

// SERIAL TEST DATA
  //Serial.println(y_raw - y_smooth);
  //Serial.println(y_smooth);
  //Serial.println(digitalRead(0));
  //Serial.println(analogRead(0));
  //Serial.println(mode);

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


// Reads analog flex sensor data. If the bend value is greater than a threshold, return 1
// ** Flex sensors act as switches as opposed to a range of values.
bool flex_read(int k){
  int flex = analogRead(k);
  if (flex > 900){
    return 1;
  }
  else {return 0;}
}


// Takes the average of a specified N number of values and returns that average 
// for smoother stream.
 
int x_smoothing(){

  // reads raw input of the x-axis (vertical).
  AccelerationReading reading = Bean.getAcceleration(); 
  x_raw = ((abs((reading.xAxis + 511)) / 8) - 32)*2;

  // If value jump is too great, function acknowledges an erratic movement and doesn't 
  // register it.
  if (x_av !=0 && abs(x_raw - x_av) > THRESHOLD){ 
    x_raw = x_av;
  }

  // stores and updates past 5 raw values for averaging.
  x_total -= x_box[i];
  x_total += x_raw;
  x_box[i] = x_raw;
  i += 1;
  if (i >= N){
    i = 0;
  }
  x_av = x_total/N;
  
  return x_av;  // returns smoother data
}

// same algorithm for the y-axis (horizontal)
int y_smoothing(){

  AccelerationReading reading = Bean.getAcceleration(); 
  y_raw = ((abs((reading.yAxis + 511)) / 8) - 32)*2;

  if (y_av !=0 && abs(y_raw - y_av) > THRESHOLD){
    y_raw = y_av;
  }

  y_total -= y_box[i];
  y_total += y_raw;
  y_box[i] = y_raw;
  j += 1;
  if (j >= N){
    j = 0;
  }
  y_av = y_total/N;
  
  return y_av;
}



// ------------------------------- Includes -----------------------------------------------//
#include <Bounce.h>
#include <Wire.h>
#include <SPI.h>
#include <microsmooth.h>
#include "I2Cdev.h"
#include "MPU6050.h"
//#include <Adafruit_NeoPixel.h>

// ------------------------------ Definitions ---------------------------------------------//


// Earth's magnetic field varies by location. Add or subtract
// a declination to get a more accurate heading.
//#define DECLINATION 3.52 // Declination (degrees) in Chicago, IL.

// define the button pins (digital)
#define SHIFT     1    // Shift pin
#define MODE      0    // Accel. mode pin
#define ON        2    // Device on/off switch pin
#define PLAY      3    // Play/Stop switch pin

#define UP        5    // D-Pad up direction pin
#define DOWN      4    // D-Pad down direction pin
#define LEFT      7    // D-Pad left direction pin
#define RIGHT     6    // D-Pad right direction pin

#define P_TIP     11    // Pointer tip pin
#define M_TIP     9   // Middle tip pin
#define R_TIP     10   // Ring tip pin

#define VIBE      21   // Vibration motor pin (digital)

#define THRESHOLD   200  // double click must happen between this time
#define HOLDTIME    1000 // ms hold period: how long to wait for press+hold event

// define the flex sensor pins (analog)
#define THUMB     0   // Thumb pin
#define POINTER   6   // Pointer Finger pin
#define MIDDLE    3   // Middle Finger pin
#define RING      2   // Ring Finger pin
#define PINKY     1   // Pink Finger pin


// define the LED pins for feedback from audio software (digital)
#define LED1       13    // Blue LED
#define LED2       22    // Red LED


#define XTHRESH    5000
#define YTHRESH    5000
#define ZTHRESH    7000


// -------------------------- Initiallize Variables ----------------------------------//

// shift variables
int shift;
int shift_cc;
byte shift_state = LOW;
byte previous_state = LOW;

// mode variables
int mode;
int clicks;
int count;

int current, init;
int mapper, prev;

int16_t ax, ay, az;
int16_t x_min, x_max, y_min, y_max, z_min, z_max;
int16_t axp = 0, ayp = 0, azp = 0;
int pre = 0;
int16_t x_change, y_change, z_change;
int16_t gx, gy, gz;
float roll, pitch;
int MIDI_roll, MIDI_pitch;
MPU6050 accelgyro;   // define the accelerometer struct

byte flex_state[6] = { };
byte prev_flex[6] = { };
bool p_flex, m_flex;
int flex[6] = { };
bool gest[6] = { };
int flexMin[6] = {150, 150, 150, 150, 150, 150};
int flexMax[6] = {200, 200, 200, 200, 200, 200};

// button initializers, the bounce data type debounces buttons every time a press is detected
Bounce b_shift = Bounce(SHIFT, 5);
Bounce b_mode = Bounce(MODE, 5);
Bounce b_on = Bounce(ON, 5);
Bounce b_play = Bounce(PLAY, 5);
Bounce d_up = Bounce(UP, 5);
Bounce d_down = Bounce(DOWN, 5);
Bounce d_left = Bounce(LEFT, 5);
Bounce d_right = Bounce(RIGHT, 5);
Bounce p_tip = Bounce(P_TIP, 5);
Bounce m_tip = Bounce(M_TIP, 5);
Bounce r_tip = Bounce(R_TIP, 5);

bool blink_state = LOW;

// -------------------------- Initiallize Functions --------------------------------------//

void shift_read();
void click_read();
void mode_read();

void button_check();
boolean button_read(int i);

void short_vibe();
void mid_vibe();
void long_vibe();
void multi_vibe(int times);

//int flex_val();
void gesture_command();
void flex_check();
boolean flex_read(int k);
void callibrate();

void getRotation();
void checkMovement();

void blink_led(int u);


// ------------------------------- Setup -------------------------------------------------//

void setup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif

  Serial.begin(115200);

  // initialize vibration pin as output
  pinMode(VIBE, OUTPUT);

  // initialize all digital pins as HIGH for reading button pushes
  pinMode(SHIFT, INPUT_PULLUP);
  pinMode(MODE, INPUT_PULLUP);
  pinMode(ON, INPUT_PULLUP);
  pinMode(PLAY, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);
  pinMode(P_TIP, INPUT_PULLUP);
  pinMode(M_TIP, INPUT_PULLUP);
  pinMode(R_TIP, INPUT_PULLUP);

  accelgyro.initialize();

  // offsets a known accelerometer and gyroscope value to stabilize
  accelgyro.setXAccelOffset(1297);
  accelgyro.setYAccelOffset(-251);
  accelgyro.setZAccelOffset(1509);

  accelgyro.setXGyroOffset(70);
  accelgyro.setYGyroOffset(19);
  accelgyro.setZGyroOffset(35);

  mapper = 0;
  mode = 0;

  // initialize LED pins as outputs
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
  //  pinMode(LED3, OUTPUT);
  //  pinMode(LED4, OUTPUT);

//
//  delay(500);
//  accel_cal();
//  flex_cal();
}

void loop() {
  
  button_check();  // check remaining buttons to send MIDI out
  //flex_check();

  if (shift == 0){ flex_check();}  // check flex sensors for note data 
  if(shift == 14){ gesture_commands();}    // check for different types of gestures and issue command

  if (mode == 0){ 
    getRotation();
    digitalWrite(LED1, LOW);
  }
  if (mode == 1){ 
    checkMovement();
    digitalWrite(LED1, HIGH);
  }
  if (mode == 2){ blink_led(1);}

  //while (usbMIDI.read()) ; // read and discard any incoming MIDI messages
}


// ------------------------------ BUTTON FUNCTIONS ---------------------------------------------//

// Button data is used as switches.
// buttons store current state
void button_check() {
  
  b_shift.update();
  b_mode.update();
  b_on.update();
  b_play.update();
  d_up.update();
  d_down.update();
  d_left.update();
  d_right.update();
  p_tip.update();
  m_tip.update();
  r_tip.update();

  // shift button
  if (b_shift.fallingEdge()){
    shift_read();
  }

  // mode button
  if(b_mode.fallingEdge()){
    click_read();
  }
  
  // device on/off switch
  if (b_on.fallingEdge()) {
    usbMIDI.sendNoteOn(55 + shift, 99, 1);
  }
  if (b_on.risingEdge()) {
    usbMIDI.sendNoteOn(55 + shift, 0, 1);
  }

  // play/stop button
  if (b_play.fallingEdge()) {
    usbMIDI.sendNoteOn(56 + shift, 99, 1);
  }
  if (b_play.risingEdge()) {
    usbMIDI.sendNoteOn(56 + shift, 0, 1);
  }

  // D-pad up
  if (d_up.fallingEdge()) {
    usbMIDI.sendNoteOn(51 + shift, 99, 1);
  }
  if (d_up.risingEdge()) {
    usbMIDI.sendNoteOn(51 + shift, 0, 1);
  }

  // D-pad down
  if (d_down.fallingEdge()) {
    usbMIDI.sendNoteOn(52 + shift, 99, 1);
  }
  if (d_down.risingEdge()) {
    usbMIDI.sendNoteOn(52 + shift, 0, 1);
  }

  // D-pad left
  if (d_left.fallingEdge()) {
    usbMIDI.sendNoteOn(53 + shift, 99, 1);
  }
  if (d_left.risingEdge()) {
    usbMIDI.sendNoteOn(53 + shift, 0, 1);
  }

  // D-pad right
  if (d_right.fallingEdge()) {
    usbMIDI.sendNoteOn(54 + shift, 99, 1);
  }
  if (d_right.risingEdge()) {
    usbMIDI.sendNoteOn(54 + shift, 0, 1);
  }

  // Pointer tip 
  if (p_tip.fallingEdge()) {
    usbMIDI.sendNoteOn(43 + shift, 99, 1);
  }
  if (p_tip.risingEdge()) {
    usbMIDI.sendNoteOn(43 + shift, 0, 1);
  }

  // Middle tip
  if (m_tip.fallingEdge()) {
    usbMIDI.sendNoteOn(44 + shift, 99, 1);
  }
  if (m_tip.risingEdge()) {
    usbMIDI.sendNoteOn(44 + shift, 0, 1);
  }

  // Ring tip
  if (r_tip.fallingEdge()) {
    usbMIDI.sendNoteOn(45 + shift, 99, 1);
  }
  if (r_tip.risingEdge()) {
    usbMIDI.sendNoteOn(45 + shift, 0, 1);
  }
}

// if shift is engaged, changes all MIDI data to alt. value
void shift_read() {
  
  shift_state = !shift_state;
    
  if (shift_state == 1) {    // once shift is pressed,
                               // all MIDI note values increase by 14
                               // all MIDI cc values incread by 64
    shift = 14;
    shift_cc = 64;
    digitalWrite(LED2, HIGH);
  }
  else {                    // resests MIDI notes and cc to original values
    shift = 0;
    shift_cc = 0;
    digitalWrite(LED2, LOW);
  }
}

// different presses result in different commands

void click_read(){
  clicks = 1;
  init = millis();
  current = millis() - init;

  // double click test
  b_mode.update();
  if (b_mode.risingEdge()){
    init = millis();
    current = millis() - init;
    while (current < THRESHOLD){
      b_mode.update();
      if(b_mode.fallingEdge()){
        clicks = 2;
        }
      current = millis() - init;
    }
  }

  // hold mode test
  else{
    while (!b_mode.risingEdge()){
      if (current > HOLDTIME){
        clicks = 3;
        break;
      }
      current = millis() - init;
      b_mode.update();
    }
  }
  

  switch (clicks){
    // Single Click command
    case 1:
      mode++;
      //mode_read();
      if (mode == 3){ 
        mode = 0;
      }
      break;
    // Double Click command
    case 2:
      Serial.println("Double click detected.");
      break;
    // Long Hold command
    case 3:
      Serial.println("Hold mode detected. Starting callibration:");
      flex_cal();
      accel_cal();
      break;
    // no click occured or too many clicks too fast (>2)
    default:
      break;
  }
}

// ------------------------------------ VIBRATIONS -----------------------------------------------//

void short_vibe(){
  digitalWrite(VIBE, HIGH);
  delay(50);
  digitalWrite(VIBE, LOW);
}

void mid_vibe(){
  digitalWrite(VIBE, HIGH);
  delay(75);
  digitalWrite(VIBE, LOW);
}

void long_vibe(){
  digitalWrite(VIBE, HIGH);
  delay(100);
  digitalWrite(VIBE, LOW);
}

void multi_vibe(int times){
  for (int v = 0; v < times; v++){
    digitalWrite(VIBE, HIGH);
    delay(40);
    digitalWrite(VIBE, LOW);
  }
}

// ---------------------------------- FLEX FUNCTIONS ---------------------------------------------//

boolean flex_read(int k) {
  prev_flex[k] = flex_state[k];
  if (analogRead(k) < 150) {
    flex_state[k] = 1;
  }
  else{
     flex_state[k] = 0;
  }
  
  if (flex_state[k] == 1 && flex_state[k] != prev_flex[k]) {
    return 1;
  }
  else {
    return 0;
  }
}

void flex_check() {
  // thumb
  if (flex_read(THUMB)) {
    usbMIDI.sendNoteOn(46 + shift, 99, 2);
    short_vibe();
    usbMIDI.sendNoteOff(46 + shift, 0, 2);
  }

  // index finger
  if (flex_read(POINTER)) {
    usbMIDI.sendNoteOn(47 + shift, 99, 2);
    short_vibe();
    usbMIDI.sendNoteOff(47 + shift, 0, 2);
  }
  // middle finger
  if (flex_read(MIDDLE)) {
    usbMIDI.sendNoteOn(48 + shift, 99, 2);
    short_vibe();
    usbMIDI.sendNoteOff(48 + shift, 0, 2);
  }
  // ring finger
  if (flex_read(RING)) {
    usbMIDI.sendNoteOn(49 + shift, 99, 2);
    short_vibe();
    usbMIDI.sendNoteOff(49 + shift, 0, 2);
  }
  //pinky finger
  if (flex_read(PINKY)) {
    usbMIDI.sendNoteOn(50 + shift, 99, 2);
    short_vibe();
    usbMIDI.sendNoteOff(50 + shift, 0, 2);
  }
}

void gesture_commands(){
  
  init = millis();
  current = millis() - init;
  while (current < 75) {
    for (int j = 0; j<7; j++){
      if (flex_read(j)){
        gest[j] = HIGH;
      }
    }
    current = millis() - init; 
  }
  
  if (gest[PINKY]){ mapper = mapper + 1;}
  if (gest[RING]){ mapper = mapper + 10;}
  if (gest[MIDDLE]){ mapper = mapper + 100;}
  if (gest[POINTER]){ mapper = mapper + 1000;}
  if (gest[THUMB]){ mapper = mapper + 10000;}

  init = millis();

  switch (mapper){
  // thumbs up
  case 1111:
    usbMIDI.sendNoteOn(30, 99, 1);
    usbMIDI.sendNoteOff(30, 0, 1);
    delay(400);
    break;
  // fist
  case 11111:
    usbMIDI.sendNoteOn(31, 99, 1);
    usbMIDI.sendNoteOff(31, 0, 1);
    long_vibe();
    delay(400);
    break;
  
  // pointer
  case 10111:
    usbMIDI.sendNoteOn(32, 99, 1);
    usbMIDI.sendNoteOff(32, 0, 1);
    delay(400);
    break;

  // peace
  case 10011:
    usbMIDI.sendNoteOn(33, 99, 1);
    usbMIDI.sendNoteOff(33, 0, 1);
    delay(400);
    break;
  
  // three
  case 10001:
    usbMIDI.sendNoteOn(34, 99, 1);
    usbMIDI.sendNoteOff(34, 0, 1);
    delay(400);
    break;  
  
  // four
  case 10000:
    usbMIDI.sendNoteOn(35, 99, 1);
    usbMIDI.sendNoteOff(35, 0, 1);
    delay(400);
    break;

  // rock
  case 10110:
    usbMIDI.sendNoteOn(36, 99, 1);
    usbMIDI.sendNoteOff(36, 0, 1);
    delay(400);
    break;

  // hang-10
  case 01110:
    usbMIDI.sendNoteOn(37, 99, 1);
    usbMIDI.sendNoteOff(37, 0, 1);
    delay(400);
    break;
      
  // if none of the gestures, just exit.
  default:
    break;
  }
  mapper = 0;
  for(int j = 0; j<7; j++){
    gest[j] = LOW;
  }
}

void flex_cc() {
  p_flex = analogRead(POINTER);
  p_flex = constrain(flex[POINTER], flexMin[POINTER], flexMax[POINTER]);
  p_flex = map(flex[POINTER], flexMin[POINTER], flexMax[POINTER], 0, 127);

  m_flex = analogRead(MIDDLE);
  m_flex = constrain(flex[MIDDLE], flexMin[MIDDLE], flexMax[MIDDLE]);
  m_flex = map(flex[MIDDLE], flexMin[MIDDLE], flexMax[MIDDLE], 0, 127);

  usbMIDI.sendControlChange(18 + shift_cc, p_flex, 2);
  usbMIDI.sendControlChange(19 + shift_cc, m_flex, 2);
  //  usbMIDI.sendControlChange(18, flex[2], 1);
  //  usbMIDI.sendControlChange(19, flex[0], 1);
}

// ------------------------------------- ACCEL FUNCTIONS ---------------------------------------------//
void checkMovement() {
  accelgyro.getAcceleration(&ax, &ay, &az);

  x_change = ax - axp;
  axp = ax;
  y_change = ay - ayp;
  ayp = ay;
//  z_change = az - azp;
//  azp = az;
//  Serial.print(z_change);
//  Serial.print("\t");

  if (x_change > XTHRESH && pre != 1){
    pre = 1;
    usbMIDI.sendNoteOn(38, 99, 1);
    usbMIDI.sendNoteOff(38, 0, 1);
  }
//  else if (x_change < -XTHRESH && pre != 2 && pre !=1 ){
//    pre = 2;
//    usbMIDI.sendNoteOn(39, 99, 1);
//    Serial.println("x- detected");
//    usbMIDI.sendNoteOff(39, 0, 1);
//  }
  if (y_change > YTHRESH && pre != 3){
    pre = 3;
    usbMIDI.sendNoteOn(40, 99, 1);
    usbMIDI.sendNoteOff(40, 0, 1);
  }
//  else if (y_change < -YTHRESH && pre != 4 && pre != 3){
//    pre =4;
//    usbMIDI.sendNoteOn(40, 99, 1);
//    Serial.println("y- detected");
//    usbMIDI.sendNoteOff(40, 0, 1);
//  }
//  if (z_change > ZTHRESH && pre != 5){
//    pre = 5;
//    usbMIDI.sendNoteOn(42, 99, 1);
//    Serial.println("z+ detected");
//    usbMIDI.sendNoteOff(42, 0, 1);
//  }
//  else if (z_change < -ZTHRESH && pre != 6){
//    pre = 6;
//    usbMIDI.sendNoteOn(42, 99, 1);
//    Serial.println("z- detected");
//    usbMIDI.sendNoteOff(42, 0, 1);
//  }
}

void getRotation() {
  //get latest accel data
  accelgyro.getAcceleration(&ax, &ay, &az);

  // roll calculates horizontal rotation (around x-axis)
  roll = atan2(ax, az);
  roll *= (180.0 / PI);            // convert to degrees
  roll = abs(-roll + 80);          // reverse axis and have range be all positive
  if (roll > 180) {
    roll = 180; //ceiling
  }

  MIDI_roll = map (roll, 2, 180, 0, 127);    // converts to MIDI values 0-127

  // pitch calculates vertical rotation (around y-axis)
  pitch = atan2(-ay, sqrt(ax * ax + az * az));
  pitch *= (180.0 / PI);           // convert to degrees
  pitch = abs(-pitch + 90);        // reverse axis and have range be all positive
  if (pitch < 16) {
    pitch = 16; //floor
  }

  MIDI_pitch = map(pitch, 16, 171, 0, 127);     // convert to MIDI values 0-127

  // test accel.
  //  Serial.println(MIDI_roll);
  //  Serial.println(MIDI_pitch);

  usbMIDI.sendControlChange(16 + shift_cc, MIDI_roll, 2);
  usbMIDI.sendControlChange(17 + shift_cc, MIDI_pitch, 2);
}

// --------------------------------------- LED FUNCTIONS -----------------------------------------//
//
//void LED_set(int j){
//  // write MIDI read functions for specified messages
//}

void blink_led(int u){
  blink_state = !blink_state;
  if (u == 1){ digitalWrite(LED1, blink_state);}
  if (u == 2){ digitalWrite(LED2, blink_state);}
}

// --------------------------------------- CALLIBRATIONS --------------------------------------------//

void flex_cal(){
  Serial.println("Calibrating flex sensors... ");
  Serial.println("Bend and flex all fingers multiple times");

  init = millis();
  current = millis() - init;

  while (current < 4000) {
    for (int i = 0; i < 7; i++) { //read all flex values once
      flex[i] = analogRead(i);
      if (flex[i] < flexMin[i]) {
        flexMin[i] = flex[i]; //update sensor MINs
      }
      else if (flex[i] > flexMax[i]) {
        flexMax[i] = flex[i]; //update sensor MAXs
      }
    }
    current = millis() - init;
  }

  Serial.print("pointer min/max: ");
  Serial.print(flexMin[POINTER]);
  Serial.print("/ ");
  Serial.println(flexMax[POINTER]);

  Serial.print("middle min/max: ");
  Serial.print(flexMin[MIDDLE]);
  Serial.print("/ ");
  Serial.println(flexMax[MIDDLE]);

  Serial.print("ring min/max: ");
  Serial.print(flexMin[RING]);
  Serial.print("/ ");
  Serial.println(flexMax[RING]);

  Serial.print("pointer min/max: ");
  Serial.print(flexMin[PINKY]);
  Serial.print("/ ");
  Serial.println(flexMax[PINKY]);

  Serial.print("thumb min/max: ");
  Serial.print(flexMin[THUMB]);
  Serial.print("/ ");
  Serial.println(flexMax[THUMB]);
}

void accel_cal(){
  Serial.println("Calibrating accel/gyro...");
  Serial.println("Don't make large movements");

  init = millis();
  current = millis() - init;

  while (current < 4000) {

    accelgyro.getAcceleration(&ax, &ay, &az);

    if ( ax > x_max) { x_max = ax;}
    else if (ax < x_min) { x_min = ax;}

    if ( ay > y_max) { y_max = ay;}
    else if (ay < y_min) { y_min = ay;}

    if ( az > z_max) { z_max = az;}
    else if (az < z_min) { z_min = az;}

    current = millis() - init;
  }

  Serial.print("x max/min: ");
  Serial.print(x_max);
  Serial.print(" ,");
  Serial.println(x_min);

  Serial.print("y max/min: ");
  Serial.print(y_max);
  Serial.print(" ,");
  Serial.println(y_min);

  Serial.print("z max/min: ");
  Serial.print(z_max);
  Serial.print(" ,");
  Serial.println(z_min);
}

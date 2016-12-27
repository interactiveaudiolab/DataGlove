#define filter_size 9

bool button0, button1, button2, button3;

int arr_flex0[filter_size];
int arr_flex1[filter_size];

int flex0_raw, flex0_smooth;
int flex1_raw, flex1_smooth;

int arr_x[filter_size];
int arr_y[filter_size];
int arr_z[filter_size];

int x_raw, x_smooth;
int y_raw, y_smooth;
int z_raw, z_smooth;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Set digital inputs to high
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

}


void loop(){
  send_accel_midi();
  BeanMidi.enable();
  BeanMidi.sendMessage(CONTROLCHANGE, 18, x_smooth);
  BeanMidi.sendMessage(CONTROLCHANGE, 19, y_smooth);

  delay(15);
  
  //button_read();
  
  //bool done = 1;

//  while(done){
//
//    if(button0 == 1){
//      BeanMidi.sendMessage(CONTROLCHANGE, 18, y_smooth);
//      done =0;
//      break;
//    }
//
//    if(button1 == 1){
//      BeanMidi.sendMessage(CONTROLCHANGE, 19, x_smooth);
//      done = 0;
//      break;
//    }

   // else {
      //BeanMidi.sendMessage(CONTROLCHANGE, 18, x_smooth);
      //BeanMidi.sendMessage(CONTROLCHANGE, 19, y_smooth);
    //}
 // }
}


void button_read(){
  button0 = digitalRead(0);
  button1 = digitalRead(1);
  //button2 = digitalRead(2);
  //button3 = digitalRead(3);
}

void send_flex_midi(){
  flex0_raw = abs(analogRead(0) -725);
  flex0_smooth = smoothing(flex0_raw, arr_flex0);
  
  flex1_raw = abs(analogRead(1) -725);
  flex1_smooth = smoothing(flex1_raw, arr_flex1);

  BeanMidi.enable();
  BeanMidi.sendMessage(CONTROLCHANGE, 16, flex0_smooth);
  BeanMidi.sendMessage(CONTROLCHANGE, 17, flex1_smooth);
}

void send_accel_midi(){
    AccelerationReading reading = Bean.getAcceleration(); 
    x_raw = abs((reading.xAxis + 511)) / 8;
    x_smooth = smoothing(x_raw, arr_x);
 
    y_raw = abs((reading.yAxis + 511)) / 8;
    y_smooth = smoothing(y_raw, arr_y);

    z_raw = abs((reading.zAxis + 511)) / 8;
    z_smooth = smoothing(z_raw, arr_z);  

    Bean.setLed(x_smooth, y_smooth, z_smooth);

    //BeanMidi.enable();
    //BeanMidi.sendMessage(CONTROLCHANGE, 18, x_smooth);
    //BeanMidi.sendMessage(CONTROLCHANGE, 19, y_smooth);
}

int smoothing(int raw_in, int *axis_array){
  int j, k, temp, top, bottom;
  long total;
  static int i;
  static int sorted[filter_size];
  boolean done;

  i = (i + 1) % filter_size;    // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  axis_array[i] = raw_in; 

  for (j=0; j<filter_size; j++){
    sorted[j] = axis_array[j];
  }

// sorting function
  done = 0;
  while(done!= 1){
    done = 1;
    for (j = 0; j < (filter_size - 1); j++){
      if (sorted[j] > sorted[j + 1]){    
        temp = sorted[j + 1];
        sorted [j+1] =  sorted[j] ;
        sorted [j] = temp;
        done = 0;
  }

// throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((filter_size * 15)  / 100), 1); 
  top = min((((filter_size * 85) / 100) + 1), (filter_size - 1));
  k = 0;
  total = 0;
  for ( j = bottom; j< top; j++){
    total += sorted[j];  // total remaining indices
    k++; 
  }  

  return total / k;
    }
  }
}


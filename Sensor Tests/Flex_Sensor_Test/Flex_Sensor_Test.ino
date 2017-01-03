void setup() {
  Serial.begin(9600);

}


void loop()
{  
  // flex sensor values (w/ 10k resistor) range from ~750 to 950
  int flex_value0 = abs(analogRead(0) -725);
  int flex_value1 = abs(analogRead(1) -725);

  Serial.print("sensor 0 reads: ");
  Serial.println(flex_value0);
  Serial.print("sensor 1 reads: ");
  Serial.println(flex_value1);

  BeanMidi.enable();
  BeanMidi.sendMessage(CONTROLCHANGE, 16, flex_value0);
  BeanMidi.sendMessage(CONTROLCHANGE, 17, flex_value1);
  

  delay(20);

}


# Sound Gauntlet
A hardware/software project using a Bean+ and Arduino to build a glove for control of electronic music. The device integrates flex sensors 
and spacial positioning allowing the user to manipulate multiple audio parameters. Using a wireless bluetooth connection, the Bean+
microcontroller sends MIDI data to be interpreted by a Digital Audio Workstation (DAW). Any mappable parameter within the DAW can be
assigned to a sensor on the glove with immediate response. 

# Demos and Tests
A playlist of demonstrations of the device in action. This playlist is constantly updated.

[**Youtube Playlist**](https://www.youtube.com/watch?v=zSRMGvY-cKg&list=PLb5gDGqPoS3pkmFexbJDEEukBEd1r7V4p)
___

# Assembly Parts
The device consists of multiple sensors and fabric. To build your own, you will need the following physical components:
  - [**PunchThrough Bean+ Microcontroller**](http://store.punchthrough.com/collections/bean-family/products/lightblue-bean-plus) - This is the brain of the device. 
  All sensor data is read by this processor and communicated through Bluetooth to the DAW. It runs modified Arduino code and automatically 
  sends out MIDI data ready for interpretation.
  
  - **Accelerometer (Built-in)**
    The Bean+ uses a built-in accelerometer as one of the main device sensors. The accelerometer functions as an indicator of the 
    glove's position in space and sends x, y, and z dimensional values to be interpreted to parameters of the user's choosing.
  - [**Flex Sensors**](https://www.sparkfun.com/products/10264) - Flex sensors are attached at each finger to measure the a finger's 
  orientation. The sensors utilize a conductive fluid that can be measured for variable resistance values.
  - [**Switch Buttons**](https://www.sparkfun.com/products/10442) - Simple buttons used for binary commands such as a Play/Stop button, 
  bank selection, or power switch.
  - [**Programmable LEDs**](https://www.sparkfun.com/products/105) - LEDs are used as feedback for the user. It can let one know if the
  program is ready for use, it can blink at a particular tempo, etc.
  - [**Conductive Thread**](https://www.sparkfun.com/products/10867) - Allows for a light-weight method to connect the circuit. 
  - [**Flexible Wires**](https://www.sparkfun.com/products/10649) - For more sophisticated connections.

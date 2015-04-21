#include <plcLib_activeLOW.h>

/* Programmable Logic Controller Library for the Arduino and Compatibles

   inNot

   Connections:
   Input - Set - switch connected to input X0 (Arduino pin A0 / Tinkerkit pin I0)
   Input - Reset - switch connected to input X1 (Arduino pin A1 / Tinkerkit pin I1)
   Output - Q - LED connected to output Y0 (Arduino pin 3 / Tinkerkit O5)
   Output - NotQ - LED connected to output Y1 (Arduino pin 5 / Tinkerkit O4)

   Software and Documentation:
   http://www.electronics-micros.com/software-hardware/plclib-arduino/

*/
void setup() {
  // put your setup code here, to run once:
  setupPLC();  	    // Setup inputs and outputs
}

void loop() {
  /*
  |
  |    X0          X1               Y0
  |----|/|---------| |--------------( )----|
  |    Y0                           Y1
  |----| |--------------------------(/)----|
  */
  
  inNot(X0);           // Read switch connected to Input 0 (Set)
  andNotBit(X1);        // Self latch using Output 0 (Q)
  out(Y0);          // Output to Output 0 (Q)

  in(Y0);           // Read Q output 
  outNot(Y1);       // Produce inverted output on Output 1 (Not Q)
}

//#include <plcLib.h>
#include <plcLib_activeLOW.h>

/* Programmable Logic Controller Library for the Arduino and Compatibles

   Using Set and Reset commands to create a Set-Reset Latch

   Connections:
   Input - Set - switch connected to input X0 (Arduino pin A0 / Tinkerkit pin I0)
   Input - Reset - switch connected to input X1 (Arduino pin A1 / Tinkerkit pin I1)
   Output - Q - LED connected to output Y0 (Arduino pin 3 / Tinkerkit O5)
   Output - NotQ - LED connected to output Y1 (Arduino pin 5 / Tinkerkit O4)

   Software and Documentation:
   http://www.electronics-micros.com/software-hardware/plclib-arduino/

*/

void setup() {
  setupPLC();          // Setup inputs and outputs
}

void loop() {
  |      X1                   SY1
  |-----| |-------------------( )----|
  |      X2                   RY1
  |-----| |-------------------( )----|
  |
  in(X1);              // Read switch connected to Input 0 (Set input)
  set(Y1);             // Set Y0 to 1 if X0 = 1, leave Y0 unaltered otherwise

  in(X2);              // Read switch connected to X1
  reset(Y1);           // Clear Y0 to 0 if X1 = 1, leave Y0 unaltered otherwise
}

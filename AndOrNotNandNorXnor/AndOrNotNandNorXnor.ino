//#include <plcLib.h>
#include <plcLib_activeLOW.h>

/* Programmable Logic Controller Library for the Arduino and Compatibles

   AND, OR, XOR and Not - Basic Boolean Logic Functions

   Connections:
   Input - switch connected to input X0 (Arduino pin A0 / Tinkerkit pin I0)
   Input - switch connected to input X1 (Arduino pin A1 / Tinkerkit pin I1)
   Output - ANDed Output - LED connected to output Y0 (Arduino pin 3 / Tinkerkit O5)
   Output - ORed Output - LED connected to output Y1 (Arduino pin 5 / Tinkerkit O4)
   Output - XORed Output - LED connected to output Y2 (Arduino pin 6 / Tinkerkit O3)
   Output - Inverted Output - LED connected to output Y3 (Arduino pin 9 / Tinkerkit O2)

   Software and Documentation:
   http://www.electronics-micros.com/software-hardware/plclib-arduino/

*/
unsigned int R1

void setup() {
  setupPLC();  // Setup inputs and outputs
}

void loop() {
  /* Gerbang AND
  |    X0          X1          Y0 
  |----| |---------| |---------( )----|
  |  
  */
  in(X0);      // Read Input 0
  andBit(X1);  // AND with Input 1
  out(Y0);     // Send result to Output 0
   /* Gerbang ANDNOT
  |    X0          X1          Y0 
  |----| |---------| |---------( )----|
  |  
  */
  in(X0);      // Read Input 0
  orBit(R1);  // AND with Input 1
  andNotBit(X1);
  //out(R1);
  //in(R1); 
  out(Y1);     // Send result to Output 0
  /* Gerbang OR
  |    X0                      Y0 
  |----| |----|----------------( )----|
  |    X1     |
  |----| |----|  
  */
  
  /*
  in(X0);      // Read Input 0
  orBit(X1);   // OR with Input 1
  out(Y1);     // Send result to Output 1
  */
  /* Gerbang XOR
  |    X0          X1           Y0 
  |----| |---------|/|----|----( )----|
  |    X0          X1     |
  |----|/|---------| |----|  
  */  
  /*
  in(X0);      // Read Input 0
  xorBit(X1);  // XOR with Input 1
  out(Y2);     // Send result to Output 2
  */
  /* Gerbang XOR
  |    X0                      Y0 
  |----|/|----|----------------( )----|
  | 
  */  
  /*
  in(X0);      // Read Input 0
  outNot(Y3);  // Send inverted result to Output 3
  */
  /* Gerbang NAND
  |    X0                      Y0 
  |----|/|----|----------------( )----|
  |    X1     |
  |----|/|----|  
  */
  /*
  in(X0);      // Read Input 0
  nandBit(X1);   // OR with Input 1
  out(Y4);     // Send result to Output 1
  */
}


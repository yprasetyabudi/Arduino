//#include <plcLib.h>
#include <plcLib_activeLOW.h>

/* Programmable Logic Controller Library for the Arduino and Compatibles

   Comparator - Greater than test between two input pins

   Connections:
   Analogue Input - potentiometer connected to input X0 (Arduino pin A0 / Tinkerkit pin I0)
   Analogue Input - potentiometer connected to input X1 (Arduino pin A1 / Tinkerkit pin I1)
   Digital Output - LED connected to output Y0 (Arduino pin 3 / Tinkerkit O5)

   Software and Documentation:
   http://www.electronics-micros.com/software-hardware/plclib-arduino/

*/
   
void setup() {
  setupPLC();        // Setup inputs and outputs
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
}

void loop() {
  inAnalog(X0);      // Read Analogue Input 0
  compareGT(X1);     // X0 > X1 ?
  out(Y0);           // Y0 = 1 if X0 > X1, Y0 = 0 otherwise
    // cek
    Serial.print("sensor 0 = " );
    Serial.print(inAnalog(X0));
    Serial.print("\t sensor 1 = " );
    Serial.print(inAnalog(X1));
    Serial.print("\t compare = " );
    Serial.print(compareGT(X1));
    Serial.print("\t output = ");
    Serial.println(outPWM(Y0));
}

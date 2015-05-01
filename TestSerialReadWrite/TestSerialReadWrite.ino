#include <plcLib_activeLOW.h>
int incomingByte = 0;   // for incoming serial data

void setup() {
  // put your setup code here, to run once:
  setupPLC(); 
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  in(A1);
  out(Y0);
  /*
  Serial.print("input X0: ");
  Serial.print(in(X0));
  Serial.print("\t output Y0 :");
  Serial.println(Y0);
  delay(1);        // delay in between reads for stability 
  */
  if (Serial.available() > 0) {
                // read the incoming byte:
                incomingByte = Serial.read();

                // say what you got:
                Serial.print("I received: ");
                Serial.println(incomingByte);
               delay(100);        // delay in between reads for stability
        }
}

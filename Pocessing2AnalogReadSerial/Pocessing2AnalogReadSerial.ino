 //-------------------------------------------------------------------------
 //START OF ARDUINO SERIAL SERVER PREAMBLE
 //Defining constants corresponding to each command (also the ascii code number) 
 
 #define FORWARD 70       //F
 #define BACKWARD 66       //B
 #define LEFT 76           //L
 #define RIGHT 82         //R
 #define SETSPEED 83         //S
 #define STOP 67           //C
 #define SETSPEEDLEFT 88   //X
 #define SETSPEEDRIGHT 89 //Y
 
 /*
 The three check bytes (used to keep the robot from responding to random serial *data) currently "AAA" 
 */
 #define checkByte1 65   // "A"
 #define checkByte2 65   // "A"
 #define checkByte3 65   // "A" 
 
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
 
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(3, HIGH);
  digitalWrite(5, HIGH);
   Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  //int sensorValue = analogRead(A0);
  // print out the value you read:

  int dta;                              //variable to hold the serial  byte  
 
  if ( Serial.available() >= 5) {       //if 5 bytes are in the buffer (length pf a full request)    
      dta = Serial.read();     
  if ( dta = checkByte1){                        //Checks for first check byte        
      dta = Serial.read();        
  if ( dta = checkByte2){                    //Checks for second check byte
      dta = Serial.read();            
  if ( dta = checkByte3){                //Checks for third check byte
      int command = Serial.read();        //Fourth byte is the command
      int param1 = Serial.read();         //Fifth byte is param1
      interpretCommand(command, param1);  //sends the parsed request to it's handler
      Serial.print("DATA: ");
      Serial.print(command);
      Serial.print("\t ");
      Serial.println(param1);
      delay(1000);        // delay in between reads for stability
  
  }
  }
  }
  } else {
    Serial.println("NO DATA: ");
  }
  //Serial.begin(9600);
}

void interpretCommand(int command, int param1)
{
if(command == FORWARD)
{
digitalWrite(3,LOW);

delay(param1 * 100); 

//Serial.end();
}   //if forward   
 
else if(command == BACKWARD)
{
digitalWrite(3,HIGH);

delay(param1 * 100);
//Serial.end();
} //if backwards 
}

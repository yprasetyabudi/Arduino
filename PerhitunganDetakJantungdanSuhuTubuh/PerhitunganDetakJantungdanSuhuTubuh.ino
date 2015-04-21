#include "TimerOne.h"
#include "OneWire.h"

const int LEDHR=12;
int time;
int detak=1;
int heartRate;
int DS18S20_Pin = 3; //DS18S20 Signal pin on digital 3
float temperature;


// Global variable
const int THRESHOLD = 10;    
int heartBeat = 0;
float currentBeat = 0;
int beatPerMinute = 0;
long timeDiffBetween[THRESHOLD-1];
long totalDiffTime = 0;
long lastBeatTime = 0;
long newBeatTime = 0;
int beatCounter = 0;

 
OneWire ds(DS18S20_Pin); // on digital pin 3

void setup(){
Serial.begin(115200);
pinMode(12,INPUT);
}

void loop(){
  getHeartBeat2();
}


long timeSignal(){
  int isHigh = 0;
  int isLow = 0;
  int isFalling = 0;
  long totalTime = 0;
  long startTime = 0;
  long endTime = 0;
  
  while(digitalRead(LEDHR) == HIGH){
      if(isHigh == 0){
        startTime = millis();
        isHigh = 1;
      }
  }
     
  endTime = millis();
  
  if(isHigh){
      totalTime = endTime - startTime;
  }
  return totalTime;
}

void getHeartBeat2(){
    
    if(beatCounter >= THRESHOLD && digitalRead(LEDHR) ==  HIGH){

        // mendapatkan waktu beat terbaru
        newBeatTime = millis();

        // mencatat waktu yang terakhir akan dibuang
        long oldDiffTime = timeDiffBetween[beatCounter % THRESHOLD];
        
 // menghitung selisih waktu antara beat terbaru dengan beat terakhir
        timeDiffBetween[beatCounter % THRESHOLD] = newBeatTime - lastBeatTime;
        
        int temp = 0;
        totalDiffTime = 0;
        while(temp < THRESHOLD-1){
          totalDiffTime +=  timeDiffBetween[temp];
          temp++;        
        }
        //totalDiffTime -= oldDiffTime;
        //totalDiffTime += newBeatTime - lastBeatTime;

        // waktu beat terbaru akan menjadi waktu beat terakhir pada loop selanjutnya
        lastBeatTime = newBeatTime;

        // menghitung beat
        beatCounter++;
        timeSignal();
        
        // rumus heartBeat
        currentBeat = (60000.0/(float)(totalDiffTime)) * (THRESHOLD - 1);
        beatPerMinute = currentBeat;
              
        temperature = getTemp();
        //print detak jantung
        Serial.print(beatPerMinute);
        
 //print spliter
         Serial.print("|");

        //print temperatur
         Serial.println(temperature);

        delay(50);
    }
  
    while(beatCounter < THRESHOLD){
        if(digitalRead(LEDHR) ==  HIGH){
            if(beatCounter == 0){
              // set waktu beat terakhir untuk pertama kali  
              lastBeatTime = millis();
              beatCounter++;
            }else{

                  // mendapatkan waktu beat terbaru
                  newBeatTime = millis();

                  // menghitung selisih waktu antara beat terbaru dengan beat terakhir
                  timeDiffBetween[beatCounter-1] = newBeatTime - lastBeatTime;
                  totalDiffTime += newBeatTime - lastBeatTime;

                  // waktu beat terbaru akan menjadi waktu beat terakhir pada loop selanjutnya
                  lastBeatTime = newBeatTime;

                  // menghitung beat
                  beatCounter++;
                 
                  
            }     
            timeSignal();           
        }
    }
        
}


// penghitungan suhu menggunakan method getTemp()
float getTemp(){
   //returns the temperature from one DS18S20 in DEG Celsius
  
   byte data[12];
   byte addr[8];
  
   if ( !ds.search(addr)) {
     //no more sensors on chain, reset search
     ds.reset_search();
     return -1000;
   }
  
   if ( OneWire::crc8( addr, 7) != addr[7]) {
     //Serial.println("CRC is not valid!");
     return -1000;
   }
  
   if ( addr[0] != 0x10 && addr[0] != 0x28) {
     //Serial.print("Device is not recognized");
     return -1000;
   }
  
   ds.reset();
   ds.select(addr);
   ds.write(0x44,1); // start conversion, with parasite power on at the end
  
   byte present = ds.reset();
   ds.select(addr);  
   ds.write(0xBE); // Read Scratchpad
  
   
   for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
   }
   
   ds.reset_search();
   
   byte MSB = data[1];
   byte LSB = data[0];
  
   float tempRead = ((MSB << 8) | LSB); //using two's compliment
   float TemperatureSum = tempRead / 16;
   
   return TemperatureSum;
 
}

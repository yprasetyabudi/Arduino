


////////////////////////////////////////////////////////////////////////
//ETHERNET SWITCH PROGRAM
////////////////////////////////////////////////////////////////////////

// HAS A LOT OF MY U8GLIB ETC FUNCTIONS
// REMOVE IF NEEDED. THIS WILL SAVE SPACE
// I’M TOO LAZY TO DO IT FOR YOU ^^

//ARDUINO 1.0+ ONLY

#include <String.h>
#include <Ethernet.h>
#include <SPI.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <U8glib.h>
#include <Wire.h>
#include <DS1307new.h>
#include "M2tk.h"
#include "utility/m2ghu8g.h"

#define DEFAULT_FONT u8g_font_6x13
#define ICON_FONT u8g_font_m2icon_9
#define BIG_FONT u8g_font_fub20r
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 10
U8GLIB_ST7920_128X64_4X u8g(7, 5, 6, U8G_PIN_NONE); //(13 = SCK, 12 = CS, 11 = SID)


////////////////////////////////////////////////////////////////////////
//CONFIGURATION
////////////////////////////////////////////////////////////////////////

//IP manual settings
byte ip[] = { 
  192, 168, 2, 177 };   //Manual setup only LOOK FOR FORM ACTION @ END OF CODE
byte gateway[] = { 
  192, 168, 2, 1 }; //Manual setup only GATEWAY
byte subnet[] = { 
  255, 255, 255, 0 }; //Manual setup only SUBNET

// if need to change the MAC address (Very Rare)
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF };

//Ethernet Port
EthernetServer server = EthernetServer(80); //default html port 80

//The number of outputs going to be switched.
int outputQuantity = 16;  //should not exceed 10 

//Invert the output of the leds
boolean outputInverted = true; //true or false
// This is done in case the relay board triggers the relay on negative, rather then on positive supply

//Html page refresh
int refreshPage = 30; //default is 10sec. 
//Beware that if you make it refresh too fast, the page could become FROZEN.

//Display or hide the "Switch on all Pins" buttons at the bottom of page
int switchOnAllPinsButton = false; //true or false

OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
//DeviceAddress insideThermometer, outsideThermometer;
// arrays to hold device addresses
DeviceAddress insideThermometer= {
  0x28, 0xA4, 0x0E, 0x75, 0x04, 0x00, 0x00, 0x8B };
DeviceAddress outsideThermometer= {
  0x28, 0x49, 0xAC, 0x74, 0x04, 0x00, 0x00, 0xD7 };



//Button Array
//Just for a note, varables start from 0 to 9, as 0 is counted as well, thus 10 outputs.
// Select the pinout address

byte outputAddress[8] = {
  22,23,24,25,26,27,28,29}; //Allocate 10 spaces and name the output pin address.
byte outputAddress2[8]= {
  30,31,32,33,34,35,36,37};
//PS pin addresses 10, 11, 12 and 13 on the Duemilanove are used for the ethernet shield, therefore cannot be used.
//PS pin addresses 10, 50, 51 and 52 and 53 on the Mega are used for the ethernet shield, therefore cannot be used.
//PS pin addresses 4, are used for the SD card, therefore cannot be used.
//PS. pin address 2 is used for interrupt-driven notification, therefore could not be used.

// Write the text description of the output channel
String buttonText[16] = {
  "01. Right Lamp","02. Left Lamp","03. Bedroom","04. Kitchen","05. Water Heater","06. Gate","07. Toilet","08. Main Heater","09. Roof Light","10. Basement","11. Test","12. TEST2","13. TEST3","14. TEST4","15. TEST5","16. TEST6"};

// Set the output to retain the last status after power recycle.
int retainOutputStatus[16] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//1-retain the last status. 0-will be off after power cut.

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//VARIABLES DECLARATION
////////////////////////////////////////////////////////////////////////

boolean printLastCommandOnce = false;
boolean printButtonMenuOnce = false;
boolean initialPrint = true;
String allOn = "";
String allOff = "";
boolean reading = false;
boolean outputStatus[8]; //Create a boolean array for the maximum ammount.
boolean outputStatus2[8]; // relays 9-16
unsigned long timeConnectedAt;
boolean writeToEeprom = false;
int outp =0;
float tempI = 0.0;
float tempO = 0.0;
uint8_t td_hour;
uint8_t td_min;
uint8_t td_sec;
int boxX = 0;
int boxY = 53;
int boxHeight = 11;
int boxWidth = 128;
String readString; //
boolean login=false;


float newRange = 10; //height of the frame for histogram

////////////////////////////////////////////////////////////////////////
//RUN ONCE
////////////////////////////////////////////////////////////////////////
//Beginning of Program
void setup(){
  Serial.begin(57600);

  initEepromValues();
  readEepromValues();

  //Set pins as Outputs 
  boolean currentState = false;
  for (int var = 0; var < 9; var++){
    digitalWrite(outputAddress[var], HIGH);
    pinMode(outputAddress[var], OUTPUT); 

  }
  for (int var2 = 0; var2 < 9; var2++){

    pinMode(outputAddress2[var2], OUTPUT); 
    digitalWrite(outputAddress2[var2], HIGH);
  }


  //Setting up the IP address. Comment out the one you dont need.
  //Ethernet.begin(mac); //for DHCP address. (Address will be printed to serial.)
  Ethernet.begin(mac, ip, gateway, subnet); //for manual setup. (Address is the one configured above.)


  server.begin();
  //Serial.print("Server started at ");
  //Serial.println(Ethernet.localIP());
  sensors.begin();

  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  if (!sensors.getAddress(outsideThermometer, 1)) Serial.println("Unable to find address for Device 1"); 

  sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);

}


////////////////////////////////////////////////////////////////////////
//LOOP
////////////////////////////////////////////////////////////////////////
//Run once
void loop(){


  //Serial.println(lcdrelay1);
  sensors.requestTemperatures();  

  printTemperature(insideThermometer);
  //Serial.println(tempI);
  printTemperature2(outsideThermometer);
  //Serial.println(tempO);

  u8g.firstPage();  
  do {
    draw();
  } 
  while( u8g.nextPage() );

  // listen for incoming clients, and process requests.
  checkForClient();
  
}

////////////////////////////////////////////////////////////////////////
//checkForClient Function
////////////////////////////////////////////////////////////////////////
void td_get_from_RTC(void)
{
  RTC.getTime();
  td_hour = RTC.hour;
  td_min = RTC.minute;
  td_sec = RTC.second;
}

void td_put_to_RTC(void)
{
  RTC.getTime();
  RTC.fillByHMS(td_hour, td_min, td_sec);
  RTC.setTime();
  RTC.startClock();  
}
void checkForClient(){

  EthernetClient client = server.available();

  if (client) {

    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    boolean sentHeader = false;
    boolean login = false;

    while (client.connected()) {
      if (client.available()) {
        //if header was not set send it

        //read user input
        char c = client.read();
        Serial.println(c);
        readString.concat(c);
        if(c == '*'){

          printHtmlHeader(client); //call for html header and css
          printLoginTitle(client);
          printHtmlFooter(client);
          //sentHeader = true;
          login=false;
          break;
        }

        if(!sentHeader){

          printHtmlHeader(client); //call for html header and css
          printHtmlButtonTitle(client); //print the button title

          //This is for the arduino to construct the page on the fly. 
          sentHeader = true;
        }

        //if there was reading but is blank there was no reading
        if(reading && c == ' '){
          reading = false;
          login=false;

        }
        if(c == '?') {
          reading = true; //found the ?, begin reading the info


        }
        if (login==false) {
            if(readString.indexOf("User=user&Pass=user") > 0) {
              login=true;

            }
          }

        //if there is a ? there was user input


        // if there was user input switch the relevant output
        if(login && reading){

          //if user input is H set output to 1
          if(c == 'H') {
            outp = 1;
          }

          //if user input is L set output to 0
          if(c == 'L') {
            outp = 0;
          }


          //Serial.print(c);   //print the value of c to serial communication
          //Serial.print(outp);
          //Serial.print('\n');

          switch (c) {

          case '0':
            //add code here to trigger on 0
            triggerPin(outputAddress[0], client, outp);
            break;            
          case '1':
            //add code here to trigger on 1
            triggerPin(outputAddress[1], client, outp);
            break;             
          case '2':
            //add code here to trigger on 2
            triggerPin(outputAddress[2], client, outp);
            break;
          case '3':
            //add code here to trigger on 3
            triggerPin(outputAddress[3], client, outp);
            break;
          case '4':
            //add code here to trigger on 4
            triggerPin(outputAddress[4], client, outp);
            break;
          case '5':
            //add code here to trigger on 5
            triggerPin(outputAddress[5], client, outp);
            //  printHtml(client);
            break;
          case '6':
            //add code here to trigger on 6
            triggerPin(outputAddress[6], client, outp);
            break;
          case '7':
            //add code here to trigger on 7
            triggerPin(outputAddress[7], client, outp);
            break;
          case '8':
            //add code here to trigger on 8
            triggerPin(outputAddress[8], client, outp);
            break;
          case '9':
            // add code here to trigger on 9
            triggerPin(outputAddress[9], client, outp);
            break;
          case 'A':
            triggerPin(outputAddress[10], client, outp);
            break;
          case 'B':
            triggerPin(outputAddress[11], client, outp);
            break;
          case 'C':
            triggerPin(outputAddress[12], client, outp);
            break;
          case 'D':
            triggerPin(outputAddress[13], client, outp);
            break;
          case 'E':
            triggerPin(outputAddress[14], client, outp);
            break;
          case 'F':
            triggerPin(outputAddress[15], client, outp);
            break;
          case 'Logout':
            login=false;
            readString="";
            break;
          }
        }

        // readString.concat(c);
        //if user input was blank
        if (c == '\n' && currentLineIsBlank){ 

          
          // Serial.print(readString);
      
         
          if(login == false)
          {

            printLoginTitle(client);
            printHtmlFooter(client);
            readString="";

          }

          printLastCommandOnce = true;
          printButtonMenuOnce = true;
          triggerPin(777, client, outp); //Call to read input and print menu. 777 is used not to update any outputs

          break;
        }

      }

    }

    printHtmlFooter(client); //Prints the html footer
    //clearing string for next read
    // readString="";
    //stopping client
    //  client.stop();
  } 
  else
  {  //if there is no client

    //And time of last page was served is more then a minute.
    if (millis() > (timeConnectedAt + 60000)){           

      if (writeToEeprom == true){ 
        writeEepromValues();  //write to EEprom the current output statuses
        Serial.println("No Clients for more then a minute - Writing statuses to Eeprom.");
        writeToEeprom = false;
      }

    }
  }


}

void draw(void) {
  // graphic commands to redraw the complete screen should be placed here
  // PROBE 1
  u8g.setFont(u8g_font_4x6);
  u8g.setPrintPos(2, 20);
  u8g.print("T1:"); 
  u8g.setPrintPos(14, 20); 
  u8g.print(tempO); 
  // PROBE 2
  u8g.setPrintPos(40, 20);
  u8g.print("T2:"); 
  u8g.setPrintPos(52, 20); 
  u8g.print(tempI); 
  //u8g.setFont(u8g_font_helvB08r);

  RTC.getTime();

  // u8g.setFont(u8g_font_courB08r);
  u8g.setFont(u8g_font_freedoomr10r);

  u8g.setPrintPos(0,13);

  u8g.print(m2_utl_u8d(RTC.hour,2));
  u8g.print(":");
  u8g.print(m2_utl_u8d(RTC.minute+4,2));
  u8g.print(":");
  u8g.print(m2_utl_u8d(RTC.second,2));

  u8g.setPrintPos(68,13);

  u8g.print(m2_utl_u8d(RTC.month,2));
  u8g.print("/");
  u8g.print(m2_utl_u8d(RTC.day,2));
  u8g.print("/");
  u8g.print(m2_utl_u8d(RTC.year-2000,2));

  u8g.drawLine(0,12,128,12);

  u8g.drawFrame(boxX, boxY, boxWidth, boxHeight);//void U8GLIB::drawFrame(u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h)

  readOutputStatuses();

  

}

////////////////////////////////////////////////////////////////////////
//triggerPin Function
////////////////////////////////////////////////////////////////////////
//
void triggerPin(int pin, EthernetClient client, int outp){
  //Switching on or off outputs, reads the outputs and prints the buttons   

  //Setting Outputs
  if (pin != 777){ 

    if(outp == 1) {
      if (outputInverted ==false){ 
        digitalWrite(pin, HIGH);
      } 
      else{
        digitalWrite(pin, LOW);
      }
    }
    if(outp == 0){
      if (outputInverted ==false){ 
        digitalWrite(pin, LOW);
      } 
      else{
        digitalWrite(pin, HIGH);
      }
    }
  }
  //Refresh the reading of outputs
  readOutputStatuses();


  //Prints the buttons
  if (printButtonMenuOnce == true){
    printHtmlButtons(client);
    printButtonMenuOnce = false;
  }

}

////////////////////////////////////////////////////////////////////////
//printHtmlButtons Function
////////////////////////////////////////////////////////////////////////
//print the html buttons to switch on/off channels
void printHtmlButtons(EthernetClient client){

  //Start to create the html table
  client.println("");
  //client.println("<p>");
  client.println("<FORM>");
  client.println("<table border=\"0\" align=\"center\">");


  //Printing the Temperature
  client.print("<tr>\n");        

  client.print("<td><h4>");
  client.print("Temperatures");
  client.print("</h4></td>\n");
  client.print("<td></td>");             
  client.print("<td>");
  client.print("<h3>");
  client.print(tempI);
  client.print(" &deg;F");
  client.print("<br>");
  client.print(tempO);
  client.print(" &deg;F</h3></td>\n");


  client.print("<td></td>");
  client.print("</tr>");


  //Start printing button by button
  for (int var = 0; var < outputQuantity; var++)  {      

    //set command for all on/off
    allOn += "H";
    allOn += outputAddress[var];
    allOff += "L";
    allOff += outputAddress[var];



    //Print begining of row
    client.print("<tr>\n");        

    //Prints the button Text
    client.print("<td><h4>");
    client.print(buttonText[var]);
    client.print("</h4></td>\n");

    //Prints the ON Buttons
    client.print("<td>");
    //client.print(buttonText[var]);
    client.print("<INPUT TYPE=\"button\" VALUE=\"ON ");
    //client.print(buttonText[var]);
    client.print("\" onClick=\"parent.location='/?H");
    //Serial.print(var, HEX);
    client.print(var, HEX);
    client.print("'\"></td>\n");

    //Prints the OFF Buttons 
    client.print(" <td><INPUT TYPE=\"button\" VALUE=\"OFF");
    //client.print(var);
    client.print("\" onClick=\"parent.location='/?L");
    // Serial.print(var, HEX);
    client.print(var, HEX);
    client.print("'\"></td>\n");


    //Print first part of the Circles or the LEDs

    //Invert the LED display if output is inverted.

    if (outputStatus[var] == true ){                                                            //If Output is ON
      if (outputInverted == false){                                                             //and if output is not inverted 
        client.print(" <td><div class='green-circle'><div class='glare'></div></div></td>\n"); //Print html for ON LED
      }
      else{                                                                                    //else output is inverted then
        client.print(" <td><div class='black-circle'><div class='glare'></div></div></td>\n"); //Print html for OFF LED
      }
    }
    else                                                                                      //If Output is Off
    {
      if (outputInverted == false){                                                           //and if output is not inverted
        client.print(" <td><div class='black-circle'><div class='glare'></div></div></td>\n"); //Print html for OFF LED
      }
      else{                                                                                   //else output is inverted then 
        client.print(" <td><div class='green-circle'><div class='glare'></div></div></td>\n"); //Print html for ON LED                    
      }
    }  
    //Print end of row
    client.print("</tr>\n");  
  }

  //Display or hide the Print all on Pins Button
  if (switchOnAllPinsButton == true ){

    //Prints the ON All Pins Button
    client.print("<tr>\n<td><INPUT TYPE=\"button\" VALUE=\"Switch ON All Pins");
    client.print("\" onClick=\"parent.location='/?");
    client.print(allOn);
    client.print("'\"></td>\n");

    //Prints the OFF All Pins Button            
    client.print("<td><INPUT TYPE=\"button\" VALUE=\"Switch OFF All Pins");
    client.print("\" onClick=\"parent.location='/?");
    client.print(allOff);
    client.print("'\"></td>\n<td></td>\n<td></td>\n</tr>\n");
  }
  //LOGOUT
  //client.print("<h2><a href='/?Logout'>Logout</a>");
  client.print("<h3 align=\"center\"><input type=button onClick=\"location.href='/?Logout'\" value='Logout'></h3>");
  //Closing the table and form
  client.println("</table>");
  client.println("</FORM>");
  //client.println("</p>"); 

}

////////////////////////////////////////////////////////////////////////
//readOutputStatuses Function
////////////////////////////////////////////////////////////////////////
//Reading the Output Statuses
void readOutputStatuses(){
  for (int var = 0; var < outputQuantity; var++)  { 
    outputStatus[var] = digitalRead(outputAddress[var]);
    outputStatus2[var] = digitalRead(outputAddress2[var]);
    //Serial.print(outputStatus[var]);
  }
  // for (int var = 0; var < outputQuantity; var++)  { 
  //  outputStatus2[var] = digitalRead(outputAddress2[var]);
  //  Serial.print(outputStatus2[var]);

  // }

}

////////////////////////////////////////////////////////////////////////
//readEepromValues Function
////////////////////////////////////////////////////////////////////////
//Read EEprom values and save to outputStatus
void readEepromValues(){
  for (int adr = 0; adr < outputQuantity; adr++)  { 
    outputStatus[adr] = EEPROM.read(adr); 
  }
}

////////////////////////////////////////////////////////////////////////
//writeEepromValues Function
////////////////////////////////////////////////////////////////////////
//Write EEprom values
void writeEepromValues(){
  for (int adr = 0; adr < outputQuantity; adr++)  { 
    EEPROM.write(adr, outputStatus[adr]);
  }

} 

////////////////////////////////////////////////////////////////////////
//initEepromValues Function
////////////////////////////////////////////////////////////////////////
//Initialiaze EEprom values
//if eeprom values are not the correct format ie not euqual to 0 or 1 (thus greater then 1) initialize by putting 0
void initEepromValues(){
  for (int adr = 0; adr < outputQuantity; adr++){        
    if (EEPROM.read(adr) > 1){
      EEPROM.write(adr, 0);
    } 

  }

}

void printTemperature(DeviceAddress deviceAddress)
{
  tempI =sensors.getTempF(deviceAddress);
}
void printTemperature2(DeviceAddress deviceAddress)
{
  tempO = sensors.getTempF(deviceAddress);
}
////////////////////////////////////////////////////////////////////////
//htmlHeader Function
////////////////////////////////////////////////////////////////////////
//Prints html header
void printHtmlHeader(EthernetClient client){
  // Serial.print("Serving html Headers at ms -");
  timeConnectedAt = millis(); //Record the time when last page was served.
  // Serial.print(timeConnectedAt); // Print time for debbugging purposes
  writeToEeprom = true; // page loaded so set to action the write to eeprom

  // send a standard http response header
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connnection: close");
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<head>");

  // add page title 
  client.println("<title>Ethernet Switching</title>");
  client.println("<meta name=\"description\" content=\"Ethernet Switching\"/>");

  // add a meta refresh tag, so the browser pulls again every x seconds:
  client.print("<meta http-equiv=\"refresh\" content=\"");
  client.print(refreshPage);
  client.println("; url=/\">");

  // add other browser configuration
  client.println("<meta name=\"apple-mobile-web-app-capable\" content=\"yes\">");
  client.println("<meta name=\"apple-mobile-web-app-status-bar-style\" content=\"default\">");
  client.println("<meta name=\"viewport\" content=\"width=device-width, user-scalable=no\">");          

  //inserting the styles data, usually found in CSS files.
  client.println("<style type=\"text/css\">");
  client.println("");

  //This will set how the page will look graphically
  client.println("html { height:100%; }");  

  client.println("  body {");
  client.println("    height: 100%;");
  client.println("    margin: 0;");
  client.println("    font-family: helvetica, sans-serif;");
  client.println("    -webkit-text-size-adjust: none;");
  client.println("   }");
  client.println("");
  client.println("body {");
  client.println("    -webkit-background-size: 100% 21px;");
  client.println("    background-color: #c5ccd3;");
  client.println("    background-image:");
  client.println("    -webkit-gradient(linear, left top, right top,");
  client.println("    color-stop(.75, transparent),");
  client.println("    color-stop(.75, rgba(255,255,255,.1)) );");
  client.println("    -webkit-background-size: 7px;");
  client.println("   }");
  client.println("");
  client.println(".view {");
  client.println("    min-height: 100%;");
  client.println("    overflow: auto;");
  client.println("   }");
  client.println("");
  client.println(".header-wrapper {");
  client.println("    height: 44px;");
  client.println("    font-weight: bold;");
  client.println("    text-shadow: rgba(0,0,0,0.7) 0 -1px 0;");
  client.println("    border-top: solid 1px rgba(255,255,255,0.6);");
  client.println("    border-bottom: solid 1px rgba(0,0,0,0.6);");
  client.println("    color: #fff;");
  client.println("    background-color: #8195af;");
  client.println("    background-image:");
  client.println("    -webkit-gradient(linear, left top, left bottom,");
  client.println("    from(rgba(255,255,255,.4)),");
  client.println("    to(rgba(255,255,255,.05)) ),");
  client.println("    -webkit-gradient(linear, left top, left bottom,");
  client.println("    from(transparent),");
  client.println("    to(rgba(0,0,64,.1)) );");
  client.println("    background-repeat: no-repeat;");
  client.println("    background-position: top left, bottom left;");
  client.println("    -webkit-background-size: 100% 21px, 100% 22px;");
  client.println("    -webkit-box-sizing: border-box;");
  client.println("   }");
  client.println("");
  client.println(".header-wrapper h1 {");
  client.println("    text-align: center;");
  client.println("    font-size: 20px;");
  client.println("    line-height: 44px;");
  client.println("    margin: 0;");
  client.println("   }");
  client.println("");
  client.println(".group-wrapper {");
  client.println("    margin: 9px;");
  client.println("    }");
  client.println("");
  client.println(".group-wrapper h2 {");
  client.println("    color: #4c566c;");
  client.println("    font-size: 17px;");
  client.println("    line-height: 0.8;");
  client.println("    font-weight: bold;");
  client.println("    text-shadow: #fff 0 1px 0;");
  client.println("    margin: 20px 10px 12px;");
  client.println("   }");
  client.println("");
  client.println(".group-wrapper h3 {");
  client.println("    color: #4c566c;");
  client.println("    font-size: 12px;");
  client.println("    line-height: 1;");
  client.println("    font-weight: bold;");
  client.println("    text-shadow: #fff 0 1px 0;");
  client.println("    margin: 20px 10px 12px;");
  client.println("   }");
  client.println("");
  client.println(".group-wrapper h4 {");  //Text for description
  client.println("    color: #212121;");
  client.println("    font-size: 14px;");
  client.println("    line-height: 1;");
  client.println("    font-weight: bold;");
  client.println("    text-shadow: #aaa 1px 1px 3px;");
  client.println("    margin: 5px 5px 5px;");
  client.println("   }");
  client.println(""); 
  client.println(".group-wrapper table {");
  client.println("    background-color: #fff;");
  client.println("    -webkit-border-radius: 10px;");

  client.println("    -moz-border-radius: 10px;");
  client.println("    -khtml-border-radius: 10px;");
  client.println("    border-radius: 10px;");


  client.println("    font-size: 17px;");
  client.println("    line-height: 20px;");
  client.println("    margin: 9px 0 20px;");
  client.println("    border: solid 1px #a9abae;");
  client.println("    padding: 11px 3px 12px 3px;");
  client.println("    margin-left:auto;");
  client.println("    margin-right:auto;");

  client.println("    -moz-transform :scale(1);"); //Code for Mozilla Firefox
  client.println("    -moz-transform-origin: 0 0;");



  client.println("   }");
  client.println("");


  //how the green (ON) LED will look
  client.println(".green-circle {");
  client.println("    display: block;");
  client.println("    height: 23px;");
  client.println("    width: 23px;");
  client.println("    background-color: #0f0;");
  //client.println("    background-color: rgba(60, 132, 198, 0.8);");
  client.println("    -moz-border-radius: 11px;");
  client.println("    -webkit-border-radius: 11px;");
  client.println("    -khtml-border-radius: 11px;");
  client.println("    border-radius: 11px;");
  client.println("    margin-left: 1px;");

  client.println("    background-image: -webkit-gradient(linear, 0% 0%, 0% 90%, from(rgba(46, 184, 0, 0.8)), to(rgba(148, 255, 112, .9)));@");
  client.println("    border: 2px solid #ccc;");
  client.println("    -webkit-box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px;");
  client.println("    -moz-box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px; /* FF 3.5+ */");
  client.println("    box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px; /* FF 3.5+ */");

  client.println("    }");
  client.println("");

  //how the black (off)LED will look
  client.println(".black-circle {");
  client.println("    display: block;");
  client.println("    height: 23px;");
  client.println("    width: 23px;");
  client.println("    background-color: #040;");
  client.println("    -moz-border-radius: 11px;");
  client.println("    -webkit-border-radius: 11px;");
  client.println("    -khtml-border-radius: 11px;");
  client.println("    border-radius: 11px;");
  client.println("    margin-left: 1px;");
  client.println("    -webkit-box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px;");
  client.println("    -moz-box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px; /* FF 3.5+ */"); 
  client.println("    box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px; /* FF 3.5+ */");
  client.println("    }");
  client.println("");

  //this will add the glare to both of the LEDs
  client.println("   .glare {");
  client.println("      position: relative;");
  client.println("      top: 1;");
  client.println("      left: 5px;");
  client.println("      -webkit-border-radius: 10px;");
  client.println("      -moz-border-radius: 10px;");
  client.println("      -khtml-border-radius: 10px;");
  client.println("      border-radius: 10px;");
  client.println("      height: 1px;");
  client.println("      width: 13px;");
  client.println("      padding: 5px 0;");
  client.println("      background-color: rgba(200, 200, 200, 0.25);");
  client.println("      background-image: -webkit-gradient(linear, 0% 0%, 0% 95%, from(rgba(255, 255, 255, 0.7)), to(rgba(255, 255, 255, 0)));");
  client.println("    }");
  client.println("");


  //and finally this is the end of the style data and header
  client.println("</style>");
  client.println("</head>");

  //now printing the page itself
  client.println("<body>");
  client.println("<div class=\"view\">");
  client.println("    <div class=\"header-wrapper\">");
  client.println("      <h1>Ethernet Switching</h1>");
  client.println("    </div>");

  //////

} //end of htmlHeader

////////////////////////////////////////////////////////////////////////
//htmlFooter Function
////////////////////////////////////////////////////////////////////////
//Prints html footer
void printHtmlFooter(EthernetClient client){
  //Set Variables Before Exiting 
  printLastCommandOnce = false;
  printButtonMenuOnce = false;
  allOn = "";
  allOff = "";

  //printing last part of the html
  client.println("\n<h3 align=\"center\">A. Palmer <br> Authentication V1.0");
  client.println("</h3></div>\n</div>\n</body>\n</html>");

  delay(1); // give the web browser time to receive the data

  client.stop(); // close the connection:

  //Serial.println(" - Done, Closing Connection.");

  delay (2); //delay so that it will give time for client buffer to clear and does not repeat multiple pages.

} //end of htmlFooter


////////////////////////////////////////////////////////////////////////
//printHtmlButtonTitle Function
////////////////////////////////////////////////////////////////////////
//Prints html button title
void printHtmlButtonTitle(EthernetClient client){
  client.println("<div  class=\"group-wrapper\">");
  client.println("<h2 align=\"center\">Switch the required output.</h2>");
  client.println();
}


////////////////////////////////////////////////////////////////////////
//printLoginTitle Function
////////////////////////////////////////////////////////////////////////
//Prints html button title
void printLoginTitle(EthernetClient client){
  client.println("<div  class=\"group-wrapper\">");
  client.println("<h2 align=\"center\">Welcome...</h2>");
  client.println("<h2 align=\"center\">Enter Username and Password.</h2>");
  client.println(" </div>");
  client.print("<form action=''>"); //change to your IP 192.168.2.177/
  client.print("");
  client.println("    <div class=\"group-wrapper\">");
  client.print("<h2 align=\"center\">Username:</h2>");
  client.print("<h2 align=\"center\"><input name='User' value=''></h2>");
  client.print("<h2 align=\"center\">Password:</h2>");
  client.print("<h2 align=\"center\"><input type='Password' name='Pass' value=''></h2>");
  client.print("</div>");
  client.print("<h2 align=\"center\"><input type='submit' value=' Login '></h2>");
  //client.print("<input type='submit' value=' Login '>");
  client.print("<hr /></form><hr />");
  client.println("</head></center>");
}


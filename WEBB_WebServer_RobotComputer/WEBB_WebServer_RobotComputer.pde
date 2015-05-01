/*
 * Arduino Controlled Web Connected Robot (WEBB) - Webserver
 * For more details visit: http://www.oomlout.com/serb 
 * 
 * Behaviour: The program listens for http (webpage) requests in the form
 *            "/request?command=F&param1=100" and then passes them to the
 *            Arduino              
 *            Supported Commands - 'F' - 70 - Forward
 *                                 'B' - 66 - Backward
 *                                 'L' - 76 - Left
 *                                 'R' - 82 - Right
 *                                 'S' - 83 - Speed
 *                                 'X' - 88 - SetSpeedLeft
 *                                 'Y' - 89 - SetSpeedRight 
 *                                 'C' - 67 - Stop 
 *            param1's - 0 - 255 (0 to 25.5 Seconds) value * 100 milliseconds 
 *
 *            also serves the control webpage to the browser if asking for ControlPage.html
 *
 * Operation: 1. Click on the Comm port your Arduino is connected to
 *            2. Use the test buttons to see if your arduino is listening 
 *               (will move in the direction clicked for half a second)
 *            3. Click Start and your webserver will be listening for requests
 *
 *
 * License: This work is licenced under the Creative Commons 
 *          Attribution-Share Alike 3.0 Unported License. To 
 *          view a copy of this licence, visit 
 *          http://creativecommons.org/licenses/by-sa/3.0/ 
 *          or send a letter to Creative Commons, 171 Second 
 *          Street, Suite 300, San Francisco, California 94105, 
 *          USA.
 *         
*/

import processing.serial.*;         //import the serial library to use Serial (for talking to Arduino)
import processing.net.*;            //import the net library to use Server    (for talking to the internet)

/* Serial Port Related Variables */
int serialPortIndex = -1;           //the index of the serial port we are using in the Serial.list() list
String[] commOptions;               //A variable to store Serial.list() in so there is no need to poll 
                                    //the serial ports each time we wish to refrence it (polling causes a 
                                    //jump in the arduino servos)
Serial serialPort;                  //Makes the serial port addressable throughout this sketch

/* Graphic and Rendering Related Variables */
PFont font;                        //makes font public so it only needs to be loaded once (Arial-72)

/* Network Related Variables */
Server wServer;                    //The Server which handles web requests
int port = 12345;                  //The port the Server listens to
int cycleCount = 0;                //Counts the number of times the draw loop executes (used to 
                                   //animate the "running" text)           

/* Utility Variables */ 
PImage bg;     //The background image currently \WEBB-background.png

/*
 * Button Related Variables
 * (buttons are implemented very roughly, bounding boxes are stored and graphic and text are
 *  drawn on the background image) ie. moving button variables will not move appearance of buttons
 */
Button comm1; Button comm2; Button comm3; Button comm4; //The buttons to choose the correct serial port
Button up; Button right; Button left; Button down;//The direction buttons to test if the robot is responding
Button webServer;                                       //The button to start the webServer

/*
 * sets evrything up
 */
void setup() {
  frameRate(5);                          //Slows the sketch down a little so it is not as taxing on the system
  bg = loadImage("WEBB-background.png"); //Loads the background image (stored in the sketch folder)
  size(700, 400);                        //Sets the size of the sketch window
  font = loadFont("ArialMT-72.vlw");     //Loads the font we will use throught this sketch
  commOptions = Serial.list();           //Polls the Serial ports and gets a list of available ports 
                                         //(doing this only once keeps the robot from twitching everytime it's 
                                         //serial port is polled)
  defineButtons();            //Defines the bounding boxes for each button
  background(bg);             //Paints the background image (this image has all the button graphics on it)
}

/* The sketches main loop */
void draw() {
  if(cycleCount==0){
  //-- call draw routines
    drawCommBox();                        //Draws the emelments in the Comm Box (box 1)
    drawTestBox();                        //Draws all the elements in the Test Box (box 2) 
    drawWebServerBox();                   //Draws all the elements in the WebServer Box (box 3)
  }
  //-- call working routines
  pollWebServer();              //Polls the web server to see if any requests have come in via the network
}

//----------------------------------------------------------------------------
//Start of draw routines (split up just to make code nicer to read)

/*
 * The draw update routine (executed every cycle) for items in the comm box (box 1)
 */
void drawCommBox(){
  fill(0,0,0);                                 //Set the fill to black
  textFont(font, 15);                          //Set the font to the right size
  for(int i = 0; i < commOptions.length; i++){ //For each comm option
    text(commOptions[i], 100, 102 + i*70);     //Writes the name of each comm option beside 
                                               //the appropriate button
  }
  /* Draw the dot beside the currently active comm port */
  if(serialPortIndex != -1){                    //checks to see if a comm port has been chosen
    fill(0,147,221);                            //Set fill to cyan
    ellipse(180, 96 + 70 * serialPortIndex, 30, 30);     //Draw a circle beside the currently active Comm port
  } 
}

/* The draw update routine (executed every cycle) for items in the test box (box 2)*/
void drawTestBox(){
}

/* The draw update routine (executed every cycle) for items in the webserver box (box 3) */
void drawWebServerBox(){
  fill(0,0,0);                                  //Set the fill to black
  textFont(font, 30);                           //Set the font to the right size
  if(wServer == null){text("Start", 553, 210);  //If the Webserver has not started writes Start on the button
  }else{                                                 
    String running = "Running.";                        //If the Webserver is running writes running and 
                                                        //a progression of dots for movement
    if((cycleCount / 1) > 0){running = running + ".";} //the number of dots is based on cycleCount     
    if((cycleCount / 1) > 1){running = running + ".";}
    if((cycleCount / 1) > 2){running = running + ".";}
    if((cycleCount / 1) > 3){cycleCount=0;}
    text(running, 520, 210);
  }
  cycleCount++;                                     //Every time through cycleCount is incremented by one
}
  
//End of Draw routines
//-------------------------------------------------------------

//-------------------------------------------------------------
//Start of Service routines

/*
 * mousePressed is called everytime the mouse is pressed this 
 * checks if the mouse is inside any of the button bounding boxes
 * and if it is the appropriate routine is called
*/
void mousePressed(){
    if(comm1.pressed())      {updateSerialPort(0);}       //Changes to the comm port at index 0 in Serial.list()
    else if(comm2.pressed()) {updateSerialPort(1);}       //Changes to the comm port at index 1 in Serial.list()
    else if(comm3.pressed()) {updateSerialPort(2);}       //Changes to the comm port at index 2 in Serial.list()
    else if(comm4.pressed()) {updateSerialPort(3);}       //Changes to the comm port at index 3 in Serial.list()
    else if(up.pressed())    {sendRobotCommand("F", 5);} //Sends a command to move the robot Forward for 0.5 seconds
    else if(left.pressed())  {sendRobotCommand("L", 5);} //Sends a command to move the robot Left for 0.5 seconds
    else if(right.pressed()) {sendRobotCommand("R", 5);} //Sends a command to move the robot Right for 0.5 seconds
    else if(down.pressed())  {sendRobotCommand("B", 5);} //Sends a command to move the robot Backward for 0.5 seconds
    else if(webServer.pressed()) {startWebServer();}      //Starts the webServer       
    cycleCount = 0;
    background(bg);                       //Repaints the background image (this image has all the button graphics on it)
    draw();
}

/*
 * Called once an execution it checks the Server to see if there are any waiting connections
 * if there is a waiting connection it pulls out the request and passes it to the parseRequest(String)
 * routine to strip away extra text. This is then sent to the interpretRequest(String) routine to 
 * call the appropriate action routine (ie send instructions to robot). 
 *
 * A Request is passed by entering the computers address and port into a web browser
 * For local machine "http://127.0.0.1:12345/request?command=F&param1=100"
 *
 * Example Request:
 *   GET /request?command=F&param1=100 HTTP/1.1
 *   User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US) AppleWebKit/525.19 (KHTML, like Gecko)
 *   Accept: *//*
 *   Accept-Language: en-US,en
 *   Accept-Charset: ISO-8859-1,*,utf-8
 *   Accept-Encoding: gzip,deflate,bzip2
 *   Host: 127.0.0.1:12345
 *   Connection: Keep-Alive
 *
 * Example Reply (for processed request)
 *   HTTP/1.1 200 OK
 *   Connection: close
 *   Content-Type: text/html
 *
 *   command: F param1: 100   
 */
void pollWebServer(){
  if(wServer != null){                               //If the webServer is running
    Client request = wServer.available();            //Load the next Client in line in the 
                                                     //server (null if no requests waiting)
    if(request != null){                             //if there is a request process it
      String fullRequest = request.readString();     //Store request contents as a string
      String ip = request.ip();                      //Store the clients ip address as a string 
      String requestString = parseRequest(fullRequest.toUpperCase());  //Strip away all the extra 
                                         //information leaving only the request string (text after GET)
                                         //(shifts to upper case to make string testing easier)
       if(requestString.indexOf("REQUEST?") != -1){        //Checks if the request has "REQUEST?"
         String reply = interpretRequest(requestString);   //Sends the request to the request interpreter
         request.write("HTTP/1.1 200 OK\nConnection: close\nContent-Type: text/html\n\n" + reply);   
                                                          //Sends the reply a 200 to say the request was 
                                                          //handled and the "reply" string as the response 
         wServer.disconnect(request);                     //Disconnects the Client
       }else if(requestString.indexOf("CONTROLPAGE") != -1){  //If the request is for the control web page
         String[] webPageArray = loadStrings("WEBB-ControlPage.html");  
                                                          //Loads the control webPage from \data\WEBB-ControlPage.html 
                                                          //(loaded as an array of lines)
         String webPage ="";                              //To make editing easier the webpage has been left with 
                                                          //line breaks so this for loop striops those lines out
         for(int i = 0; i < webPageArray.length; i++){
           webPage = webPage + webPageArray[i];
         }
         request.write("HTTP/1.1 200 OK\nConnection: close\nContent-Type: text/html\n\n" + webPage); 
                                                          //sends the browser the instructions that the request was not 
                                                          //processed and the webpage as a string
         wServer.disconnect(request);                     //Disconnects the client
       }else if(requestString.indexOf("/TEST/") != -1){   //For Testing outputs the request as a println
         println(fullRequest);
         println(ip);
         request.write("HTTP/1.1 200 OK\nConnection: close\nContent-Type: text/html\n\n"); 
                                                          //sends the browser the instructions that the request was not 
                                                          //processed and the webpage as a string
         wServer.disconnect(request);                     //Disconnects the client
       }else{
         request.write("HTTP/1.1 404 FILE NOT FOUND\nConnection: close\n"); 
                                                          //sends the browser the instructions that the request was 
                                                          //not processed (usually due to a typo) then tells it to 
                                                          //close the connection
         wServer.disconnect(request);                     //Disconnects the client
       }
    }
  } 
}

//End of Service Routines
//-------------------------------------------------------------

//-------------------------------------------------------------
//Start of setup routines (routines called once or not very often

/*
 * Defines all the bounding boxes for the buttons used in this sketch
 */
void defineButtons(){
  comm1 = new Button(30, 65, 60, 60);  //Defines the comm1 button
  comm2 = new Button(30, 135, 60, 60); //Defines the comm2 button
  comm3 = new Button(30, 205, 60, 60); //Defines the comm3 button
  comm4 = new Button(30, 275, 60, 60); //Defines the comm1 button
  up = new Button(320, 110, 60, 60);   //Defines the up/forward button
  left = new Button(260, 170, 60, 60); //Defines the left button
  right = new Button(380, 170, 60, 60);//Defines the right button
  down = new Button(320, 230, 60, 60); //Defines the down/backward button
  webServer = new Button(493, 170, 180, 60); //Defines the webserver button
}

/*
 * When the "Start" button in the webserver box is pressed the server starts listening
 * for incoming connections on port port (12345) defined in the preamble
 */
void startWebServer(){
    if(wServer == null) {wServer = new Server(this, port);}  
}

/*
 * This takes a verbose http request from a client and
 * returns only the request line (what comes after GET)
 */
String parseRequest(String fullRequest){
  fullRequest = fullRequest.substring(fullRequest.indexOf("GET ") + "GET ".length());   
                                            //Cuts off everything in the string before GET
  fullRequest = fullRequest.substring(0, fullRequest.indexOf(" "));                     
                                            //Takes from the begining of the new string to the first space
  return fullRequest;                       //returns the shortened request
}

/*
 * updates the serial port being used to comunicate with the arduino
 * portIndex is the index of the port we wish to use in Serial.list();
 * as Serial.list() is loaded into memory at startup adding serial ports 
 * after startup could cause unexcpected operation
 */
void updateSerialPort(int portIndex){
  if(commOptions.length > portIndex){ //if the passed portIndex is a valid index in Serial.list() 
                                      //(ie. not option three in a list of two)
     serialPortIndex = portIndex;     //set the serialPortIndex variable to the new serialport index 
                                      //(this is used to display the dot beside the active port)
  }
  if(serialPortIndex != -1){                    //check to make sure a port has been chosen sometime previous
    if(serialPort != null){serialPort.stop();}  //if a Serial port is already being used stop it before loading a new one 
    serialPort = new Serial(this, Serial.list()[serialPortIndex], 9600);  
                                                //Create a new Serial object to comunicate with the Arduino 9600 Baud
  }
}

/*
 * Takes a request string (ex: "request?command=F&param1=100") and will strip out the command
 * and param1 and pass it to the arduino (adding extra parameters or non arduino commands would be done here)
 */
String interpretRequest(String requestString){
 String returnValue = "OK";                     //Prepares a returnValue variable
 String command;                                //Next three lines extract the value after "command=" 
                                                //(the extra line is in case the command parameter is at the 
                                                //end of the request and not followed by an &
 if(requestString.indexOf("COMMAND=") != -1){
   command = requestString.substring(requestString.indexOf("COMMAND=") + "COMMAND=".length());} else{command = "Z";}
 if(command.indexOf("&") != -1){
   command = command.substring(0,command.indexOf("&"));}else{command = command;}

 String param1String;                         //Next three lines extract the value after "param1=" 
                                              //(the extra line is in case the command parameter is at 
                                              //the end of the request and not followed by an &
 if(requestString.indexOf("PARAM1=") != -1){
   param1String = requestString.substring(requestString.indexOf("PARAM1=") + "PARAM1=".length());} else{param1String = "0";}
 if(param1String.indexOf("& ") != -1){
   param1String = param1String.substring(0,param1String.indexOf("&"));} else{param1String = param1String;}
 int param1 = Integer.parseInt(param1String);  //Turns the param1 String into an integer
 sendRobotCommand(command,param1);            //Sends the command to the routine that sends it to the Arduino
 returnValue = "command: " + command + " param1: " + param1;  //at the moment just the parsed command but sending 
                                                              //as html so formatting can be included
 return returnValue;
}

/*
 * Takes a command (currently one letter) and a parameter (one byte 0-255) and sends it to the arduino
 * which is listening and reasts
 * Currrently Supported Commands
 * F -
*/
void sendRobotCommand(String command, int param1){
  print("command: " + command + " time: " + param1);
  String checkString = "AAA";
  if(serialPort != null){
    serialPort.write(checkString + command);
    serialPort.write(byte(param1));
  }
  print(">>>" + checkString + command + param1);
  println();
}

//End of setup/not very commonly called routines
//-------------------------------------------------------------------------------------

//Example button and RectButton code from processing.org example with some changes 
//made mostly to simplify and remove roll over animation
//http://processing.org/learning/topics/buttons.html (2008-09-23)
class Button{
  int x, y;  int width, height;
  Button(int ix, int iy, int iwidth, int iheight) {
    x = ix;  y = iy;  width = iwidth;  height = iheight;
  }
  
  boolean pressed() {
    if(overRect()) { return true; } 
    else {           return false;}    
  }

  boolean overRect() {
    if (mouseX >= x && mouseX <= x+width && mouseY >= y && mouseY <= y+height) { return true; } 
    else { return false; }
  }
}

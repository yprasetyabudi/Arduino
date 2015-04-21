#include <SPI.h>
#include <Ethernet.h> // Initialize the libraries.
#include <Servo.h> 
Servo myservo;
 
byte mac[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; //I left the MAC address and IP address blank.
byte ip[] = { 192,168,2,177 };                     // You will want to fill these in with your MAC and IP address.
byte gateway[] = { 192, 168, 2, 1 }; //Manual setup only
byte subnet[] = { 255, 255, 255, 0 }; //Manual setup only
  
EthernetServer server(80); // Assigning the port forwarded number. It's almost always 80.
 
String readString; // We will be using strings to keep track of things.
int val;    // Using val as a variable for the PIR status.
int pir=3;
 
//////////////////////
 
void setup(){
 
  pinMode(2, OUTPUT);
  pinMode(pir, INPUT);
  Ethernet.begin(mac, ip);
  myservo.attach(9);
  myservo.write(45);
}
 
void loop(){
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {                     // This is all where we start up the server and strings.
        char c = client.read();                     
        if (readString.length() < 100) {
          readString += c;
        } 
        if (c == '\n') {
          Serial.println(readString);                      // And here we begin including the HTML
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<hmtl>");
client.println("<head>");
client.println("ARDUINO PAGE");
client.println("</head>");
client.println("<title>");
client.println("ARDUINO + ETHERNET Page");
client.println("</title>");
client.println("<body bgcolor=black>");
client.println("<font color=white>");
client.println("<meta http-equiv=\"refresh\" content=\"4\">"); // This is used to refresh the page so
client.println("<center>");                                      // we can see if there is Motion or not.
client.println("<b>");
client.println("Greetings! Here you will find a somewhat interactive page served up by my Arduino!");
client.println("</br>");
client.println("As well, you can interact with my Arduino! It's pretty basic, and you really can't see it,");
client.println("</br>");
client.println("but as you press a button, you turn on or off an LED or move a Servo! Have fun!");
client.println("</b>");
client.println("<p>");
client.println("<table border=0 width=200>");
client.println("<tr>");
client.println("<td align=center>");
client.println("<font color=white>");
client.println("The Temperature is:");
client.println("</td>");
client.println("</tr>");
client.println("<tr>");
client.println("<td align=center>");
client.println("<font color = turquoise size=10>");
int temp = (((5*analogRead(5)*100/1024)*1.8)+32);     // This replaces the 00 with a temperature in F.
client.println(temp);
client.println("* F");
client.println("</td>");
client.println("</tr>");
client.println("</table>");
client.println("<p>");
client.println("<FORM>");
client.println("<INPUT type=button value=LED1-ON onClick=window.location='/?lighton1\'>");
client.println("<INPUT type=button value=LED1-OFF onClick=window.location='/?lightoff1\'>");
client.println("</FORM>");   // Above and below you'll see that when we click on a button, it adds a little snippet
client.println("<FORM>");    // to the end of the URL. The Arduino watches for this and acts accordingly.
client.println("<INPUT type=button value=Servo-0 onClick=window.location='/?serv0\'>");
client.println("<INPUT type=button value=Servo-45 onClick=window.location='/?serv45\'>");
client.println("<INPUT type=button value=Servo-90 onClick=window.location='/?serv90\'>");
client.println("</FORM>");
client.print("<table border=1 width=200>");
client.print("<tr>");
client.print("<td align=center>");
client.print("<font color=white size=3>");
client.print("There is currently");
client.print("</td>");
client.print("</tr>");
client.print("<tr>");
client.print("<td align=center>");
client.print("<font color=white size=3>");  // And below we will print Motion if there is, and No Motion if there's not.
val = digitalRead(pir);
          if (val == HIGH){
           client.print("MOTION");
          }
         else {
          client.print("NO MOTION");
         }
client.print("</td>");
client.print("</tr>");
client.print("</table>");
client.println("</center>");
client.println("</font>");
client.println("</body>");
client.println("</html>");
 
          delay(1);
  	  
          if(readString.indexOf("?lighton") >0)     // these are the snippets the Arduino is watching for.
          {
            digitalWrite(2, HIGH);
          }
          else{
          if(readString.indexOf("?lightoff") >0)
          {
            digitalWrite(2, LOW);
          }
          
          else{
            if(readString.indexOf("?serv0") >0)
            {
              myservo.write(0);
            }
          
          else{
            if(readString.indexOf("?serv45") >0)
            {
              myservo.write(45);
            }
          
          else{
            if(readString.indexOf("?serv90") >0)
            {
              myservo.write(90);
            }
          }
          }
          }
          }
          readString="";
          client.stop();            // End of session.
 
        }
      }
    }
  }
 
}

/*********
Clark Emerick
GK Design Studios
Nov 7, 2018
Rev 0.5
SAM Proof of Concept Controller
*********/

// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid     = "xxxx";
const char* password = "xxxxx";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String topmagnetState = "off";
String bottommagnetState = "off";
String topsolenoidState = "off";
String bottomsolenoidState = "off";
String samlockState = "off";

// Assign output variables to GPIO pins
const int topmagnetpin = 25;
const int bottommagnetpin = 26;
const int topsolenoidpin = 27;
const int bottomsolenoidpin = 14;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  Serial.println("Setting up I/O pins  " + String(topmagnetpin)  + ":" + String(bottommagnetpin)  + ":" + String(topsolenoidpin) + ":" + String(bottomsolenoidpin));
  pinMode(topmagnetpin, OUTPUT);
  pinMode(bottommagnetpin, OUTPUT);
  pinMode(topsolenoidpin, OUTPUT);
  pinMode(bottomsolenoidpin, OUTPUT);
  // Set outputs to LOW
  digitalWrite(topmagnetpin, HIGH);
  digitalWrite(bottommagnetpin, HIGH);
  digitalWrite(topsolenoidpin, HIGH);
  digitalWrite(bottomsolenoidpin, HIGH);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("Server IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // checking if header is valid
            // dXNlcjpwYXNz = 'user:pass' (user:pass) base64 encode
            // Finding the right credential string, then loads web page
              Serial.println("Doing userid and password.");
            if(header.indexOf("dXNlcjpwYXNz") >= 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              Serial.println("Userid and password were sent correctly.");
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();

              // turns the GPIOs on and off
              if (header.indexOf("GET /sam/lock") >= 0) {
                Serial.println("Magnets off");
                Serial.println("Solenoids off -- Pins Out");
                digitalWrite(topsolenoidpin, LOW);
                digitalWrite(bottomsolenoidpin, LOW);
                delay(200);
                digitalWrite(topmagnetpin, LOW);
                digitalWrite(bottommagnetpin, LOW);
                delay(200);
                digitalWrite(topsolenoidpin, HIGH);
                digitalWrite(bottomsolenoidpin, HIGH);
                delay(200);
                digitalWrite(topmagnetpin, HIGH);
                digitalWrite(bottommagnetpin, HIGH);

                topsolenoidState = "off";
                bottomsolenoidState = "off";

                 samlockState="on";
                topmagnetState = "off";
                bottommagnetState = "off";



              } else if (header.indexOf("GET /sam/open") >= 0) {
                Serial.println("Magnets on to release pins.");
                samlockState="off";

                digitalWrite(topmagnetpin, LOW);
                digitalWrite(bottommagnetpin, LOW);
                Serial.println("Solenoids on -- Pulling pins in.");
                delay(200);
                digitalWrite(topsolenoidpin, LOW);
                digitalWrite(bottomsolenoidpin, LOW);
                delay(1000);
                Serial.println("Now turning magnets off.");
                digitalWrite(topmagnetpin, HIGH);
                digitalWrite(bottommagnetpin, HIGH);

                topmagnetState = "off";
                bottommagnetState = "off";

                topsolenoidState = "on";
                bottomsolenoidState = "on";

              } else if (header.indexOf("GET /topmag/on") >= 0) {
                Serial.println("GET /topmag/on");
                topmagnetState = "on";
                digitalWrite(topmagnetpin, LOW);
              } else if (header.indexOf("GET /topmag/off") >= 0) {
                Serial.println("Top magnet off.");
                topmagnetState = "off";
                digitalWrite(topmagnetpin, HIGH);

            } else if (header.indexOf("GET /botmag/on") >= 0) {
              Serial.println("Bottom magnet on.");
              bottommagnetState = "on";
              digitalWrite(bottommagnetpin, LOW);

          } else if (header.indexOf("GET /botmag/off") >= 0) {
            Serial.println("Bottom magnet off.");
            bottommagnetState = "off";
            digitalWrite(bottommagnetpin, HIGH);

        } else if (header.indexOf("GET /topnoid/on") >= 0) {
          Serial.println("Top solenoid on.");
          topsolenoidState = "on";
          digitalWrite(topsolenoidpin, LOW);
        } else if (header.indexOf("GET /topnoid/off") >= 0) {
          Serial.println("Top solenoid off.");
          topsolenoidState = "off";
          digitalWrite(topsolenoidpin, HIGH);

      } else if (header.indexOf("GET /botnoid/on") >= 0) {
        Serial.println("Bottom solenoid on.");
        bottomsolenoidState = "on";
        digitalWrite(bottomsolenoidpin, LOW);

    } else if (header.indexOf("GET /botnoid/off") >= 0) {
      Serial.println("Bottom solenoid off.");
      bottomsolenoidState = "off";
      digitalWrite(bottomsolenoidpin, HIGH);
    }

  Serial.println("Starting web page display");
              // Display the HTML web page
              client.println("<!DOCTYPE html><html><head>  <title>Smart Mirror Controller</title>  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">  <link rel=\"icon\" href=\"data:,\">");
              client.println("<style>  html {  font-family: Helvetica;  display:inline-flex;    margin: 0px auto;  text-align: left;}");
              client.println(".button {  background-color: #4CAF50;  border: none;  border-radius: 22px;  color: white;  padding: 16px 40px;  text-decoration: none;  font-size: 30px;  margin: 2px;  cursor: pointer;}");
              client.println(".button2 {  background-color: #555555}");
              // Display current state
              client.println("table, th, td {    border: 1px solid black;    th {    text-align: center ;}h2 {    -webkit-column-span: all;  column-span: all;}");
              client.println("</style></head><body>  <h1>Smart Mirror Controls</h1>  <h2>Mechanical Proof of Concepts</h2>  <h3>Nov 7, 2018 -- Rev 0.5</h3>  <table style=\"width:100%\"> <tr><th colspan=\"2\">");
              client.println("<strong>Automatic Controls</strong><th> </tr> <tr> <th>Item</th> <th>State</th> </tr> <tr> <td align=\"center\">SAM</td> <td align=\"center\"> ");

              // Do sam auto lock or unlock
              if (samlockState=="off") {
                client.println("<a href=\"/sam/lock\"><button class=\"button button2\">Open</button></a></td>");
              } else {
                client.println("<a href=\"/sam/open\"><button class=\"button \">Lock</button></a></td>");
              }

              // Manual control top and bottom magnets and solenoids
              client.println("</tr></table><br> <table> <tr> <th colspan=\"2\"><strong>Manual Controls</strong><th> </tr> <tr> <td align=\"center\">Top Magnet</td>");

              // If the output27State is off, it displays the ON button
              if (topmagnetState=="off") {
                client.println("<td> <a href=\"/topmag/on\"><button class=\"button button2\">Off</button></a></td>");
              } else {
                client.println("<td> <a href=\"/topmag/off\"><button class=\"button\">On</button></a></td>");
              }

              client.println("<tr> <td align=\"center\">Bottom Magnet</td>");

              if (bottommagnetState=="off") {
                client.println("<td> <a href=\"/botmag/on\"><button class=\"button button2\">Off</button></a></td>");
              } else {
                client.println("<td> <a href=\"/botmag/off\"><button class=\"button\">On</button></a></td>");
              }

                client.println("<tr> <td align=\"center\">Top Solenoid</td>");

              if (topsolenoidState=="off") {
                client.println("<td> <a href=\"/topnoid/on\"><button class=\"button button2\">Off</button></a></td>");
              } else {
                client.println("<td> <a href=\"/topnoid/off\"><button class=\"button\">On</button></a></td>");
              }

                client.println("<tr> <td align=\"center\">Bottom Solenoid</td>");

              if (bottomsolenoidState=="off") {
                client.println("<td> <a href=\"/botnoid/on\"><button class=\"button button2\">Off</button></a></td>");
              } else {
                client.println("<td> <a href=\"/botnoid/off\"><button class=\"button\">On</button></a></td>");
              }
        //    client.println("</td><tr><td align=\"center\">Bottom Magnet</td><td align=\"center\"><a href=\"/botmag/on\"><button class=\"button button2\">Off</button></a></td>");
        //    client.println("<tr><td align=\"center\">Top Solenoid</td><td align=\"center\"><a href=\"/topnoid/off\"><button class=\"button button2\">Off</button></a></td></tr>");
      //      client.println("<tr><td align=\"center\">Bottom Solenoid</td><td align=\"center\"><a href=\"/botnoid/on\"><button class=\"button button2\">Off</button></a></td></tr>");
            client.println(" </table></body></html>");

              // The HTTP response ends with another blank line
              client.println();
              // Break out of the while loop
              break;
            }
            // Wrong user or password, so HTTP request fails...
            else {
              client.println("HTTP/1.1 401 Unauthorized");
              client.println("WWW-Authenticate: Basic realm=\"Secure\"");
              client.println("Content-Type: text/html");
              client.println();
              client.println("<html>Authentication failed</html>");
            }
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }


    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

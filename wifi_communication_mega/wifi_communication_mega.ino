

#include "WiFiS3.h"



// Example
// char ssid[] = "2024000123";
char ssid[] = "Vendingmachine"; // your network SSID (name): please your student id as ssid
char pass[] = "91046116";        // your network password (minimum 8 characters long)
int cost = 0;

String output = "";
String header;

// Current time
unsigned long currentTime = millis();
unsigned long previousTime = 0;

// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  Serial.begin(9600); // initialize serial communication
  Serial1.begin(9600); // Mega와의 통신을 위해 추가
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create an access point:
  status = WiFi.beginAP(ssid, pass);
  
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  server.begin(); // start the web server on port 80
  printWifiStatus(); // you're connected now, so print out the status
}

void loop() {
  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      // a device has connected to the AP
      Serial.println("Device connected to AP");
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }

  webServer();  
}

void webServer() {
  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();


            if (header.indexOf("GET /500") >= 0) {
              cost = 500;
              Serial.println("Cost 값을 1로 설정 (500 Won)");
              Serial.println(cost);
              Serial1.println(cost); //Mega 전송
            } else if (header.indexOf("GET /1000") >= 0) {
              cost = 1000;              
              Serial.println("Cost 값을 2로 설정 (1000 Won)");
              Serial.println(cost);
              Serial1.println(cost); //Mega 전송
            }


            // 간단한 HTML 응답 페이지
            //head와 style
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println("button { padding: 15px 30px; font-size: 20px; margin: 10px; }");
            client.println("</style></head>"); //head와 style

            //body 부분
            client.println("<body><h1>Select Product</h1>");

            //button 추가
            client.println("<button onclick=\"location.href='/500'\">500 Won</button>");
            client.println("<button onclick=\"location.href='/1000'\">1000 Won</button>");
            client.println("</body></html>"); //body 끝

            client.println(); // HTTP 응답 끝
            break;
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

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("Now open this URL on your browser --> http://");
  Serial.println(ip);
}
#include "WiFiS3.h"

char ssid[] = "Vendingmachine";
char pass[] = "91046116";       
int cost = 0;

String output = "";
String header;

unsigned long currentTime = millis();
unsigned long previousTime = 0;

const long timeoutTime = 2000;

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  Serial.begin(9600); 
  Serial1.begin(9600); // Mega와의 통신을 위해 추가

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  status = WiFi.beginAP(ssid, pass);
  
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    while (true);
  }

  server.begin(); 
  printWifiStatus(); 
}

void loop() {
  if (status != WiFi.status()) {
    status = WiFi.status();
    if (status == WL_AP_CONNECTED) {
      Serial.println("Device connected to AP");
    } else {
      Serial.println("Device disconnected from AP");
    }
  }

  webServer();  
}

void webServer() {
  WiFiClient client = server.available();   
  if (client) {                             
    Serial.println("New Client.");          
    String currentLine = "";                
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { 
      currentTime = millis();
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                    
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /500") >= 0) {
              cost = 500;
              Serial.println("Cost 값을 500로 설정 (500 Won)");
              Serial.println(cost);
              Serial1.println(cost); // Mega로 전송
            } else if (header.indexOf("GET /1000") >= 0) {
              cost = 1000;
              Serial.println("Cost 값을 1000로 설정 (1000 Won)");
              Serial.println(cost);
              Serial1.println(cost); // Mega로 전송
            }

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println("button { padding: 15px 30px; font-size: 20px; margin: 10px; }");
            client.println("</style></head>");
            client.println("<body><h1>Select Product</h1>");
            client.println("<button onclick=\"location.href='/500'\">500 Won</button>");
            client.println("<button onclick=\"location.href='/1000'\">1000 Won</button>");
            client.println("</body></html>");

            client.println();
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.print("Now open this URL on your browser --> http://");
  Serial.println(ip);
}
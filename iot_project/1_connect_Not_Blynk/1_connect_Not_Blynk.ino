#include <ESP8266HTTPClient.h>

#include <SoftwareSerial.h>

SoftwareSerial esp8266(2, 3); // RX, TX

String ssid = "mstjr_2.4";
String password = "tjrmsrwifi";

String host = "192.168.1.239"; // Your Spring Boot server IP
int port = 8080;

void sendCommand(String cmd, int timeout = 2000) {
  esp8266.println(cmd);
  long int time = millis();

  while ((time + timeout) > millis()) {
    while (esp8266.available()) {
      char c = esp8266.read();
      Serial.write(c); // debug output
    }
  }
}

void setup() {
  Serial.begin(9600);
  esp8266.begin(9600);

  delay(2000);

  Serial.println("Initializing ESP8266...");

  sendCommand("AT");
  sendCommand("AT+CWMODE=1"); // Station mode

  // Connect to WiFi
  String cmd = "AT+CWJAP=\"" + ssid + "\",\"" + password + "\"";
  sendCommand(cmd, 6000);

  delay(15000);
}

void loop() {
  // Open TCP connection
  String cmd = String("AT+CIPSTART=\"TCP\",\"") + host + "\"," + String(port);
  sendCommand(cmd, 3000);

  // Create JSON payload
  String json = "{\"temperature\":98.6,\"humidity\":50.5}";

  // Build HTTP POST request
  String httpRequest =
      String("POST /api/sensors HTTP/1.1\r\n") +
      "Host: " + host + "\r\n" +
      "Content-Type: application/json\r\n" +
      "Content-Length: " + String(json.length()) + "\r\n" +
      "\r\n" +
      json;

  // Tell ESP8266 how many bytes we will send
  cmd = "AT+CIPSEND=" + String(httpRequest.length());
  sendCommand(cmd, 2000);

  delay(1000);

  // Send actual HTTP request
  esp8266.print(httpRequest);

  Serial.println("Request sent!");

  delay(5000);

  // Close connection
  sendCommand("AT+CIPCLOSE");

  delay(10000); // wait before next send
}

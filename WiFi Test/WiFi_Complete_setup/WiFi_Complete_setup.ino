#include <SoftwareSerial.h>
#include <DHT.h>
#include "arduino_secrets.h"

// Arduino Uno R3 + ESP8266 (AT firmware) + DHT sensor
// Sends temperature and humidity as JSON to a Spring Boot backend.

#define DHTPIN 4
#define DHTTYPE DHT11

SoftwareSerial esp8266(2, 3); // RX, TX
DHT dht(DHTPIN, DHTTYPE);

const char* WIFI_SSID = SECRET_SSID;
const char* WIFI_PASSWORD = SECRET_PASS;
const char* HOST = SECRET_SERVER_HOST;
const int PORT = SECRET_SERVER_PORT;
const char* PATH = SECRET_API_PATH;

bool sendATCommand(const String& command, const String& expected, unsigned long timeout) {
  esp8266.println(command);
  unsigned long start = millis();
  String response = "";

  while (millis() - start < timeout) {
    while (esp8266.available()) {
      char c = esp8266.read();
      response += c;
    }

    if (response.indexOf(expected) >= 0) {
      Serial.println("ESP8266 OK: " + command);
      return true;
    }
  }

  Serial.println("ESP8266 ERROR: " + command);
  Serial.println(response);
  return false;
}

bool connectWiFi() {
  Serial.println("Starting connectWifi");
  if (!sendATCommand("AT", "OK", 2000)) return false;
  if (!sendATCommand("AT+CWMODE=1", "OK", 2000)) return false;

  String joinCmd = "AT+CWJAP=\"";
  joinCmd += WIFI_SSID;
  joinCmd += "\",\"";
  joinCmd += WIFI_PASSWORD;
  joinCmd += "\"";

  return sendATCommand(joinCmd, "WIFI GOT IP", 15000) || sendATCommand("", "OK", 3000);
}

bool postSensorData(float temperature, float humidity) {
  String jsonBody = "{\"temperature\":";
  jsonBody += String(temperature, 2);
  jsonBody += ",\"humidity\":";
  jsonBody += String(humidity, 2);
  jsonBody += "}";

  String httpRequest = "POST ";
  httpRequest += PATH;
  httpRequest += " HTTP/1.1\r\n";
  httpRequest += "Host: ";
  httpRequest += HOST;
  httpRequest += ":";
  httpRequest += PORT;
  httpRequest += "\r\n";
  httpRequest += "Content-Type: application/json\r\n";
  httpRequest += "Connection: close\r\n";
  httpRequest += "Content-Length: ";
  httpRequest += jsonBody.length();
  httpRequest += "\r\n\r\n";
  httpRequest += jsonBody;

  String cipstart = "AT+CIPSTART=\"TCP\",\"";
  cipstart += HOST;
  cipstart += "\",";
  cipstart += PORT;
  if (!sendATCommand(cipstart, "OK", 10000)) return false;

  String cipsend = "AT+CIPSEND=" + String(httpRequest.length());
  if (!sendATCommand(cipsend, ">", 5000)) return false;

  esp8266.print(httpRequest);
  delay(5000);

  String response = "";
  while (esp8266.available()) {
    response += (char)esp8266.read();
  }

  Serial.println("Server response:");
  Serial.println(response);

  sendATCommand("AT+CIPCLOSE", "OK", 3000);
  return response.indexOf("200 OK") >= 0 || response.indexOf("201") >= 0;
}

void setup() {
  Serial.begin(9600);
  esp8266.begin(9600);
  dht.begin();

  delay(2000);
  Serial.println("Connecting to WiFi...");

  if (connectWiFi()) {
    Serial.println("WiFi connected.");
  } else {
    Serial.println("WiFi connection failed.");
  }
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor.");
    delay(10000);
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  if (postSensorData(temperature, humidity)) {
    Serial.println("POST request sent successfully.");
  } else {
    Serial.println("POST request failed.");
  }

  delay(30000);
}

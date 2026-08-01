/*
  Expected File
  arduino_secrets.h

  Must contain #define for
  #define SECRET_SSID "SSID For WIFI Network"
  #define SECRET_PASS "Password For SSID Above"
  #define SECRET_SERVER_HOST "IP Or DNS name for backend API"
  #define SECRET_SERVER_PORT 8080 or "Port for Backend API"
  #define SECRET_API_PATH    "/api/sensors" Or "Whatever your API path is for PUT command"
*/

#include <WiFi.h>
#include <HTTPClient.h>

#include "arduino_secrets.h"
#include "wifi_functions.h"

const unsigned long POST_INTERVAL_MS = 2000;
const int LED_PIN = 2;

unsigned long lastPostTime = 0;
unsigned long passValue = 1;
unsigned long postAttempts = 0;
unsigned long successfulPosts = 0;
bool ledState = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);
  delay(1000);

  Serial.println();
  Serial.println("========================================");
  Serial.println(" ESP32 REST POST client starting");
  Serial.println("========================================");
  Serial.print("[CONFIG] SSID: ");
  Serial.println(SECRET_SSID);
  Serial.print("[CONFIG] Backend: http://");
  Serial.print(SECRET_SERVER_HOST);
  Serial.print(":");
  Serial.print(SECRET_SERVER_PORT);
  Serial.println(SECRET_API_PATH);
  Serial.print("[CONFIG] POST interval: ");
  Serial.print(POST_INTERVAL_MS);
  Serial.println(" ms");

  if (!connectToWiFi()) {
    Serial.println("[APP] Wi-Fi is unavailable; continuing without a network connection.");
    Serial.println("[APP] A new connection will be attempted before the next POST.");
  }

  // Allow the first POST to occur immediately when loop() starts.
  lastPostTime = millis() - POST_INTERVAL_MS;
}

void loop() {
  const unsigned long currentTime = millis();

  if (currentTime - lastPostTime >= POST_INTERVAL_MS) {
    lastPostTime = currentTime;
    postSensorReading();
  }
}

void postSensorReading() {
  postAttempts++;
  Serial.println();
  Serial.println("----------------------------------------");
  Serial.print("[POST] Attempt #");
  Serial.print(postAttempts);
  Serial.print(" at millis=");
  Serial.println(millis());

  if (!connectToWiFi()) {
    Serial.println("[POST] Cancelled because Wi-Fi is not connected.");
    return;
  }

  String url = "http://";
  url += SECRET_SERVER_HOST;
  url += ":";
  url += String(SECRET_SERVER_PORT);
  url += SECRET_API_PATH;

  Serial.print("[POST] Testing TCP connection to ");
  Serial.print(SECRET_SERVER_HOST);
  Serial.print(":");
  Serial.println(SECRET_SERVER_PORT);

  WiFiClient connectionTest;
  connectionTest.setTimeout(5000);
  if (!connectionTest.connect(SECRET_SERVER_HOST, SECRET_SERVER_PORT)) {
    Serial.println("[TCP] Connection failed.");
    Serial.println("[TCP] Confirm that:");
    Serial.println("      1. The backend is running.");
    Serial.println("      2. It is listening on port 8080.");
    Serial.println("      3. Windows Firewall allows inbound TCP 8080.");
    Serial.println("      4. ESP32 and backend computer are on the same LAN.");
    return;
  }
  Serial.println("[TCP] Backend port is reachable.");
  connectionTest.stop();

  WiFiClient client;
  HTTPClient http;

  if (!http.begin(client, url)) {
    Serial.println("[HTTP] Unable to initialize HTTP connection.");
    return;
  }

  http.setConnectTimeout(5000);
  http.setTimeout(5000);
  http.setReuse(false);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.addHeader("X-Device-Id", "esp32-devkit");

  String payload = "{\"temperature\":100.13,\"humidity\":101.12,\"light\":52,\"passValue\":";
  payload += String(passValue);
  payload += "}";

  Serial.print("[HTTP] POST ");
  Serial.println(url);
  Serial.print("[HTTP] Content-Length: ");
  Serial.println(payload.length());
  Serial.print("[HTTP] Payload: ");
  Serial.println(payload);

  const unsigned long requestStarted = millis();
  const int statusCode = http.POST(payload);
  const unsigned long requestDuration = millis() - requestStarted;

  if (statusCode > 0) {
    Serial.print("[HTTP] Status: ");
    Serial.println(statusCode);
    Serial.print("[HTTP] Duration: ");
    Serial.print(requestDuration);
    Serial.println(" ms");

    const String response = http.getString();
    Serial.print("[HTTP] Response: ");
    if (response.isEmpty()) {
      Serial.println("<empty>");
    } else {
      Serial.println(response);
    }

    if (statusCode >= 200 && statusCode < 300) {
      successfulPosts++;
      Serial.println("[HTTP] SUCCESS: backend accepted the reading.");
    } else {
      Serial.println("[HTTP] ERROR: backend returned a non-2xx response.");
    }
  } else {
    Serial.print("[HTTP] POST failed. Error code: ");
    Serial.println(statusCode);
    Serial.print("[HTTP] Error: ");
    Serial.println(HTTPClient::errorToString(statusCode));
  }

  http.end();
  passValue++;

  ledState = !ledState;
  digitalWrite(LED_PIN, ledState);

  Serial.print("[LED] GPIO 2 is now ");
  Serial.println(ledState ? "HIGH" : "LOW");
  Serial.print("[STATS] Attempts: ");
  Serial.print(postAttempts);
  Serial.print(", successful: ");
  Serial.print(successfulPosts);
  Serial.print(", next passValue: ");
  Serial.println(passValue);
}

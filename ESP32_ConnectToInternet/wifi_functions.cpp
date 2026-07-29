#include "wifi_functions.h"

#include <Arduino.h>
#include <WiFi.h>

#include "arduino_secrets.h"

namespace {
constexpr unsigned long WIFI_TIMEOUT_MS = 15000;
constexpr unsigned long WIFI_RETRY_DELAY_MS = 250;
}

bool connectToWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[WiFi] Already connected. RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    return true;
  }

  Serial.print("[WiFi] Connecting to ");
  Serial.print(SECRET_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.disconnect();
  delay(200);
  WiFi.begin(SECRET_SSID, SECRET_PASS);

  const unsigned long connectionStart = millis();
  while (WiFi.status() != WL_CONNECTED &&
         millis() - connectionStart < WIFI_TIMEOUT_MS) {
    Serial.print(".");
    delay(WIFI_RETRY_DELAY_MS);
  }

  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("[WiFi] ERROR: Connection failed. Wi-Fi status code: ");
    Serial.println(static_cast<int>(WiFi.status()));
    Serial.println("[WiFi] Check SECRET_SSID, SECRET_PASS, and 2.4 GHz Wi-Fi.");
    Serial.println("[WiFi] Returning control to the application.");
    return false;
  }

  Serial.print("[WiFi] Connected. ESP32 IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("[WiFi] Gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("[WiFi] Subnet mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("[WiFi] RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  return true;
}

#include "http_functions.h"
#include <HTTPClient.h>
#include <WiFi.h>

#include "arduino_secrets.h"
#include "oled_functions.h"
#include "wifi_functions.h"

namespace
{
  constexpr unsigned long HTTP_TIMEOUT_MS = 5000;

  String createPayload(float temperature,
                       float humidity,
                       uint8_t light,
                       unsigned long passValue,
                       const String &utcTime)
  {
    String payload;

    const float temperatureF = (temperature * 9.0F / 5.0F) + 32.0F;

    payload.reserve(96);
    payload += "{\"temperature\":";
    payload += String(temperatureF, 1);
    payload += ",\"humidity\":";
    payload += String(humidity, 1);
    payload += ",\"light\":";
    payload += String(light);
    payload += ",\"passValue\":";
    payload += String(passValue);
    payload += ",\"sentAt\":\"";
    payload += utcTime;
    payload += "\"";
    payload += "}";
    return payload;
  }
}

bool postSensorReading(float temperature,
                       float humidity,
                       uint8_t light,
                       unsigned long passValue,
                       const String &utcTime,
                       Adafruit_SSD1306 &temperatureDisplay,
                       Adafruit_SSD1306 &timeDisplay)
{
  OledFunctions::updateTimeDisplay(timeDisplay, utcTime);
  OledFunctions::updateTemperatureDisplay(
      temperatureDisplay, temperature, humidity);

  if (!connectToWiFi())
  {
    Serial.println("[POST] Cancelled because Wi-Fi is not connected.");
    return false;
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
  connectionTest.setTimeout(HTTP_TIMEOUT_MS);
  if (!connectionTest.connect(SECRET_SERVER_HOST, SECRET_SERVER_PORT))
  {
    Serial.println("[TCP] Connection failed.");
    Serial.println("[TCP] Confirm that:");
    Serial.println("      1. The backend is running.");
    Serial.println("      2. It is listening on the configured port.");
    Serial.println("      3. The firewall allows inbound traffic on that port.");
    Serial.println("      4. ESP32 and backend computer are on the same LAN.");
    return false;
  }
  Serial.println("[TCP] Backend port is reachable.");
  connectionTest.stop();

  WiFiClient client;
  HTTPClient http;
  if (!http.begin(client, url))
  {
    Serial.println("[HTTP] Unable to initialize HTTP connection.");
    return false;
  }

  http.setConnectTimeout(HTTP_TIMEOUT_MS);
  http.setTimeout(HTTP_TIMEOUT_MS);
  http.setReuse(false);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.addHeader("X-Device-Id", "esp32-devkit");

  const String payload = createPayload(temperature, humidity, light, passValue, utcTime);
  Serial.print("[HTTP] POST ");
  Serial.println(url);
  Serial.print("[HTTP] Content-Length: ");
  Serial.println(payload.length());
  Serial.print("[HTTP] Payload: ");
  Serial.println(payload);

  const unsigned long requestStarted = millis();
  const int statusCode = http.POST(payload);
  const unsigned long requestDuration = millis() - requestStarted;
  bool succeeded = false;

  if (statusCode > 0)
  {
    Serial.print("[HTTP] Status: ");
    Serial.println(statusCode);
    Serial.print("[HTTP] Duration: ");
    Serial.print(requestDuration);
    Serial.println(" ms");

    const String response = http.getString();
    Serial.print("[HTTP] Response: ");
    Serial.println(response.isEmpty() ? "<empty>" : response);

    succeeded = statusCode >= 200 && statusCode < 300;
    Serial.println(succeeded
                       ? "[HTTP] SUCCESS: backend accepted the reading."
                       : "[HTTP] ERROR: backend returned a non-2xx response.");
  }
  else
  {
    Serial.print("[HTTP] POST failed. Error code: ");
    Serial.println(statusCode);
    Serial.print("[HTTP] Error: ");
    Serial.println(HTTPClient::errorToString(statusCode));
  }

  http.end();
  return succeeded;
}

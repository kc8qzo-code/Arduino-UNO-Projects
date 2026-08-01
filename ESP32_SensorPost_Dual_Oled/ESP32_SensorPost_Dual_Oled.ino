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
#include <RTClib.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "arduino_secrets.h"
#include "http_functions.h"
#include "oled_functions.h"
#include "wifi_functions.h"
#include "date_functions.h"

const unsigned long POST_INTERVAL_MS = 2000;

// I2C bus 0: Temperature OLED + DS3231 RTC
constexpr int I2C0_SDA_PIN = 21;
constexpr int I2C0_SCL_PIN = 22;

// I2C bus 1: Time OLED
constexpr int I2C1_SDA_PIN = 25;
constexpr int I2C1_SCL_PIN = 26;

// DHT22 data pin
constexpr int DHT_PIN = 4;
constexpr uint8_t DHT_TYPE = DHT22;

// Most SSD1306 OLED modules use 0x3C.
constexpr uint8_t OLED_ADDRESS = 0x3C;

constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 64;
constexpr int OLED_RESET_PIN = -1;

// Refresh periods
constexpr unsigned long DHT_REFRESH_MS = 5000;
const int ONBOARD_LED_PIN = 2;

// Wire uses ESP32 I2C controller 0 for the temperature OLED and RTC.
// This additional object uses ESP32 I2C controller 1 for the time OLED.
TwoWire timeI2C(1);

RTC_DS3231 rtc;
// DHT dht(DHT_PIN, DHT_TYPE);

Adafruit_SSD1306 temperatureDisplay(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    OLED_RESET_PIN);

Adafruit_SSD1306 timeDisplay(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &timeI2C,
    OLED_RESET_PIN);

float temperature = 100.13F;
float humidity = 101.12F;
uint8_t light = 52;

unsigned long lastDhtRefresh = 0;
unsigned long lastPostTime = 0;
unsigned long passValue = 1;
unsigned long postAttempts = 0;
unsigned long successfulPosts = 0;
bool ledState = LOW;

[[noreturn]] void haltWithMessage(const char *message);

void setup()
{
  Serial.begin(115200);
  pinMode(ONBOARD_LED_PIN, OUTPUT);
  digitalWrite(ONBOARD_LED_PIN, ledState);
  delay(1000);

  // Start I2C controller 0.
  if (!Wire.begin(
          I2C0_SDA_PIN,
          I2C0_SCL_PIN,
          400000))
  {
    haltWithMessage("Could not start I2C bus 0.");
  }

  // Start I2C controller 1.
  if (!timeI2C.begin(
          I2C1_SDA_PIN,
          I2C1_SCL_PIN,
          400000))
  {
    haltWithMessage("Could not start I2C bus 1.");
  }

  if (!temperatureDisplay.begin(
          SSD1306_SWITCHCAPVCC,
          OLED_ADDRESS,
          false,
          false))
  {
    haltWithMessage(
        "Temperature OLED was not found.");
  }

  if (!timeDisplay.begin(
          SSD1306_SWITCHCAPVCC,
          OLED_ADDRESS,
          false,
          false))
  {
    haltWithMessage(
        "Time OLED was not found.");
  }

  showStartupScreen(
      temperatureDisplay,
      "Starting DHT22...");

  showStartupScreen(
      timeDisplay,
      "Starting DS3231...");

  // dht.begin();

  // Connect RTClib to I2C controller 0.
  if (!rtc.begin(&Wire))
  {
    haltWithMessage(
        "DS3231 RTC was not found.");
  }

  lastDhtRefresh = millis();

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

  if (!connectToWiFi())
  {
    Serial.println("[APP] Wi-Fi is unavailable; continuing without a network connection.");
    Serial.println("[APP] A new connection will be attempted before the next POST.");
  }

  // Allow the first POST to occur immediately when loop() starts.
  lastPostTime = millis() - POST_INTERVAL_MS;
}

void loop()
{
  const unsigned long currentMillis = millis();

  if (currentMillis - lastPostTime >= POST_INTERVAL_MS)
  {
    lastPostTime = currentMillis;
    postAttempts++;
    Serial.println();
    Serial.println("----------------------------------------");
    Serial.print("[POST] Attempt #");
    Serial.print(postAttempts);
    Serial.print(" at millis=");
    Serial.println(millis());

    const DateTime now = rtc.now();
    const String utcTime = buildUTCDate(now);

    if (postSensorReading(
            temperature,
            humidity,
            light,
            passValue,
            utcTime,
            temperatureDisplay,
            timeDisplay))
    {
      successfulPosts++;
    }

    passValue++;
    ledState = !ledState;
    digitalWrite(ONBOARD_LED_PIN, ledState);

    Serial.print("[STATS] Attempts: ");
    Serial.print(postAttempts);
    Serial.print(", successful: ");
    Serial.print(successfulPosts);
    Serial.print(", next passValue: ");
    Serial.println(passValue);
    Serial.println(", Sent Time: ");
    Serial.println(utcTime);
  }

  if (currentMillis - lastDhtRefresh >= DHT_REFRESH_MS)
  {
    lastDhtRefresh = currentMillis;
    //   temperature = dht.readTemperature();
    //   humidity = dht.readHumidity();
    humidity++;
    temperature++;
  }
}

[[noreturn]] void haltWithMessage(const char *message)
{
  Serial.println(message);

  while (true)
  {
    delay(1000);
  }
}

void showStartupScreen(
    Adafruit_SSD1306 &display,
    const char *message)
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);

  display.println("ESP32 Sensor Clock");
  display.println();
  display.println(message);
  display.display();
}

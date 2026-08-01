#include <Wire.h>
#include <RTClib.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ============================================================
// ESP32 Elegoo DevKit V1 pin assignments
// ============================================================

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
constexpr unsigned long CLOCK_REFRESH_MS = 1000;
constexpr unsigned long DHT_REFRESH_MS = 2500;

// ============================================================
// Hardware objects
// ============================================================


// Wire uses ESP32 I2C controller 0 for the temperature OLED and RTC.
// This additional object uses ESP32 I2C controller 1 for the time OLED.
TwoWire timeI2C(1);

RTC_DS3231 rtc;
//DHT dht(DHT_PIN, DHT_TYPE);

Adafruit_SSD1306 temperatureDisplay(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    OLED_RESET_PIN
);

Adafruit_SSD1306 timeDisplay(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &timeI2C,
    OLED_RESET_PIN
);

unsigned long lastClockRefresh = 0;
unsigned long lastDhtRefresh = 0;

const char* DAYS_OF_WEEK[] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"
};

// Declare this explicitly so Arduino's generated prototype retains the
// [[noreturn]] attribute used by the definition below.
[[noreturn]] void haltWithMessage(const char* message);

// ============================================================
// Arduino setup
// ============================================================

void setup() {
  Serial.begin(115200);
  delay(250);

  // Start I2C controller 0.
  if (!Wire.begin(
          I2C0_SDA_PIN,
          I2C0_SCL_PIN,
          400000)) {
    haltWithMessage("Could not start I2C bus 0.");
  }

  // Start I2C controller 1.
  if (!timeI2C.begin(
          I2C1_SDA_PIN,
          I2C1_SCL_PIN,
          400000)) {
    haltWithMessage("Could not start I2C bus 1.");
  }

  /*
   * The final two false arguments mean:
   *
   * 1. Do not use a hardware reset pin.
   * 2. Do not call Wire.begin() again because both
   *    I2C buses have already been initialized.
   */

  if (!temperatureDisplay.begin(
          SSD1306_SWITCHCAPVCC,
          OLED_ADDRESS,
          false,
          false)) {
    haltWithMessage(
        "Temperature OLED was not found."
    );
  }

  if (!timeDisplay.begin(
          SSD1306_SWITCHCAPVCC,
          OLED_ADDRESS,
          false,
          false)) {
    haltWithMessage(
        "Time OLED was not found."
    );
  }

  showStartupScreen(
      temperatureDisplay,
      "Starting DHT22..."
  );

  showStartupScreen(
      timeDisplay,
      "Starting DS3231..."
  );

  //dht.begin();

  // Connect RTClib to I2C controller 0.
  if (!rtc.begin(&Wire)) {
    haltWithMessage(
        "DS3231 RTC was not found."
    );
  }

  /*
   * When the DS3231 reports that backup power was lost,
   * set it to the date and time at which this sketch
   * was compiled.
   *
   * It will not reset the clock on every normal reboot.
   */
  if (rtc.lostPower()) {
    Serial.println(
        "RTC lost power; setting compile time."
    );

    // Set the RTC to four hours after the sketch compile time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)) + TimeSpan(0, 4, 0, 0));
  }

  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  updateTemperatureDisplay();
  updateTimeDisplay();

  lastDhtRefresh = millis();
  lastClockRefresh = millis();
}

// ============================================================
// Arduino loop
// ============================================================

void loop() {
  const unsigned long currentMillis = millis();

  if (currentMillis - lastClockRefresh
      >= CLOCK_REFRESH_MS) {
    lastClockRefresh = currentMillis;
    updateTimeDisplay();
  }

  if (currentMillis - lastDhtRefresh
      >= DHT_REFRESH_MS) {
    lastDhtRefresh = currentMillis;
    updateTemperatureDisplay();
  }
}

// ============================================================
// Utility functions
// ============================================================

[[noreturn]] void haltWithMessage(const char* message) {
  Serial.println(message);

  while (true) {
    delay(1000);
  }
}

void showStartupScreen(
    Adafruit_SSD1306& display,
    const char* message
) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);

  display.println("ESP32 Sensor Clock");
  display.println();
  display.println(message);
  display.display();
}

// ============================================================
// Temperature OLED
// ============================================================

void updateTemperatureDisplay() {
  const float humidity = 55.123f;
  const float temperatureC = 22.1f;

  temperatureDisplay.clearDisplay();
  temperatureDisplay.setTextColor(SSD1306_WHITE);

  temperatureDisplay.setTextSize(1);
  temperatureDisplay.setCursor(0, 0);
  temperatureDisplay.println("TEMPERATURE");

  if (isnan(humidity) || isnan(temperatureC)) {
    temperatureDisplay.setTextSize(2);
    temperatureDisplay.setCursor(8, 20);
    temperatureDisplay.println("DHT22");

    temperatureDisplay.setTextSize(1);
    temperatureDisplay.println("Read failed");
    temperatureDisplay.display();

    Serial.println("DHT22 read failed.");
    return;
  }

  const float temperatureF = (temperatureC * 9.0F / 5.0F) + 32.0F;

  // Large Fahrenheit reading
  temperatureDisplay.setTextSize(2);
  temperatureDisplay.setCursor(8, 13);
  temperatureDisplay.print(temperatureF, 1);

  temperatureDisplay.setTextSize(2);
  temperatureDisplay.print(" F");

  // Celsius reading
  temperatureDisplay.setTextSize(1);
  temperatureDisplay.setCursor(8, 43);
  temperatureDisplay.print("Celsius:  ");
  temperatureDisplay.print(temperatureC, 1);
  temperatureDisplay.println(" C");

  // Humidity reading
  temperatureDisplay.setCursor(8, 54);
  temperatureDisplay.print("Humidity: ");
  temperatureDisplay.print(humidity, 1);
  temperatureDisplay.println(" %");

  temperatureDisplay.display();

  Serial.print("Temperature: ");
  Serial.print(temperatureF, 1);
  Serial.print(" F / ");
  Serial.print(temperatureC, 1);
  Serial.print(" C, Humidity: ");
  Serial.print(humidity, 1);
  Serial.println(" %");
}

// ============================================================
// Time OLED
// ============================================================
void updateTimeDisplay() {
  const DateTime now = rtc.now();

  // Convert the DS3231's 24-hour value to 12-hour time.
  uint8_t hour12 = now.hour() % 12;

  if (hour12 == 0) {
    hour12 = 12;
  }

  const char* timeInAmPm =
      now.hour() >= 12 ? "PM" : "AM";

  char timeBuffer[9];
  char dateBuffer[11];

  snprintf(
      timeBuffer,
      sizeof(timeBuffer),
      "%02u:%02u:%02u",
      hour12,
      now.minute(),
      now.second()
  );

  snprintf(
      dateBuffer,
      sizeof(dateBuffer),
      "%02u/%02u/%04u",
      now.month(),
      now.day(),
      now.year()
  );

  timeDisplay.clearDisplay();
  timeDisplay.setTextColor(SSD1306_WHITE);

  timeDisplay.setTextSize(1);
  timeDisplay.setCursor(0, 0);
  timeDisplay.println("CURRENT TIME");

  timeDisplay.setTextSize(2);
  timeDisplay.setCursor(0, 15);
  timeDisplay.print(timeBuffer);

  timeDisplay.setTextSize(1);
  timeDisplay.setCursor(104, 21);
  timeDisplay.print(timeInAmPm);

  timeDisplay.setCursor(0, 42);
  timeDisplay.print(dateBuffer);

  timeDisplay.setCursor(0, 54);
  timeDisplay.print(
      DAYS_OF_WEEK[now.dayOfTheWeek()]
  );

  timeDisplay.display();
}

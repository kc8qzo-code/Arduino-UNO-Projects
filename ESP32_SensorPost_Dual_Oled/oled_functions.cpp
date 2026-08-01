#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include "oled_functions.h"
#include "date_functions.h"

bool OledFunctions::updateTimeDisplay(Adafruit_SSD1306 &display,
                                      const String &utcTime)
{
  DateTime dateTime;
  if (!parseUTCDate(utcTime, dateTime))
  {
    Serial.print("[TIME] Invalid UTC timestamp: ");
    Serial.println(utcTime);
    return false;
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("CURRENT TIME (UTC)");

  display.setTextSize(1);
  display.setCursor(0, 16);
  display.print(buildReadableTime(dateTime));

  display.setTextSize(1);
  display.setCursor(98, 19);
  display.print(dateTime.hour() >= 12 ? "PM" : "AM");

  display.setCursor(0, 36);
  display.print(buildReadableDate(dateTime));

  display.setCursor(0, 54);
  display.print(DAYS_OF_WEEK[dateTime.dayOfTheWeek()]);
  display.display();
  return true;
}

void OledFunctions::updateTemperatureDisplay(Adafruit_SSD1306 &display,
                                             float temperatureC,
                                             float humidity)
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("TEMPERATURE");

  if (isnan(humidity) || isnan(temperatureC))
  {
    display.setTextSize(1);
    display.setCursor(8, 20);
    display.println("DHT22");

    display.setTextSize(1);
    display.println("Read failed");
    display.display();

    Serial.println("DHT22 read failed.");
    return;
  }

  const float temperatureF = (temperatureC * 9.0F / 5.0F) + 32.0F;

  display.setTextSize(1);
  display.setCursor(8, 16);
  display.print(temperatureF, 1);
  display.print(" F");

  display.setTextSize(1);
  display.setCursor(8, 36);
  display.print("Celsius:  ");
  display.print(temperatureC, 1);
  display.println(" C");

  display.setCursor(8, 54);
  display.print("Humidity: ");
  display.print(humidity, 1);
  display.println(" %");

  display.display();

  Serial.print("Displayed Temperature: ");
  Serial.print(temperatureF, 1);
  Serial.print(" F / ");
  Serial.print(temperatureC, 1);
  Serial.print(" C, Humidity: ");
  Serial.print(humidity, 1);
  Serial.println(" %");
}

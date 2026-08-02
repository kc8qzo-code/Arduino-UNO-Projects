#ifndef OLED_FUNCTIONS_H
#define OLED_FUNCTIONS_H

#include <Arduino.h>

class Adafruit_SSD1306;

class OledFunctions
{
public:
  static void showStartupScreen(Adafruit_SSD1306 &display,
                                const char *message);

  static bool updateTimeDisplay(Adafruit_SSD1306 &display,
                                const String &utcTime);

  static void updateTemperatureDisplay(Adafruit_SSD1306 &display,
                                       float temperatureC,
                                       float humidity);
};

#endif

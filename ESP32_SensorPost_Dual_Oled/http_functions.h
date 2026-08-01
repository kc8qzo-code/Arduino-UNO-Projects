#ifndef HTTP_FUNCTIONS_H
#define HTTP_FUNCTIONS_H

#include <Arduino.h>

class Adafruit_SSD1306;

// Sends one sensor reading to the configured backend.
// Returns true only when the backend responds with a 2xx status code.
bool postSensorReading(float temperature,
                       float humidity,
                       uint8_t light,
                       unsigned long passValue,
                       const String &utcTime,
                       Adafruit_SSD1306 &timeDisplay);

#endif

#include <DS3231.h>
#include <Wire.h>

DS3231 rtc;

void setup() {
  Serial.begin(115200);
  Serial.println("Initialize RTC module");
  
  Wire.begin();
  
  // Set the time and date to match your computer's compile time.
  // This automatically sets the RTC to the exact moment you upload the code.
  rtc.adjust(DateTime(__DATE__, __TIME__));
}

void loop() {
  // Empty
}
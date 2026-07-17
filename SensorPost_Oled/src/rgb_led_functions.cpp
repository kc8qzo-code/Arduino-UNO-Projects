#include "rgb_led_functions.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

namespace {
constexpr int RED_PIN = 11;
constexpr int GREEN_PIN = 10;
constexpr int BLUE_PIN = 9;

constexpr int FADE_DELAY = 10;
}

bool initializeRgbLed() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  return true;
}

void updateRgbLed(int r, int g, int b) {
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
}

void actuateColorChange(){
  // Phase 1: Red is fully on. Fade in Green (Red + Green = Yellow)
  for (int g = 0; g <= 255; g++) {
    updateRgbLed(255, g, 0);
    delay(FADE_DELAY);
  }

  // Phase 2: Green is fully on. Fade out Red (Pure Green)
  for (int r = 255; r >= 0; r--) {
    updateRgbLed(r, 255, 0);
    delay(FADE_DELAY);
  }

  // Phase 3: Green is fully on. Fade in Blue (Green + Blue = Cyan)
  for (int b = 0; b <= 255; b++) {
    updateRgbLed(0, 255, b);
    delay(FADE_DELAY);
  }

  // Phase 4: Blue is fully on. Fade out Green (Pure Blue)
  for (int g = 255; g >= 0; g--) {
    updateRgbLed(0, g, 255);
    delay(FADE_DELAY);
  }

  // Phase 5: Blue is fully on. Fade in Red (Blue + Red = Magenta/Purple)
  for (int r = 0; r <= 255; r++) {
    updateRgbLed(r, 0, 255);
    delay(FADE_DELAY);
  }

  // Phase 6: Red is fully on. Fade out Blue (Back to Pure Red)
  for (int b = 255; b >= 0; b--) {
    updateRgbLed(255, 0, b);
    delay(FADE_DELAY);
  }
}

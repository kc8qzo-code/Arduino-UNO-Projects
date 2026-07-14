#include <Arduino_BuiltIn.h>
#include <ArduinoGraphics.h>
#include <Arduino_LED_Matrix.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

ArduinoLEDMatrix matrix;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // See datasheet for Address; 0x3C for most 128x48/128x64 displays

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ═══════════════════════════════════════════════════════════════════════════════
void setup() {
  Serial.begin(19200);

  matrix.begin();

   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextWrap(false);  // Allow text to trail off edge

  updateMatrix("T.J.");
}

// ═══════════════════════════════════════════════════════════════════════════════
void loop() {

  updateOled("Hello World");

  delay(5000);
}

// Updates OLED 128x64
void updateOled(String value) {
  // Clear the buffer
  display.clearDisplay();

  display.setCursor(0,28);     // Start at top-left corner
  display.println(value);
  display.display(); // Actually push the text to the screen
}

// Updates onboard Matrix on UNO R4 Board
void updateMatrix(String text){
  matrix.beginDraw();
  matrix.stroke(0x0000FF);
  matrix.textScrollSpeed(75);
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF); // Center text
  matrix.println(text);
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
}

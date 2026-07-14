#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

const unsigned long CLEAR_DELAY = 3000;   // 3 seconds
const int GATE_UP = 90;
const int GATE_DOWN = 0;

// Pin assignments

const int SERVO_PIN = 9;
const int TRIG_PIN = 10;
const int ECHO_PIN = 11;

const int DETECT_DISTANCE = 10;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // See datasheet for Address; 0x3C for most 128x48/128x64 displays

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Servo gateServo;  // Create a servo object to control the motor

bool trainPresent = false;
bool gateDown = false;
unsigned long clearTimer = 0;

// ═══════════════════════════════════════════════════════════════════════════════
void setup() {
  Serial.begin(19200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  gateServo.attach(SERVO_PIN);  // Attach the servo signal to digital pin 10
  gateServo.write(GATE_UP);

   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextWrap(false);  // Allow text to trail off edge
}

// ═══════════════════════════════════════════════════════════════════════════════
void loop() {

  float returnedDistance = getDistance();

  // Print the distance to the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(returnedDistance);
  Serial.println(" cm");

  String updatedString = String(returnedDistance) + " CM";
  updateOled(updatedString);

  if (returnedDistance > 0 && returnedDistance < DETECT_DISTANCE) {

    trainPresent = true;
    clearTimer = millis();

    lowerGate();

  } else {

    if (trainPresent &&
      millis() - clearTimer > CLEAR_DELAY) {

      trainPresent = false;

      raiseGate();
      
    }
  }

  delay(100);
}

float getDistance() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0)
    return -1;

  return duration * 0.0343 / 2.0;
}

void lowerGate() {

  if (!gateDown) {

    Serial.println("Gate DOWN");

    for (int pos = GATE_UP; pos >= GATE_DOWN; pos--) {
      gateServo.write(pos);
      delay(15);
    }

    gateDown = true;
  }
}

void raiseGate() {

  if (gateDown) {

    Serial.println("Gate UP");

    for (int pos = GATE_DOWN; pos <= GATE_UP; pos++) {
      gateServo.write(pos);
      delay(15);
    }

    gateDown = false;
  }
}

// Helper Functions

// Updates OLED 128x64
void updateOled(String value) {
  // Clear the buffer
  display.clearDisplay();

  display.setCursor(0,28);     // Start at top-left corner
  display.println(value);
  display.display(); // Actually push the text to the screen
}

#include <IRremote.hpp>
#include "rgb_led_functions.h"

// Change this line to your new pin
const int RECV_PIN = 6; 

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK); // Start the receiver

  initializeRgbLed();
  updateRgbLed(15,15,15);
}

void loop() {
  if (IrReceiver.decode()) {
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    Serial.println("Encoding");
    IrReceiver.resume();
  }
}
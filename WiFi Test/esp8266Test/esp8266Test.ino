#include <SoftwareSerial.h>

SoftwareSerial esp(2, 3); // RX, TX

void setup() {
  Serial.begin(115200);
  esp.begin(115200); // or try 115200 if nothing shows

  Serial.println("Ready");
}

void loop() {
  if (esp.available()) {
    Serial.write(esp.read());
  }
  if (Serial.available()) {
    esp.write(Serial.read());
  }
}
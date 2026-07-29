void setup() {
  Serial.begin(115200); // Start serial communication
  pinMode(2, OUTPUT);   // Set built-in LED pin as output
}

void loop() {
  Serial.println("Hello World!"); // Print message
  digitalWrite(2, HIGH);          // Turn LED on
  delay(1000);                    // Wait 1 second
  digitalWrite(2, LOW);           // Turn LED off
  delay(1000);                    // Wait 1 second
}
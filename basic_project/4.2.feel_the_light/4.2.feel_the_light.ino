#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
}

// photoresistor to VCC , 10k resistor to GND
// much dark , much low value

void loop() {
  int sensorValue = analogRead(A0);

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  Serial.print("Temperature: ");
  Serial.print((temperature * 9.0 /5.0) + 32.0);
  Serial.println(" F");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.println(sensorValue);
  delay(1000);       
}

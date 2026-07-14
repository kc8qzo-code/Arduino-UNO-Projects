#include <DHT.h>
#include <LiquidCrystal_I2C.h>


#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight
  dht.begin();
}

// photoresistor to VCC , 10k resistor to GND
// much dark , much low value

void loop() {
  int sensorValue = analogRead(A0);

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(true);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" F");

  Serial.print("Humidity: ");

  Serial.print(humidity);
  Serial.println(" %");

  lcd.clear();
  Serial.println(sensorValue);
  lcd.setCursor(1, 0); // set the cursor to column 3, line 0
  lcd.print("Temp: " + String(temperature) + " F");  // Print a message to the LCD

  lcd.setCursor(0, 1); // set the cursor to column 2, line 1
  lcd.print("Humidity: " + String(humidity) + "%");  // Print a message to the LCD.
  delay(5000);       
}

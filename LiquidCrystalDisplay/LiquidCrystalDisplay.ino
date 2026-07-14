#include <Wire.h>
#include <LiquidCrystal_I2C.h>
/**********************************************************/
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
/*********************************************************/
void setup()
{
  lcd.init();  //initialize the lcd
  Serial.begin(9600);
  lcd.backlight();  //open the backlight
  Serial.println("Initialized");
}
/*********************************************************/
void loop()
{
  lcd.setCursor(5, 0); // set the cursor to column 3, line 0
  lcd.print("Missy");  // Print a message to the LCD

  lcd.setCursor(2, 1); // set the cursor to column 2, line 1
  lcd.print("Happy Birthday!");  // Print a message to the LCD.
  delay(2500);

  lcd.clear();

  lcd.setCursor(5, 0); // set the cursor to column 3, line 0
  lcd.print("Missy");  // Print a message to the LCD

  lcd.setCursor(2, 1); // set the cursor to column 2, line 1
  lcd.print("I Love You!");  // Print a message to the LCD.
  delay(2500);
}
/************************************************************/
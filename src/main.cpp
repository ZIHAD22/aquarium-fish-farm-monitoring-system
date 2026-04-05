#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SimpleDHT.h>

// water level sensor
int waterLevelPin = A0;
int waterLevel;

// turbidity sensor
int turbidity;
int turbidityPin = A1;
int turbidityValue;

int pinDHT11 = 5;
SimpleDHT11 dht11(pinDHT11);

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
}

void loop()
{

  // water level sensor
  waterLevel = analogRead(waterLevelPin);
  // Serial.println(waterLevel);

  // turbidity sensor
  turbidityValue = analogRead(turbidityPin);
  int turbidity = map(turbidityValue, 600, 750, 100, 0);
  turbidity = constrain(turbidity, 0, 100);
  Serial.println(turbidityValue);

  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
  {
    lcd.setCursor(1, 0);
    lcd.print("failed to read");
    delay(1000);
    lcd.clear();
    return;
  }

  lcd.setCursor(0, 0);
  lcd.print((int)temperature);
  lcd.setCursor(2, 0);
  lcd.print("C ");
  // lcd.setCursor(9, 0);
  // lcd.print((int)humidity);
  // lcd.setCursor(11, 0);
  // lcd.print(" H ");

  lcd.setCursor(4, 0);
  lcd.print("Water:");
  lcd.setCursor(10, 0);
  if (waterLevel < 200)
  {
    lcd.print("LOW");
  }
  else if (waterLevel < 500 && waterLevel > 200)
  {
    lcd.print("Medium");
  }
  else
  {
    lcd.print("High");
  }

  lcd.setCursor(0, 1);
  lcd.print("Tur (");
  lcd.print(turbidity);
  lcd.print("): ");

  lcd.setCursor(11, 1);

  if (turbidityValue < 650)
  {
    lcd.print("NO");
  }
  else if (turbidity < 20)
  {
    lcd.print("CLEAR");
  }
  else if (turbidity < 60)
  {
    lcd.print("CLOUDY");
  }
  else
  {
    lcd.print("DIRTY");
  }

  delay(1000);
  lcd.clear();
}
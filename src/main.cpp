#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SimpleDHT.h>

int buttonPin = 9;
int lastButtonState = HIGH;

int menu = 1;

// debounce
unsigned long lastPressTime = 0;
int debounceDelay = 200;

// sensors
int waterLevelPin = A0;
int waterLevel;

int turbidityPin = A1;
int turbidityValue;

int pinDHT11 = 5;
SimpleDHT11 dht11(pinDHT11);

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
}

void loop()
{
  int currentState = digitalRead(buttonPin);

  if (lastButtonState == HIGH && currentState == LOW)
  {
    if (millis() - lastPressTime > debounceDelay)
    {
      menu++;
      if (menu > 4)
        menu = 1;

      Serial.print("Menu: ");
      Serial.println(menu);

      lcd.clear();
      lastPressTime = millis();
    }
  }

  lastButtonState = currentState;

  waterLevel = analogRead(waterLevelPin);

  turbidityValue = analogRead(turbidityPin);
  int turbidity = map(turbidityValue, 600, 750, 100, 0);
  turbidity = constrain(turbidity, 0, 100);

  byte temperature = 0;
  byte humidity = 0;

  dht11.read(&temperature, &humidity, NULL);

  if (menu == 1)
  {
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print((int)temperature);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Hum: ");
    lcd.print((int)humidity);
    lcd.print("%");
  }

  else if (menu == 2)
  {
    lcd.setCursor(0, 0);
    lcd.print("Water Level");

    lcd.setCursor(0, 1);

    if (waterLevel < 200)
      lcd.print("LOW   ");
    else if (waterLevel < 500)
      lcd.print("MEDIUM");
    else
      lcd.print("HIGH  ");
  }

  else if (menu == 3)
  {
    lcd.setCursor(0, 0);
    lcd.print("Turbidity: ");
    lcd.print(turbidity);

    Serial.println(turbidity);

    lcd.setCursor(0, 1);

    if (turbidityValue < 650)
      lcd.print("NO WATER");
    else if (turbidity < 20)
      lcd.print("CLEAR   ");
    else if (turbidity < 60)
      lcd.print("CLOUDY  ");
    else
      lcd.print("DIRTY   ");
  }

  else if (menu == 4)
  {
    lcd.setCursor(0, 0);
    lcd.print("Settings");

    lcd.setCursor(0, 1);
    lcd.print("Coming Soon...");
  }

  delay(200); // small refresh
}
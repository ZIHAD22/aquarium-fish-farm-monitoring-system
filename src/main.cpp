#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SimpleDHT.h>

// relay
int airPumpRelayPin = 11;
int airPumpState = HIGH;

int waterPumpRelayPin = 10;
int waterPumpState = HIGH;

// menu
int menuButtonPin = 8;
int menuButtonState = HIGH;
int menu = 5;

// setting
int settingButtonPin = 9;
int settingButtonSate = HIGH;

// debounce
unsigned long lastMenuPressTime = 0;
unsigned long lastSettingPressTime = 0;
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

  // relay
  pinMode(airPumpRelayPin, OUTPUT);
  pinMode(waterPumpRelayPin, OUTPUT);

  // menu
  pinMode(menuButtonPin, INPUT_PULLUP);

  // setting
  pinMode(settingButtonPin, INPUT_PULLUP);

  // lcd
  lcd.init();
  lcd.backlight();
}

void loop()
{

  if (airPumpState == HIGH && waterPumpState == HIGH)
  {
    digitalWrite(airPumpRelayPin, HIGH);
    digitalWrite(waterPumpRelayPin, HIGH);
  }

  // menu
  int currentMenuState = digitalRead(menuButtonPin);

  if (menuButtonState == HIGH && currentMenuState == LOW)
  {
    if (millis() - lastMenuPressTime > debounceDelay)
    {
      menu++;
      if (menu > 5)
      {
        menu = 1;
      }
      lcd.clear();
      lastMenuPressTime = millis();
    }
  }

  menuButtonState = currentMenuState;

  // setting
  int currentSettingState = digitalRead(settingButtonPin);

  if (settingButtonSate == HIGH && currentSettingState == LOW)
  {
    if (millis() - lastSettingPressTime > debounceDelay)
    {

      if (airPumpState == HIGH && menu == 5)
      {
        airPumpState = LOW;
        digitalWrite(airPumpRelayPin, LOW);
      }
      else
      {
        airPumpState = HIGH;
        digitalWrite(airPumpRelayPin, HIGH);
      }

      Serial.print("button Click");
      Serial.println(currentSettingState);

      lastSettingPressTime = millis();
    }
  }

  settingButtonSate = currentSettingState;

  // water level
  waterLevel = analogRead(waterLevelPin);

  // turbidity
  turbidityValue = analogRead(turbidityPin);
  int turbidity = map(turbidityValue, 600, 750, 100, 0);
  turbidity = constrain(turbidity, 0, 100);

  // dht11
  byte temperature = 0;
  byte humidity = 0;
  dht11.read(&temperature, &humidity, NULL);

  if (menu == 1)
  {
    // dh11
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
    // water level
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
    // turbidity
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
    lcd.print("Settings: ");

    lcd.setCursor(0, 1);
    lcd.print("Click to go next");
  }
  else if (menu == 5)
  {
    // air pump on/of setting

    lcd.setCursor(0, 0);
    lcd.print("Air Pump: ");
    lcd.setCursor(10, 0);
    if (airPumpState == HIGH)
    {
      lcd.print("OFF");
      lcd.setCursor(0, 1);
      lcd.print("Click to on     ");
    }
    else
    {
      lcd.print("ON ");
      lcd.setCursor(0, 1);
      lcd.print("Click to go next");
    }

    lcd.setCursor(0, 1);
  }

  delay(200);
}
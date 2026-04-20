#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SimpleDHT.h>

// auto Mode

boolean autoMode = false;

// sound

int soundPin = 2;

// water level alert
int warningLastActiveTime = 0;
int warningCount = 0;
int warningState = false;
int warningSensor = 0; // 0 -> no warning 1 -> warning from temp , 2 -> water level , 3 -> turbidity

// relay
int airPumpRelayPin = 11;
int airPumpState = HIGH;

int waterPumpRelayPin = 10;
int waterPumpState = HIGH;

// menu
int menuButtonPin = 8;
int menuButtonState = HIGH;
int menu = 3;

// setting
int settingButtonPin = 9;
int settingButtonSate = HIGH;

// debounce
unsigned long lastMenuPressTime = 0;
unsigned long lastSettingPressTime = 0;
unsigned long debounceDelay = 190;

// sensors

// water level
int waterLevelPin = A0;
int waterLevelPowerPin = 3;
int waterLevel;
int waterLevelStatus = HIGH;
int percent;

// turbidity
int turbidityPin = A1;
int turbidityPowerPin = 4;
int turbidityValue;

int turbidityStatus = HIGH;

// dht
int pinDHT11 = 5;
int dhtPowerPin = 6;
byte temperature = 0;
byte humidity = 0;
SimpleDHT11 dht11(pinDHT11);
int dhtStatus = HIGH;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Serial.begin(9600);

  // dht
  pinMode(dhtPowerPin, OUTPUT);

  // water Level
  pinMode(waterLevelPowerPin, OUTPUT);

  // turbidity
  pinMode(turbidityPowerPin, OUTPUT);

  // sound

  pinMode(soundPin, OUTPUT);

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
    delay(10);
  }

  if (dhtStatus == HIGH)
  {
    digitalWrite(dhtPowerPin, HIGH);
    delay(10);
  }

  if (waterLevelStatus == HIGH)
  {
    digitalWrite(waterLevelPowerPin, HIGH);
    delay(10);
  }

  if (turbidityStatus == HIGH)
  {
    digitalWrite(turbidityPowerPin, HIGH);
    delay(10);
  }
  // menu
  int currentMenuState = digitalRead(menuButtonPin);

  if (menuButtonState == HIGH && currentMenuState == LOW)
  {
    if (millis() - lastMenuPressTime > debounceDelay)
    {
      menu++;
      if (menu > 7)
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

      tone(soundPin, 1915);
      delay(100);
      noTone(soundPin);
      if (menu == 1)
      {
        dhtStatus = dhtStatus == HIGH ? LOW : HIGH;
        digitalWrite(dhtPowerPin, dhtStatus);
      }
      else if (menu == 2)
      {
        waterLevelStatus = waterLevelStatus == HIGH ? LOW : HIGH;
        digitalWrite(waterLevelPowerPin, waterLevelStatus);

        if (waterLevelStatus == LOW && warningSensor == 2)
        {
          if (airPumpState == LOW)
          {
            airPumpState = HIGH;
            digitalWrite(airPumpRelayPin, airPumpState);
          }

          if (waterPumpState == LOW)
          {
            waterPumpState = HIGH;
            digitalWrite(waterPumpRelayPin, waterPumpState);
          }

          warningCount = 0;
          warningState = false;
          warningSensor = 0;
        }
      }
      else if (menu == 3)
      {
        turbidityStatus = turbidityStatus == HIGH ? LOW : HIGH;
        digitalWrite(turbidityPowerPin, turbidityStatus);

        if (turbidityStatus == LOW && warningSensor == 3)

        {
          if (airPumpState == LOW)
          {
            airPumpState = HIGH;
            digitalWrite(airPumpRelayPin, airPumpState);
          }

          if (waterPumpState == LOW)
          {
            waterPumpState = HIGH;
            digitalWrite(waterPumpRelayPin, waterPumpState);
          }

          warningCount = 0;
          warningState = false;
          warningSensor = 0;
        }
      }
      else if (airPumpState == HIGH && menu == 5)
      {
        if (!autoMode)
        {
          airPumpState = LOW;
          digitalWrite(airPumpRelayPin, LOW);
        }
      }
      else if (waterPumpState == HIGH && menu == 6)
      {
        if (autoMode != true)
        {
          waterPumpState = LOW;
          digitalWrite(waterPumpRelayPin, LOW);
        }
      }
      else if (waterPumpState == LOW && menu == 6)
      {
        if (autoMode != true)
        {
          waterPumpState = HIGH;
          digitalWrite(waterPumpRelayPin, HIGH);
        }
      }
      else if (autoMode == false && menu == 7)
      {
        autoMode = true;
      }
      else if (autoMode == true && menu == 7)
      {
        autoMode = false;
        airPumpState = HIGH;
        digitalWrite(airPumpRelayPin, HIGH);
        delay(100);

        waterPumpState = HIGH;
        digitalWrite(waterPumpRelayPin, HIGH);
      }
      else
      {
        if (autoMode != true)
        {
          airPumpState = HIGH;
          digitalWrite(airPumpRelayPin, HIGH);
        }
      }

      Serial.print("button Click");
      Serial.println(currentSettingState);

      lastSettingPressTime = millis();
    }
  }

  settingButtonSate = currentSettingState;

  // water level

  if (waterLevelStatus == HIGH)
  {
    waterLevel = analogRead(waterLevelPin);
    percent = map(waterLevel, 0, 500, 0, 2);
    percent = constrain(percent, 0, 2);

    if (waterLevel < 430)
    {
      percent = 0;
    }
    else if (waterLevel < 500)
    {
      percent = 1;
    }
    else
    {
      percent = 2;
    }
  }

  // turbidity
  int turbidity = 0;
  if (turbidityStatus == HIGH)
  {
    turbidityValue = analogRead(turbidityPin);
    turbidity = map(turbidityValue, 600, 750, 100, 0);
    turbidity = constrain(turbidity, 0, 100);
    // Serial.print("R -");
    // Serial.println(turbidityValue);
    // Serial.print("P -");
    // Serial.println(turbidity);
  }

  // dht11

  if (dhtStatus == HIGH)
  {
    dht11.read(&temperature, &humidity, NULL);
  }

  if (menu == 1)
  {
    // dh11
    if (dhtStatus == HIGH)
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
    else
    {
      lcd.setCursor(0, 0);
      lcd.print("Temp & Hum: ");
      lcd.setCursor(0, 1);
      lcd.print("OFF     ");
    }
  }

  else if (menu == 2)
  {
    // water level
    lcd.setCursor(0, 0);
    lcd.print("Water Level: ");

    lcd.setCursor(0, 1);
    if (waterLevelStatus == LOW)
    {
      lcd.print("OFF   ");
    }
    else if (percent == 0 && waterLevelStatus == HIGH)
    {
      lcd.print("LOW   ");
    }
    else if (percent == 1 && waterLevelStatus == HIGH)
      lcd.print("MEDIUM");

    else if (percent == 2 && waterLevelStatus == HIGH)
      lcd.print("HIGH  ");
  }

  else if (menu == 3)
  {
    // turbidity
    lcd.setCursor(0, 0);
    lcd.print("Turbidity: ");
    if (turbidity == 100)
    {
      lcd.print(turbidity);
    }
    else
    {
      lcd.print(turbidity);
      lcd.setCursor(13, 0);
      lcd.print(" ");
    }

    Serial.print("P -");
    Serial.println(turbidity);

    lcd.setCursor(0, 1);

    if (turbidityStatus == LOW)
    {
      lcd.print("OFF     ");
    }
    else if (turbidityValue < 650)
      lcd.print("NO WATER");
    else if (turbidity <= 60)
      lcd.print("CLEAR   ");
    else if (turbidity <= 80)
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
    if (autoMode)
    {
      lcd.print("   ");
      lcd.setCursor(0, 1);
      lcd.print("Setting Disable ");
    }
    else if (airPumpState == HIGH)
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
  else if (menu == 6)
  {
    // air pump on/of setting

    lcd.setCursor(0, 0);
    lcd.print("Water Pump: ");
    lcd.setCursor(12, 0);
    if (autoMode)
    {
      lcd.print("   ");
      lcd.setCursor(0, 1);
      lcd.print("Setting Disable ");
    }
    else if (waterPumpState == HIGH)
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
  else if (menu == 7)
  {
    // auto mode

    lcd.setCursor(0, 0);
    lcd.print("Auto Mode: ");
    lcd.setCursor(11, 0);
    if (autoMode == false)
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
  // global reading
  if (autoMode)
  {

    // water level
    if (waterLevelStatus == HIGH && (warningSensor == 0 || warningSensor == 2))
    {
      delay(100);
      if (percent < 1)
      {

        if (!warningState)
        {
          warningState = true;
          warningCount = 5;
          warningSensor = 2;
        }

        if (warningCount != 0 && warningState == true)
        {
          tone(soundPin, 1915);
          delay(100);
          noTone(soundPin);
          delay(100);
          warningCount--;
        }

        if (warningCount == 0 && waterPumpState == HIGH)
        {
          if (airPumpState == LOW)
          {
            digitalWrite(airPumpRelayPin, HIGH);
            airPumpState = HIGH;
          }
          digitalWrite(waterPumpRelayPin, LOW);
          waterPumpState = LOW;

          warningLastActiveTime = millis();
        }
      }
      else
      {

        if (millis() - warningLastActiveTime >= 12000)
        {

          if (waterPumpState == LOW)
          {
            digitalWrite(waterPumpRelayPin, HIGH);
            waterPumpState = HIGH;
          }

          if (airPumpState == HIGH)
          {
            digitalWrite(airPumpRelayPin, LOW);
            airPumpState = LOW;
          }

          warningState = false;
          warningCount = 0;
          warningSensor = 0;
        }
      }
    }
  }

  // turbidity
  if (turbidityStatus == HIGH && (warningSensor == 0 || warningSensor == 3) && turbidity != 100)
  {
    if (turbidity > 60)

    {
      autoMode = false;
      if (!warningState)
      {
        warningState = true;
        warningCount = 0;
        warningSensor = 3;
      }

      tone(soundPin, 1915);
      delay(100);
      noTone(soundPin);
      delay(100);
    }
    else
    {
      warningState = false;
      warningCount = 0;
      warningSensor = 0;
    }
  }

  // dht safety
  if (dhtStatus == HIGH && (warningSensor == 0 || warningSensor == 1))
  {
    if ((int)temperature > 35)
    {
      autoMode = false;
      if (!warningState)
      {
        warningState = true;
        warningCount = 0;
        warningSensor = 1;
      }

      tone(soundPin, 1915);
      delay(100);
      noTone(soundPin);
      delay(100);
    }
    else
    {
      warningState = false;
      warningCount = 0;
      warningSensor = 0;
    }
  }

  delay(200);
}
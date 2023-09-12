#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define VRX_PIN  A0 // Arduino pin connected to VRX pin
#define VRY_PIN  A1 // Arduino pin connected to VRY pin
#define VRS_PIN  4 // Arduino pin connected to VRS pin

int xValue = 0; // To store value of the X axis
int yValue = 0; // To store value of the Y axis
int sensorVal = 1;
int targetMl = 50;
int targetShots = 1;
int configMode;

LiquidCrystal_I2C lcd(0x27,16,2);

void setup()
{
  lcd.init();                     
  lcd.backlight();// Включаем подсветку дисплея
  lcd.print("Nalivator v1.1");
  //lcd.setCursor(8, 1);
  //lcd.print("LCD 1602");
  pinMode(VRS_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  configMode = 1;
}


void loop()
{

  if (configMode == 1){
    setConfig();
    printTargets();
  } else {
    Serial.print("Making drinks! Vol:");
    Serial.print(targetMl);
    Serial.print(" Count:");
    Serial.println(targetShots);
    
  }


  delay(200);

}

void setConfig(){
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);
  sensorVal = digitalRead(VRS_PIN);
  
  if (xValue > 700){
    if (targetMl < 999){
      targetMl++;
    }
  }
  if (xValue < 300){
    if (targetMl > 1){
    targetMl--;
    }
  }

  if (yValue > 700){
    if (targetShots < 5){
      targetShots++;
    }
  }
  if (yValue < 300){
    if (targetShots > 1){
      targetShots--;
    }
  }

  if (sensorVal == 0){
    Serial.println("Press!!!");
    configMode = 0;
  }
  //Serial.println(sensorVal);

}

void printTargets(){
  lcd.setCursor(0, 1);
  lcd.print("Vol:");
  lcd.print(targetMl);
  if (targetMl<100 && targetMl>10){
    lcd.print(" ");
  }    
  if (targetMl<10){
    lcd.print("  ");
  }
  lcd.setCursor(8, 1);
  lcd.print("Count:");
  lcd.print(targetShots);

}

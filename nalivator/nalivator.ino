#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

//#define VRX_PIN  A0 // Arduino pin connected to VRX pin
//#define VRY_PIN  A1 // Arduino pin connected to VRY pin
//#define VRS_PIN  4 // Arduino pin connected to VRS pin


// Volume encoder
#define vol_CLK 3
#define vol_DT 4
// Shots encoder
#define shots_CLK 5
#define shots_DT 7
// Start button
#define start_btn 9
// Servo
#define servo 10
// Pumping button
#define pumping_btn 11
// Pump
#define pump 8
// 100 ml - 82 sec


// Volume init values
int vol_currentStateCLK;
int vol_lastStateCLK;
String vol_currentDir ="";

// Shots init values
int shots_currentStateCLK;
int shots_lastStateCLK;
String shots_currentDir ="";

// Start button init values
int start_btn_state = 0;
// Pumping button init value
int pumping_btn_state = 0;
// Counter
byte sensorInterrupt = 0;  // 0 = digital pin 2; 1- pin 3
byte sensorPin       = 2;
float flowRate = 0.82;

Servo myservo;

String display_current_message ="";

int xValue = 0; // To store value of the X axis
int yValue = 0; // To store value of the Y axis
int sensorVal = 1;
int targetMl = 50;
int targetShots = 1;
int configMode;
int pumpingMode;
volatile byte pulseCount;
float totalMl = 0;
int angles[] = {-1, 0, 45, 90, 135, 180};
int currentShot = 0;

LiquidCrystal_I2C lcd(0x27,16,2);


int current_ml = 0;
int current_shot = 1;


void setup()
{
  lcd.init();                     
  lcd.backlight();// Включаем подсветку дисплея
  lcd.print("Nalivator v1.1");
  lcd.setCursor(0, 1);
  lcd.print(String("Vol: ")+targetMl+String(" Count: ")+targetShots);
  Serial.begin(9600);
  configMode = 1;
  pumpingMode = 0;

  // Volume encoder setup
  pinMode(vol_CLK,INPUT);
  pinMode(vol_DT,INPUT);
  vol_lastStateCLK = digitalRead(vol_CLK);
  // Shots encoder setup
  pinMode(shots_CLK,INPUT);
  pinMode(shots_DT,INPUT);
  vol_lastStateCLK = digitalRead(shots_CLK);
  // Start button setup
  pinMode(start_btn, INPUT_PULLUP);
  // Pumping button setup
  pinMode(pumping_btn, INPUT_PULLUP);
  // Servo setup
  myservo.attach(servo);
  myservo.write(0);
  // Pump setup
  pinMode(pump, OUTPUT);
  digitalWrite(pump, LOW);
  // Sensor setup
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  pulseCount = 0;

  
}


void loop()
{

  if (configMode == 1){
    //setConfig();
    volEncoder();
    shotsEncoder();
    lcdPrint(String("Vol: ")+targetMl+String(" Count: ")+targetShots);
    start_btn_state = digitalRead(start_btn);
    if (start_btn_state == LOW) {
      Serial.println("Start program: Pressed");
      configMode = 0;
    }
    pumping_btn_state = digitalRead(pumping_btn);
    if (pumping_btn_state == LOW){
      Serial.println("Pumping");
      configMode = 0;
      pumpingMode = 1;
      digitalWrite(pump, HIGH);
      lcdPrint("Pumping...");

    }
    delay(2);
    return;
  }
  
  if (configMode == 0 && pumpingMode == 0){
    //Serial.print("Making drinks! Vol:");
    //Serial.print(targetMl);
    //Serial.print(" Count:");
    //Serial.println(targetShots);
    makeDrinksByTyme();
  }
  
  if (configMode == 0 && pumpingMode == 1){
    pumping();
  }

  delay(200);

}


void makeDrinksByTyme(){
  lcdPrint("Making drinks...");
  for (int shot=1; shot <= targetShots; shot++){
    Serial.println(shot);
    myservo.write(angles[shot]);
    delay(500);
    digitalWrite(pump, HIGH);
    Serial.println((unsigned long)(targetMl*flowRate*1000));
    delay((unsigned long)(targetMl*flowRate*1000));
    digitalWrite(pump, LOW);
    delay(500);
  }
   myservo.write(0);
   configMode = 1;
   currentShot = 0;
   Serial.println("Program is done");
   lcdPrint(String("Vol: ")+targetMl+String(" Count: ")+targetShots);  
}

void makeDrinks(){
  detachInterrupt(sensorInterrupt);
  if (currentShot == 0){
      Serial.println("Starting program");
      lcdPrint(String("Making drinks....."));
      currentShot++;
      myservo.write(angles[currentShot]);
      delay(500);
      digitalWrite(pump, HIGH);    
  }
  totalMl = pulseCount/20.71;
  //totalMl = targetMl;
  String out = String("total pulses: ")+int(pulseCount)+String(" Total ml: ")+int(totalMl);
  Serial.println(out);
  if (totalMl >= targetMl){
    Serial.print("Done for shot: ");
    Serial.println(currentShot);
    digitalWrite(pump, LOW);
    totalMl = 0;
    pulseCount = 0;
    if (targetShots == currentShot){
      myservo.write(0);
      configMode = 1;
      currentShot = 0;
      Serial.println("Program is done");
     lcdPrint(String("Vol: ")+targetMl+String(" Count: ")+targetShots);  
    }else{
      currentShot++;
      myservo.write(angles[currentShot]);
      delay(500);
      digitalWrite(pump, HIGH);
    }
  }
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}

void pumping(){
    Serial.println("Pumping");
    pumping_btn_state = digitalRead(pumping_btn);
    if (pumping_btn_state == HIGH){
      lcdPrint(String("Vol: ")+targetMl+String(" Count: ")+targetShots);  
      Serial.println("Stop pumping");
      configMode = 1;
      pumpingMode = 0;
      digitalWrite(pump, LOW);
    }
    delay(1000);
}

void volEncoder(){
  vol_currentStateCLK = digitalRead(vol_CLK);
  if (vol_currentStateCLK != vol_lastStateCLK  && vol_currentStateCLK == 1){
    if (digitalRead(vol_DT) != vol_currentStateCLK) {
      vol_currentDir ="CCW";
      if (targetMl > 1){
        targetMl--;
      }      
    } else {
      // Encoder is rotating CW so increment
      vol_currentDir ="CW";
      if (targetMl < 999){
        targetMl++;
      }
    }
    Serial.print("Volume direction: ");
    Serial.print(vol_currentDir);
    Serial.print(" | Target volume: ");
    Serial.println(targetMl);
  }
  vol_lastStateCLK = vol_currentStateCLK;  
}

void shotsEncoder(){
  shots_currentStateCLK = digitalRead(shots_CLK);
  if (shots_currentStateCLK != shots_lastStateCLK  && shots_currentStateCLK == 1){
    if (digitalRead(shots_DT) != shots_currentStateCLK) {
      shots_currentDir ="CCW";
      if (targetShots < 5){
        targetShots++;
      }      
    } else {
      // Encoder is rotating CW so increment
      shots_currentDir ="CW";
      if (targetShots > 1){
        targetShots--;
      }
    }
    Serial.print("Shots direction: ");
    Serial.print(shots_currentDir);
    Serial.print(" | Target shots: ");
    Serial.println(targetShots);
  }
  shots_lastStateCLK = shots_currentStateCLK;  
}


/*
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
*/

void printTargets(){
  String display_new_message = String("Vol: ")+targetMl+String(" Count: ")+targetShots;
  if (display_current_message != display_new_message){
    display_current_message = display_new_message;
    lcd.setCursor(0, 1);
    lcd.print(display_current_message);
  }
}

void lcdPrint(String display_new_message){
  if (display_current_message != display_new_message){
    display_current_message = display_new_message;
    int num = 16-display_current_message.length();
    String out_str = display_current_message;
    for (int i=0; i<num; i++){
      out_str = out_str + " ";
    }
    lcd.setCursor(0, 1);
    lcd.print(out_str);
  }  
}

void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}

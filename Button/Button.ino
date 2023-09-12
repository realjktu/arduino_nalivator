/*
  Button

  Turns on and off a light emitting diode(LED) connected to digital pin 13,
  when pressing a pushbutton attached to pin 2.

  The circuit:
  - LED attached from pin 13 to ground through 220 ohm resistor
  - pushbutton attached to pin 2 from +5V
  - 10K resistor attached to pin 2 from ground

  - Note: on most Arduinos there is already an LED on the board
    attached to pin 13.

  created 2005
  by DojoDave <http://www.0j0.org>
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Button
*/

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;     // the number of the pushbutton pin
const int ledPin =  8;      // the number of the LED pin
const int pumpingSwPin = 11;

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int pumpingSwState = 0;

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(pumpingSwPin, INPUT_PULLUP);
  Serial.begin(9600);
  digitalWrite(ledPin, LOW);

}

void loop() {
  //digitalWrite(ledPin, HIGH);

  
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  pumpingSwState = digitalRead(pumpingSwPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    Serial.print("UN-pressed");
    // turn LED on:
    digitalWrite(ledPin, LOW);
  } else {
    Serial.print("Pressed");
    // turn LED off:
    digitalWrite(ledPin, HIGH);
  }
  if (pumpingSwState == HIGH) {
    Serial.println(" pumpingSwState: UN-pressed");
  } else {
    Serial.println(" pumpingSwState: Pressed");
  }
  
  delay(200);
  
}

/*
 */

#include <SoftwareSerial.h>
#include <PLabBTSerial.h>
#include "PLab_PushButton.h"
#include <Servo.h>    // Inkluderer Servo-iblioteket i koden

const int txPin = 2; // Connected to tx on bt unit
const int rxPin = 3; // Connected to rx on bt unit

const int redLedPin =  4;       // the number of the red LED pin
const int greenLedPin =  5;     // the number of the green LED pin

const int redButtonPin = 6;     // the number of the pushbutton pin
const int greenButtonPin = 7;   // the number of the pushbutton pin

const int servoPin = 8;         // the number of the Servo out pin
const int potmeterPin = A0;     // the Potmeter pin
int previousSensorValue = 0;

boolean redLedState = false;
boolean greenLedState = false;

PLabBTSerial btSerial(txPin, rxPin);
PLab_PushButton redButton(redButtonPin);     // Create a PushButton object.
PLab_PushButton greenButton(greenButtonPin); // Create a PushButton object.
Servo myServo;       // Lager et servo-objekt
//...........................................................................
// Always include this method.
// It reads from the BT port and calls BTSerialMessageReceived.
// 
char msg[100];
void updateBTSerial() {
  int availableCount = btSerial.available();
  if (availableCount > 0) {
    btSerial.read(msg, availableCount);
    char *divided = strchr(msg,',');
    int msgValue = 0;
    if (divided != 0) {
       divided[0] = 0; divided++;
       String str(divided);
       msgValue = str.toInt();
    };
    String msgString(msg);
    BTSerialMessageReceived(msgString,msgValue);   
  }
}
//...........................................................................

//...........................................................................
// Always include these two methods .
// They send a message to the BT port, without or with an int value
// 
void BTSerialSendMessage(String msgString) {
  btSerial.println(msgString); 
}

void BTSerialSendMessage(String msgString,int msgValue) {
  btSerial.print(msgString); 
  btSerial.print(",");
  btSerial.println(msgValue);
}
//...........................................................................

void setup()
{  
  btSerial.begin(9600); // Open serial communication to Bluetooth unit
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  // myServo.attach(servoPin); // Knytter servo-objektet til rikitig pinne 
  Serial.begin(9600);   // Open serial communication to Serial Monitor
  Serial.println("Hallo world!");
}

void loop() {
  updateBTSerial();  // Check if we have input on the BT serial port.
  redButton.update();    // Update: Read the red button switch.
  greenButton.update();  // Update: Read the green button switch.
  if (redButton.pressed()) {  // Was the red button pressed?  
     BTSerialSendMessage("#redButtonPressed");
  };   
  if (greenButton.pressed()) {  // Was the green button pressed?  
     BTSerialSendMessage("#greenButtonPressed");
  };
  int sensorValue = analogRead(potmeterPin);
  if (abs(sensorValue-previousSensorValue) > 20) {
      BTSerialSendMessage("#analogValueChanged",sensorValue);
      previousSensorValue = sensorValue;
    }
}

void BTSerialMessageReceived(String msgString,int msgValue) {
  Serial.print("Message:"); Serial.print(msgString); // Debug print
  Serial.print(", Value:"); Serial.println(msgValue);  // Debug print
  if (msgString == "#redButtonPressed") {
    redJavaFxButtonPressed (); return;
  } 
  if (msgString == "#greenButtonPressed") {
    greenJavaFxButtonPressed (); return;
  } 
  if (msgString == "#analogValueChanged") {
    setServoPosition(msgValue); return;
  }
}

void redJavaFxButtonPressed () {
   redLedState = !redLedState;   // Toggle LED state.
   digitalWrite(redLedPin, redLedState);
}

void greenJavaFxButtonPressed () {
   greenLedState = !greenLedState;  // Toggle LED state.
   digitalWrite(greenLedPin, greenLedState);
}

void setServoPosition(int angle) {
   myServo.attach(servoPin); // Knytter servo-objektet til riktig pinne 
   myServo.write(angle); delay(400);
   myServo.detach(); // 
} 


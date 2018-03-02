/*
  An I2C based datalogger - Like the OpenLog but for I2C
  By: Nathan Seidle
  SparkFun Electronics
  Date: February 2nd, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  This example shows how to record various text and variables to Qwiic OpenLog

  To Use:
    Insert a formatted SD card into Qwiic OpenLog
    Attach Qwiic OpenLog to a RedBoard or Uno with a Qwiic cable
    Load this sketch onto the RedBoard
    Open a terminal window to see the Serial.print statements
    Then insert the SD card into a computer view the log file contents
*/

#include <Wire.h>
#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h"
OpenLog myLog; //Create instance

int ledPin = 13; //Status LED connected to digital pin 13

void setup()
{
  pinMode(ledPin, OUTPUT);

  Wire.begin(); //Initialize I2C
  myLog.begin(); //Open connection to OpenLog (no pun intended)

  Serial.begin(9600); //9600bps is used for debug statements
  Serial.println("OpenLog Write File Test");
  
  //Record something to the default log
  myLog.println("This goes to the log file");
  Serial.println("This goes to the terminal");

  float batteryVoltage = 3.4;
  myLog.println("Batt voltage: " + String(batteryVoltage));

  batteryVoltage = batteryVoltage + 0.71;

  myLog.println("Batt voltage: " + String(batteryVoltage));

  Serial.println(F("Done!"));
}

void loop()
{
  //Blink the Status LED because we're done!
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(1000);
}


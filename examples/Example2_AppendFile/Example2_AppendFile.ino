/*
  An I2C based datalogger - Like the OpenLog but for I2C
  By: Nathan Seidle
  SparkFun Electronics
  Date: February 2nd, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  This example shows how to create new files and write to them

  To Use:
    Insert a formatted SD card into Qwiic OpenLog
    Attach Qwiic OpenLog to a RedBoard or Uno with a Qwiic cable
    Load this sketch onto the RedBoard
    Open a terminal window to see the Serial.print statements
    Then insert the SD card into a computer view the log file contents

  Note: We can use the F("") in sketches to move big print statements into program memory to save RAM.
  See more at: https://forum.arduino.cc/index.php?topic=428015.0
*/

#include <Wire.h>
#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h"
OpenLog myLog; //Create instance

int ledPin = 13; //Status LED connected to digital pin 13

const byte OpenLogAddress = 42; //Default Qwiic OpenLog I2C address

void setup()
{
  pinMode(ledPin, OUTPUT);

  Wire.begin();
  myLog.begin(); //Open connection to OpenLog (no pun intended)

  Serial.begin(9600); //9600bps is used for debug statements
  Serial.println("Run OpenLog Append File Test");
  myLog.println("Run OpenLog Append File Test");

  myLog.println("This is recorded to the default log file");
  myLog.append("appendMe.txt");
  myLog.println("This is recorded to appendMe.txt");
  myLog.println("If this file doesn't exist it is created and");
  myLog.println("anything sent to OpenLog will be recorded to this file");

  myLog.println();
  myLog.println(F("Note: We can use the F(\"\") in sketches to move big print statements into program memory to save RAM"));

  Serial.println("Done!");
}

void loop()
{
  //Blink the Status LED because we're done!
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(1000);
}


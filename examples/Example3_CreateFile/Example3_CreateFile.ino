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
*/

#include <Wire.h>
#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h"
OpenLog myLog; //Create instance

int ledPin = LED_BUILTIN; //Status LED connected to digital pin 13

void setup()
{
  pinMode(ledPin, OUTPUT);

  Wire.begin();
  myLog.begin(); //Open connection to OpenLog (no pun intended)

  Serial.begin(9600); //9600bps is used for debug statements
  Serial.println("Run OpenLog New File Test"); //Goes to terminal
  myLog.println("Run OpenLog New File Test"); //Goes to the default LOG#.txt file

  myLog.println("This is recorded to the default log file");
  myLog.create("NewFile.txt");
  myLog.println("This is also recorded to the default log file. But a new file has been created");
  myLog.println("If you want to write to a file use appendFile in example 2");
  myLog.syncFile();

  Serial.println("Done!");
}

void loop()
{
  //Blink the Status LED because we're done!
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  delay(1000);
}


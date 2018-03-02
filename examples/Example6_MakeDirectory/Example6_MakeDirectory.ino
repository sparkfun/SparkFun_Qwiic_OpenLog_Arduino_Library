/*
  An I2C based datalogger - Like the OpenLog but for I2C
  By: Nathan Seidle
  SparkFun Electronics
  Date: February 2nd, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  This example shows how to:
    Create a directory
    Move into that directory called MONDAY
    Create a sub directory within MONDAY called LOGS
    Create and write to a file inside MONDAY
*/

#include <Wire.h>
#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h"
OpenLog myLog; //Create instance

int ledPin = 13; //Status LED connected to digital pin 13

void setup()
{
  pinMode(ledPin, OUTPUT);

  Wire.begin();
  Wire.setClock(400000); //Go super fast

  Serial.begin(9600); //9600bps is used for debug statements
  Serial.println("OpenLog Directory Test");
  myLog.begin(); //Open connection to OpenLog (no pun intended)

  //Record something to the default log
  myLog.println("Time to make a directory");

  myLog.changeDirectory(".."); //Make sure we're in the root directory
  myLog.makeDirectory("MONDAY");
  myLog.changeDirectory("MONDAY"); //Go into this new directory
  myLog.makeDirectory("LOGS"); //Create a sub directory within this new directory
  
  myLog.println(F("But we are still recording to the original log file in root"));

  myLog.append("TheNewMe.txt");
  myLog.println(F("This is in the new file located in the MONDAY directory!"));
  
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


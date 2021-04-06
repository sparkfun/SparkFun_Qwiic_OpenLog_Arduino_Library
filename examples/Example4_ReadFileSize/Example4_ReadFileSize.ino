/*
  An I2C based datalogger - Like the OpenLog but for I2C
  By: Nathan Seidle
  SparkFun Electronics
  Date: February 2nd, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  This example shows how to:
    Record some strings to a default log
    Check the size of a given file name
    If the given file doesn't exist, say so
*/

#include <Wire.h>
#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h"
OpenLog myLog; //Create instance

int ledPin = LED_BUILTIN; //Status LED connected to digital pin 13

void setup()
{
  pinMode(ledPin, OUTPUT);

  Wire.begin();
  Wire.setClock(400000); //Go super fast 400kHz I2C
  myLog.begin(); //Open connection to OpenLog (no pun intended)

  Serial.begin(9600); //9600bps is used for debug statements
  Serial.println();
  Serial.println("OpenLog Read File Size Example");
  
  //Record something to the default log
  myLog.println("OpenLog Read File Size Example");
  myLog.syncFile();

  String myFile = "testme.txt";

  //Get size of file
  long sizeOfFile = myLog.size(myFile);

  if (sizeOfFile == -1)
  {
    Serial.println(F("File not found."));
  }
  else
  {
    Serial.println(F("File found!"));
    Serial.print(F("Size of file: "));
    Serial.println(sizeOfFile);
  }

  Serial.println(F("Done!"));
}

void loop()
{
  //Blink the Status LED because we're done!
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  delay(1000);
}


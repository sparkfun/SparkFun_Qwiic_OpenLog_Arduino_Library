/*
  An I2C based datalogger - Like the OpenLog but for I2C
  By: Nathan Seidle
  SparkFun Electronics
  Date: February 2nd, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  This example shows how to read the files in a given directory.
  You can use wildcards if desired. This is handy for listing a certain type of file such
  as *.LOG or LOG01*.TXT

  If you need to check if a file exists, use the readFileSize command (there's an
  example sketch showing how).
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
  myLog.begin(); //Open connection to OpenLog (no pun intended)

  Serial.begin(9600); //9600bps is used for debug statements
  Serial.println("OpenLog Read Directory Test");

  //Record something to the default log
  myLog.println(F("Let's read the current directory!"));

  Serial.println("List of things in this directory:");

  myLog.searchDirectory("*"); //Give me everything
  //myLog.searchDirectory("*.txt"); //Give me all the txt files in the directory
  //myLog.searchDirectory("*/"); //Get just directories
  //myLog.searchDirectory("*.*"); //Get just files
  //myLog.searchDirectory("LOG*.TXT"); //Give me a list of just the logs
  //myLog.searchDirectory("LOG000*.TXT"); //Get just the logs LOG00000 to LOG00099 if they exist.

  String fileName = myLog.getNextDirectoryItem();
  while (fileName != "") //getNextDirectoryItem() will return "" when we've hit the end of the directory
  {
    Serial.println(fileName);
    fileName = myLog.getNextDirectoryItem();
  }
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


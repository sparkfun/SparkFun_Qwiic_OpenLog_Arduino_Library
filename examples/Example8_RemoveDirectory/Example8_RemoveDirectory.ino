/*
  An I2C based datalogger - Like the OpenLog but for I2C
  By: Nathan Seidle
  SparkFun Electronics
  Date: February 2nd, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  This example shows how to:
    Create a directory
    Create some files there
    Delete a specific file
    Delete *.TXT
    Remove the directory we created
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
  Serial.println("OpenLog Removing a Directory Example");

  Serial.println(F("Press a key to begin"));
  while(Serial.available()) Serial.read(); //Clear buffer
  while(Serial.available() == 0) delay(10); //Wait for user to press a key

  Serial.println(F("Making a directory and empty files"));
  myLog.println(F("Making a directory and empty files")); //Record something to the default log
  myLog.syncFile();

  //Create some directories and files
  myLog.changeDirectory(".."); //Return to root
  myLog.makeDirectory("MONDAY");
  myLog.changeDirectory("MONDAY"); //Move into this sub directory
  myLog.append("TheNewMe.txt");
  myLog.append("Test1.txt");
  myLog.append("Test2.txt");
  myLog.append("Test3.txt");

  myLog.println(F("This is recorded to the last appended file in MONDAY"));
  myLog.syncFile();

  if(myLog.removeFile("Test1.txt") == 1) //Delete a specific file
    Serial.println(F("We have deleted Test1.txt! You can remove the SD card to see."));
  else
    Serial.println(F("Failed to delete Test1.txt"));

  Serial.println(F("Press a key to delete all the files in the MONDAY directory."));
  while(Serial.available()) Serial.read(); //Clear buffer
  while(Serial.available() == 0) delay(10); //Wait for user to press a key

  //If user reinserted the SD card it will cause OpenLog to reset so we need to re-navigate out to MONDAY
  myLog.changeDirectory(".."); //Return to root
  myLog.changeDirectory("MONDAY"); //Move into this sub directory

  long thingsDeleted = myLog.removeFile("*.TXT"); //This is not case sensitive
  Serial.print("Files deleted: ");
  Serial.println(thingsDeleted);

  Serial.println(F("Press a key to remove the MONDAY directory."));
  while(Serial.available()) Serial.read(); //Clear buffer
  while(Serial.available() == 0) delay(10); //Wait for user to press a key

  myLog.changeDirectory(".."); //Return to root

  thingsDeleted = myLog.removeDirectory("MONDAY"); //Remove MONDAY and everything in it
  Serial.print("Things deleted: ");
  Serial.println(thingsDeleted);

  //Note: We cannot recordToOpenLog() here because we have deleted the log we were appending to
  //You must create a new file using appendFile() to continue logging
  
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


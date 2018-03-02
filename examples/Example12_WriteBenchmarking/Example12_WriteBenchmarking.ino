/*
  An I2C based datalogger - Like the OpenLog but for I2C
  By: Nathan Seidle
  SparkFun Electronics
  Date: February 2nd, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  NOTE: This is a benchmarking sketch for Qwiic OpenLog and is not often needed by general users. 

  This sends a large strings of text quickly over I2C.
  We can move 1,100,000 bytes in 53 seconds or 20,754 bytes per second. This is about half the max speed
  possible at 400kHz. Clock stretching allows Qwiic OpenLog to tell master to wait while it completes a write.

  At 115200bps with OpenLog Serial, it logged 11,520 bytes per second with a few buffer overruns
  At 100kHz I2C that's 10,000 bytes per second (10-bits per byte in I2C). Should be ok.
  At 400kHz, that's 40,000 bytes per second. We're going to need clock stretching.

  To use this sketch, attach Qwiic OpenLog to an Arduino.
  After power up, OpenLog will start flashing (indicating it's receiving characters). It takes about 1 minute for
  the sketch to run to completion. This will create a file that looks like this:

  ...
  6:abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-#
  7:abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-#
  8:abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-#
  9:abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-#
  #:abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-#
  1:abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-#
  2:abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-#
  ...

  The reason for creating these character blocks is it allows for a reader to very quickly scan the visible characters and
  indentify any byte errors or glitches along the way. Every 9 lines we print a 10th line that has a leading character
  such as # or !. These allow us to quickly see the different blocks of 10 lines.
*/

#include <Wire.h>
#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h"
OpenLog myLog; //Create instance

int ledPin = 13; //Status LED connected to digital pin 13

void setup()
{
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600); //9600bps is used for debug statements

  Wire.begin();
  Wire.setClock(400000); //Set I2C bus speed to fast 400kHz
  myLog.begin();

  Serial.println();
  Serial.println("Run OpenLog Test");
  Serial.println(F("Press a key to begin"));
  while(Serial.available()) Serial.read(); //Clear buffer
  while(Serial.available() == 0) delay(10); //Wait for user to press a key

  int testAmt = 1;
  //At 100kHz, testAmt of 10 takes about 13 seconds
  //At 400kHz, testAmt of 10 takes about 5 seconds
  //testAmt of 10 will push 110,000 characters/bytes.

  Serial.println("Test using the built-in print functions");
  myLog.println("Run OpenLog Test");
  long startTimeLibrary = millis();

  //Each test is 100 lines. 10 tests is 1000 lines (11,000 characters)
  for (int numofTests = 0 ; numofTests < testAmt ; numofTests++)
  {
    //This loop will print 100 lines of 110 characters each
    for (byte k = 33; k < 43 ; k++)
    {
      //Print one line of 110 characters with marker in the front (markers go from '!' to '*')
      myLog.write(k); //Print the ASCII value directly: ! then " then #, etc
      myLog.println(":abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-#");

      //Then print 9 lines of 110 characters with new line at the end of the line
      for (byte i = 1 ; i < 10 ; i++)
      {
        myLog.print(i, DEC);
        myLog.println(":abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-#");
      }

      if (digitalRead(ledPin) == 0) //Turn the status LED on/off as we go
        digitalWrite(ledPin, HIGH);
      else
        digitalWrite(ledPin, LOW);
    }
  }

  long endTimeLibrary = millis();

  myLog.println("Done!\n");

  unsigned long totalCharacters = (long)testAmt * 100 * 110;
  Serial.print("Characters pushed: ");
  Serial.println(totalCharacters);
  Serial.print("Time taken (ms): ");
  Serial.println(endTimeLibrary - startTimeLibrary);
  Serial.print("Throughput kb/sec: ");
  float throughput = (endTimeLibrary - startTimeLibrary) / 1000.0;
  throughput = totalCharacters / throughput;
  Serial.println(throughput, 3);
  Serial.println();

  //-=-=-=-= Finish Library test

  Serial.println("Test using the direct I2C write function");
  myLog.println("Run OpenLog Test");
  long startTimeDirect = millis();

  //Each test is 100 lines. 10 tests is 1000 lines (11,000 characters)
  for (int numofTests = 0 ; numofTests < testAmt ; numofTests++)
  {
    //This loop will print 100 lines of 110 characters each
    for (byte k = 33; k < 43 ; k++)
    {
      //Print one line of 110 characters with marker in the front (markers go from '!' to '*')
      myLog.directWrite(String((char)k));
      myLog.directWrite(":abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-#\r\n");

      //Then print 9 lines of 110 characters with new line at the end of the line
      for (byte i = 1 ; i < 10 ; i++)
      {
        myLog.directWrite(String(i, DEC));
        myLog.directWrite(":abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-#\r\n");
      }

      if (digitalRead(ledPin) == 0) //Turn the status LED on/off as we go
        digitalWrite(ledPin, HIGH);
      else
        digitalWrite(ledPin, LOW);
    }
  }

  long endTimeDirect = millis();

  myLog.println("Done!\n");

  Serial.print("Characters pushed: ");
  Serial.println(totalCharacters);
  Serial.print("Time taken (ms): ");
  Serial.println(endTimeDirect - startTimeDirect);
  Serial.print("Throughput kb/sec: ");
  throughput = (endTimeDirect - startTimeDirect) / 1000.0;
  throughput = totalCharacters / throughput;
  Serial.println(throughput, 3);
  Serial.println();

}

void loop()
{
  //Blink the Status LED because we're done!
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(1000);
}


/*
  An I2C based datalogger - Like the OpenLog but for I2C
  By: Nathan Seidle
  SparkFun Electronics
  Date: February 2nd, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  This example shows how to change the I2C address of the Qwiic OpenLog

  Note: It's easy to change the I2C address. If you forget what the address is you can use the I2CScan Example to re-discover it
  You can also close the ADR jumper on the board. This will force the I2C address to 0x29 regardless of any other setting or command.

  Valid I2C addresses are 0x08 to 0x77 (inclusive): https://www.totalphase.com/support/articles/200349176-7-bit-8-bit-and-10-bit-I2C-Slave-Addressing
*/

#include <Wire.h>
#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h"
OpenLog myLog; //Create instance

int ledPin = LED_BUILTIN; //Status LED connected to digital pin 13

byte originalAddress = 42; //Original address of the Qwiic OpenLog
byte newAddress = 30; //Address we want to go to

#define STATUS_SD_INIT_GOOD 0
#define STATUS_LAST_COMMAND_SUCCESS 1
#define STATUS_LAST_COMMAND_KNOWN 2
#define STATUS_FILE_OPEN 3
#define STATUS_IN_ROOT_DIRECTORY 4

void setup()
{
  pinMode(ledPin, OUTPUT);

  Wire.begin();
  Wire.setClock(400000); //Go super fast

  Serial.begin(9600); //9600bps is used for debug statements
  Serial.println("OpenLog Address Change Example");

  if (myLog.begin(originalAddress) == false) //Talk to OpenLog using this address (spoiler alert: it's going to change)
  {
    Serial.println("OpenLog failed to respond. Check wiring. Freezing.");
    while (1);
  }
  else
  {
    Serial.println("OpenLog detected!");
  }
  
  Serial.println(F("Press a key to begin"));
  while (Serial.available()) Serial.read(); //Clear buffer
  while (Serial.available() == 0) delay(10); //Wait for user to press a key

  myLog.setI2CAddress(30); //Set the I2C address

  byte status = myLog.getStatus(); //Check the status byte, using new address, to see if command worked (and if we can talk to OpenLog in general)

  Serial.print("Status byte: 0x");
  if (status < 0x10) Serial.print("0");
  Serial.println(status, HEX);

  if (status & 1 << STATUS_LAST_COMMAND_SUCCESS)
  {
    Serial.println("Last command was successful");
    Serial.print("The OpenLog address was successfully changed to ");
    Serial.print(newAddress, DEC);
    Serial.println(".");

    //When we changed the address of OpenLog the library also updated the address
    //So we can immediately start recording
    myLog.println("This is recorded to the last open log file but using the new I2C address");
    //Upon reset our code would need to begin with a myLog.begin(newAddress) to continue communication.
  }
  else
  {
    Serial.println("Last command failed.");
    Serial.println("Pull the SD card and check the config.txt file for the address OpenLog is configured to.");
  }

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


/*
  This is a library written for the Qwiic OpenLog
  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/products/14641

  Written by Nathan Seidle @ SparkFun Electronics, February 2nd, 2018

  Qwiic OpenLog makes it very easy to record data over I2C to a microSD.

  This library handles the initialization of the Qwiic OpenLog and the calculations
  to get the temperatures.

  https://github.com/sparkfun/SparkFun_Qwiic_OpenLog_Arduino_Library

  Development environment specifics:
  Arduino IDE 1.8.3

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h"

// Attempt communication with the device
// Return true if we got a 'Polo' back from Marco
boolean OpenLog::begin(uint8_t deviceAddress, TwoWire &wirePort)
{
  _deviceAddress = deviceAddress; // If provided, store the I2C address from user
  _i2cPort = &wirePort;           // Grab which port the user wants us to use

  // We require caller to begin their I2C port, with the speed of their choice
  // external to the library
  //_i2cPort->begin();

  // Check communication with device
  uint8_t status = getStatus();
  if (status & 1 << STATUS_SD_INIT_GOOD)
  {
    // We are good to go!
    return (true);
  }

  return (false); // SD did not init. Card not present?
}

// Simple begin
boolean OpenLog::begin(int deviceAddress)
{
  return (begin(deviceAddress, Wire));
}

// Get the version number from OpenLog
String OpenLog::getVersion()
{
  sendCommand(registerMap.firmwareMajor, "");
  // Upon completion Qwiic OpenLog will have 2 bytes ready to be read
  _i2cPort->requestFrom(_deviceAddress, (uint8_t)1);

  uint8_t versionMajor = _i2cPort->read();
  sendCommand(registerMap.firmwareMinor, "");
  // Upon completion Qwiic OpenLog will have 2 bytes ready to be read
  _i2cPort->requestFrom(_deviceAddress, (uint8_t)1);

  uint8_t versionMinor = _i2cPort->read();

  return (String(versionMajor) + "." + String(versionMinor));
}

// Get the status byte from OpenLog
// This function assumes we are not in the middle of a read, file size, or other function
// where OpenLog has bytes qued up
//   Bit 0: SD/Init Good
//   Bit 1: Last Command Succeeded
//   Bit 2: Last Command Known
//   Bit 3: File Currently Open
//   Bit 4: In Root Directory
//   Bit 5: 0 - Future Use
//   Bit 6: 0 - Future Use
//   Bit 7: 0 - Future Use
uint8_t OpenLog::getStatus()
{
  sendCommand(registerMap.status, "");
  // Upon completion OpenLog will have a status byte ready to read

  _i2cPort->requestFrom(_deviceAddress, (uint8_t)1);

  return (_i2cPort->read());
}

// Change the I2C address of the OpenLog
// This will be recorded to OpenLog's EEPROM and config.txt file.
boolean OpenLog::setI2CAddress(uint8_t addr)
{
  String temp;
  temp = addr;
  boolean result = sendCommand(registerMap.i2cAddress, temp);

  // Upon completion any new communication must be with this new I2C address

  _deviceAddress = addr; // Change the address internally

  return (result);
}

// Append to a given file. If it doesn't exist it will be created
boolean OpenLog::append(String fileName)
{
  return (sendCommand(registerMap.openFile, fileName));
  // Upon completion any new characters sent to OpenLog will be recorded to this file
}

// Create a given file in the current directory
boolean OpenLog::create(String fileName)
{
  return (sendCommand(registerMap.createFile, fileName));
  // Upon completion a new file is created but OpenLog is still recording to original file
}

// Given a directory name, create it in whatever directory we are currently in
boolean OpenLog::makeDirectory(String directoryName)
{
  return (sendCommand(registerMap.mkDir, directoryName));
  // Upon completion Qwiic OpenLog will respond with its status
  // Qwiic OpenLog will continue logging whatever it next receives to the current open log
}

// Given a directory name, change to that directory
boolean OpenLog::changeDirectory(String directoryName)
{
  return (sendCommand(registerMap.cd, directoryName));
  // Upon completion Qwiic OpenLog will respond with its status
  // Qwiic OpenLog will continue logging whatever it next receives to the current open log
}

// Return the size of a given file. Returns a 4 byte signed long
int32_t OpenLog::size(String fileName)
{
  sendCommand(registerMap.fileSize, fileName);
  // Upon completion Qwiic OpenLog will have 4 bytes ready to be read

  _i2cPort->requestFrom(_deviceAddress, (uint8_t)4);

  int32_t fileSize = 0;
  while (_i2cPort->available())
  {
    uint8_t incoming = _i2cPort->read();
    fileSize <<= 8;
    fileSize |= incoming;
  }

  return (fileSize);
}

// Read the contents of a file, up to the size of the buffer, into a given array, from a given spot
void OpenLog::read(uint8_t *userBuffer, uint16_t bufferSize, String fileName)
{
  uint16_t spotInBuffer = 0;
  uint16_t leftToRead = bufferSize; // Read up to the size of our buffer. We may go past EOF.
  sendCommand(registerMap.readFile, fileName);
  // Upon completion Qwiic OpenLog will respond with the file contents. Master can request up to 32 bytes at a time.
  // Qwiic OpenLog will respond until it reaches the end of file then it will report zeros.

  while (leftToRead > 0)
  {
    uint8_t toGet = I2C_BUFFER_LENGTH; // Request up to a 32 byte block
    if (leftToRead < toGet)
      toGet = leftToRead; // Go smaller if that's all we have left

    _i2cPort->requestFrom(_deviceAddress, toGet);
    while (_i2cPort->available())
      userBuffer[spotInBuffer++] = _i2cPort->read();

    leftToRead -= toGet;
  }
}

// Read the contents of a directory. Wildcards allowed
// Returns true if OpenLog ack'd. Use getNextDirectoryItem() to get the first item.
boolean OpenLog::searchDirectory(String options)
{
  if (sendCommand(registerMap.list, options) == true)
  {
    _searchStarted = true;
    return (true);
    // Upon completion Qwiic OpenLog will have a file name or directory name ready to respond with, terminated with a \0
    // It will continue to respond with a file name or directory until it responds with all 0xFFs (end of list)
  }
  return (false);
}

// Returns the name of the next file or directory folder in the current directory
// Returns "" if it is the end of the list
String OpenLog::getNextDirectoryItem()
{
  if (_searchStarted == false)
    return (""); // We haven't done a search yet

  String itemName = "";
  _i2cPort->requestFrom(_deviceAddress, (uint8_t)I2C_BUFFER_LENGTH);

  uint8_t charsReceived = 0;
  while (_i2cPort->available())
  {
    uint8_t incoming = _i2cPort->read();

    if (incoming == '\0')
      return (itemName); // This is the end of the file name. We don't need to read any more of the 32 bytes
    else if (charsReceived == 0 && incoming == 0xFF)
    {
      _searchStarted = false;
      return (""); // End of the directory listing
    }
    else
      itemName += (char)incoming; // Add this byte to the file name

    charsReceived++;
  }

  // We shouldn't get this far but if we do
  return (itemName);
}

// Remove a file, wildcards supported
// OpenLog will respond with the number of items removed
uint32_t OpenLog::removeFile(String thingToDelete)
{
  return (remove(thingToDelete, false));
}

// Remove a directory, wildcards supported
// OpenLog will respond with 1 when removing a directory
uint32_t OpenLog::removeDirectory(String thingToDelete)
{
  return (remove(thingToDelete, true)); // Delete all files in the directory as well
}

// Remove a file or directory (including everything in that directory)
// OpenLog will respond with the number of items removed
// Returns 1 if only a directory is removed (even if directory had files in it)
uint32_t OpenLog::remove(String thingToDelete, boolean removeEverything)
{
  if (removeEverything == true)
    sendCommand(registerMap.rmrf, thingToDelete); //-rf causes any directory to remove contents as well
  else
    sendCommand(registerMap.rm, thingToDelete); // Just delete a thing

  // Upon completion Qwiic OpenLog will have 4 bytes ready to read, representing the number of files beleted

  _i2cPort->requestFrom(_deviceAddress, (uint8_t)4);

  int32_t filesDeleted = 0;
  while (_i2cPort->available())
  {
    uint8_t incoming = _i2cPort->read();
    filesDeleted <<= 8;
    filesDeleted |= incoming;
  }

  return (filesDeleted); // Return the number of files removed

  // Qwiic OpenLog will continue logging whatever it next receives to the current open log
}

// Send a command to the unit with options (such as "append myfile.txt" or "read myfile.txt 10")
boolean OpenLog::sendCommand(uint8_t registerNumber, String option1)
{
  _i2cPort->beginTransmission(_deviceAddress);
  _i2cPort->write(registerNumber);
  if (option1.length() > 0)
  {
    //_i2cPort->print(" "); //Include space
    _i2cPort->print(option1);
  }

  if (_i2cPort->endTransmission() != 0)
  {
#ifndef __SAMD21G18A__
    return (false);
#endif
  }

  return (true);
  // Upon completion any new characters sent to OpenLog will be recorded to this file
}

// Write a single character to Qwiic OpenLog
size_t OpenLog::write(uint8_t character)
{
  _i2cPort->beginTransmission(_deviceAddress);
  _i2cPort->write(registerMap.writeFile); // Send the byte that corresponds to writing a file
  _i2cPort->write(character);
  if (_i2cPort->endTransmission() != 0)
  {
#ifndef __SAMD21G18A__
    return (false);
#endif
  }
  return (1);
}

int OpenLog::writeString(String string)
{
  _i2cPort->beginTransmission(_deviceAddress);
  _i2cPort->write(registerMap.writeFile);

  // remember, the rx buffer on the i2c openlog is 32 bytes
  // and the register address takes up 1 byte so we can only
  // send 31 data bytes at a time
  if (string.length() > 31)
  {
    return -1;
  }
  if (string.length() > 0)
  {
    //_i2cPort->print(" "); //Include space
    _i2cPort->print(string);
  }

  if (_i2cPort->endTransmission() != 0)
  {
#ifndef __SAMD21G18A__
    return (false);
#endif
  }

  return (1);
}

bool OpenLog::syncFile()
{
  _i2cPort->beginTransmission(_deviceAddress);
  _i2cPort->write(registerMap.syncFile);

  if (_i2cPort->endTransmission() != 0)
  {
#ifndef __SAMD21G18A__
    return (false);
#endif
  }

  return (1);
}
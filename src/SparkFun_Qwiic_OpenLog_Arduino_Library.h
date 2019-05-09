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

#pragma once

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

//The default I2C address for the Qwiic OpenLog is 0x2A (42). 0x29 is also possible.
#define QOL_DEFAULT_ADDRESS (uint8_t)42

//Bits found in the getStatus() byte
#define STATUS_SD_INIT_GOOD 0
#define STATUS_LAST_COMMAND_SUCCESS 1
#define STATUS_LAST_COMMAND_KNOWN 2
#define STATUS_FILE_OPEN 3
#define STATUS_IN_ROOT_DIRECTORY 4

//Platform specific configurations

//Define the size of the I2C buffer based on the platform the user has
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)

//I2C_BUFFER_LENGTH is defined in Wire.H
#define I2C_BUFFER_LENGTH BUFFER_LENGTH

#elif defined(__SAMD21G18A__)

//SAMD21 uses RingBuffer.h
#define I2C_BUFFER_LENGTH SERIAL_BUFFER_SIZE

#elif __MK20DX256__
//Teensy

#elif ARDUINO_ARCH_ESP32
//ESP32 based platforms

#else

//The catch-all default is 32
#define I2C_BUFFER_LENGTH 32

#endif
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


class OpenLog : public Print {

  public:
  
	struct memoryMap {
		byte id;
		byte status;
		byte firmwareMajor;
		byte firmwareMinor;
		byte i2cAddress;
		byte logInit;
		byte createFile;
		byte mkDir;
		byte cd;
		byte readFile;
		byte startPosition;
		byte openFile;
		byte writeFile;
		byte fileSize;
		byte list;
		byte rm;
		byte rmrf;
    byte syncFile;
	};

	const memoryMap registerMap = {
		.id = 0x00,
		.status = 0x01,
		.firmwareMajor = 0x02,
		.firmwareMinor = 0x03,
		.i2cAddress = 0x1E,
		.logInit = 0x05,
		.createFile = 0x06,
		.mkDir = 0x07,
		.cd = 0x08,
		.readFile = 0x09,
		.startPosition = 0x0A,
		.openFile = 0x0B,
		.writeFile = 0x0C,
		.fileSize = 0x0D,
		.list = 0x0E,
		.rm = 0x0F,
		.rmrf = 0x10,
    .syncFile = 0x11,
	};
    //These functions override the built-in print functions so that when the user does an 
    //myLogger.println("send this"); it gets chopped up and sent over I2C instead of Serial
    virtual size_t write(uint8_t character);
    int writeString(String string);
    bool syncFile(void);

    //By default use the default I2C addres, and use Wire port
    boolean begin(uint8_t deviceAddress = QOL_DEFAULT_ADDRESS, TwoWire &wirePort = Wire);
    boolean begin(int deviceAddress); 

    String getVersion(); //Returns a string that is the current firmware version
    uint8_t getStatus(); //Returns various status bits

    boolean setI2CAddress(uint8_t addr); //Set the I2C address we read and write to
    boolean append(String fileName); //Open and append to a file
    boolean create(String fileName); //Create a file but don't open it for writing
    boolean makeDirectory(String directoryName); //Create the given directory
    boolean changeDirectory(String directoryName); //Change to the given directory
    int32_t size(String fileName); //Given a file name, read the size of the file

    void read(uint8_t* userBuffer, uint16_t bufferSize, String fileName); //Read the contents of a file into the provided buffer

    boolean searchDirectory(String options); //Search the current directory for a given wildcard
    String getNextDirectoryItem(); //Return the next file or directory from the search

    uint32_t removeFile(String thingToDelete); //Remove file
    uint32_t removeDirectory(String thingToDelete); //Remove a directory including the contents of the directory
    uint32_t remove(String thingToDelete, boolean removeEverthing); //Remove file or directory including the contents of the directory

    //These are the core functions that send a command to OpenLog
    boolean sendCommand(uint8_t registerNumber, String option1);

  private:

    //Variables
    TwoWire *_i2cPort; //The generic connection to user's chosen I2C hardware
    uint8_t _deviceAddress; //Keeps track of I2C address. setI2CAddress changes this.
    uint8_t _escapeCharacter = 26; //The character that needs to be sent to QOL to get it into command mode
    uint8_t _escapeCharacterCount = 3; //Number of escape characters to get QOL into command mode

    boolean _searchStarted = false; //Goes true when user does a search. Goes false when we reach end of directory.
};
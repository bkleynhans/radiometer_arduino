/*
    The Adafruit Data Logging Shield is an Arduino-based add-on card (shield)
    that is used as an interface to a SD-Card for data storage purposes.  This
    library is intended to ease the use of the shield

    Program Description : A OOP-based (class-based) library for interacting with the
        adafruit datalogging shield.  Simply create an instance of the library object
        and use it to access the SD-card.
    Created By : Benjamin Kleynhans
    Creation Date : June 4, 2020
    Authors : Benjamin Kleynhans
    
    NOTE: Many functions and user feedback have been commented out due to 
            space restrictions on the Arduino Uno/Adafruit Metro the code
            is being used on.

    Last Modified By : Benjamin Kleynhans
    Last Modified Date : June 24, 2020
    Filename : AdafruitDataloggingShield.cpp
*/

#include "AdafruitDataloggingShield.h"

AdafruitDataloggingShield::AdafruitDataloggingShield(char* pSiteName, HardwareSerial* pSerial, int* baud, char* pHeadingString)
{
    this->pSerial = pSerial;
    this->pBaud = baud;
    this->pSiteName = pSiteName;
    this->pHeadingString = pHeadingString;
    
    this->openSerial();
    this->initializeSdCard();
}

AdafruitDataloggingShield::AdafruitDataloggingShield(char* pSiteName, HardwareSerial* pSerial, int* baud)
{
    this->pSerial = pSerial;
    this->pBaud = baud;
    this->pSiteName = pSiteName;
    
    this->openSerial();
    this->initializeSdCard();
}

AdafruitDataloggingShield::~AdafruitDataloggingShield()
{
    delete this->pOpenedFile;
}

// Set or change a heading after construction
void AdafruitDataloggingShield::setHeading(char* pHeadingString)
{
    this->pHeadingString = pHeadingString;
}

// Get the current heading string
char* AdafruitDataloggingShield::getHeading()
{
    return this->pHeadingString;
}

// Set or change a heading after construction
void AdafruitDataloggingShield::setSiteName(char* pSiteName)
{
    this->pSiteName = pSiteName;
}

// Get the current site name
char* AdafruitDataloggingShield::getSiteName()
{
    return this->pSiteName;
}

// Get the chip used to access the SD card
byte AdafruitDataloggingShield::getChipSelect()
{
    return this->chipSelect;
}

// Return a pointer to the opened file
File* AdafruitDataloggingShield::getOpenedFile()
{
    return this->pOpenedFile;
}

//Display a directory of the SD-Card
void AdafruitDataloggingShield::dir()
{
    if (this->sdCardInitialized) {
        
        this->pSerial->print(F("\n      Files found on the card (name, date and size in bytes): \n"));
        this->sdRoot.openRoot(this->sdVolume);

        // list all files in the card with date and size
        this->sdRoot.ls(LS_R | LS_DATE | LS_SIZE);
        
    } else {
        
        this->pSerial->print(F("\n      !!! The SD-card has not been initialized. !!!\n"));
    }
}

// Open the serial connection
void AdafruitDataloggingShield::openSerial()
{
    if (!this->pSerial) {
        
        this->pSerial->begin(*this->pBaud);

        while (!this->pSerial) {
            ; // Wait for the serial port to connect.
        }
    }
}

// Initialize the SD-card
void AdafruitDataloggingShield::initializeSdCard()
{
    this->pSerial->print(F("\n      Initializing SD card...\n"));
    
    if (this->sdCard.init(SPI_HALF_SPEED, this->chipSelect)) {

        this->pSerial->print(F("\n      Wiring is correct and an SD-Card is present."));

        if (this->sdVolume.init(this->sdCard)) {

            this->pSerial->print(F("\n      A FAT16/FAT32 partition is present. \n"));

            this->sdCardInitialized = true;

        } else {
            
            this->pSerial->print(F("\n      !!! No FAT16/FAT32 partition. !!!\n"));
        }

    } else {

        this->pSerial->print(F("\n --> Failed on startup."));
    }
}

// Public method for writing data to a file
bool AdafruitDataloggingShield::write(char* filename, char* data)
{    
    if ((sizeof(*filename) / sizeof(char)) > 8) {
        
        this->pSerial->print(F("\n      The specified filename "));
        this->pSerial->print(filename);
        this->pSerial->print(F(" is longer than 8 characters, which is not supported.\n"));
        
        return false;
    } else {        
        if (!SD.begin(this->chipSelect)) {
            
            this->pSerial->print(F("\n      !!! Failed on write. !!!"));
            
        } else {
            if (!this->fileExists(filename)) {            
                this->createFile(filename);
            }
            
            if (this->openFile('w', filename)) {                
                this->pOpenedFile->println(data);
                this->closeFile();
            }
        }
        
        SD.end();
    
        return true;
    }
}

// Open the file with an access type.
// r - read
// w - write
bool AdafruitDataloggingShield::openFile(char accessType, char* filename)
{
    switch (accessType)
    {
        case 'r':            
            this->pOpenedFile = &(SD.open(filename, O_READ));
            
            return true;
        case 'w':
            this->pOpenedFile = &(SD.open(filename, FILE_WRITE));
        
            return true;
        default:
            return false;
    }
}

// Create an empty file
void AdafruitDataloggingShield::createFile(char* filename)
{    
    this->pSerial->print(F("\n      File does not exist, creating "));
    this->pSerial->print(filename);
    this->pSerial->println("\n");
    delay(5000);

    this->pOpenedFile = &(SD.open(filename, FILE_WRITE));
    this->pOpenedFile->println(this->pHeadingString);
    this->pOpenedFile->close();
}

// Test if the file exists
bool AdafruitDataloggingShield::fileExists(char* filename)
{
    if (SD.exists(filename)) {
        return true;
    } else {
        return false;
    }
}

// Close the open file
void AdafruitDataloggingShield::closeFile()
{
    this->pOpenedFile->close();
}

// Ask if the clock has been set since startup
bool AdafruitDataloggingShield::clockSet()
{
    return this->initialClockSet;
}

// Sets the clock to the specified time
void AdafruitDataloggingShield::setClock(int yyyy, int mo, int dd, int hh, int mm, int ss)
{
    // This line sets the realtime clock
    rtc.adjust(DateTime(yyyy, mo, dd, hh, mm, ss));
    
    if (!this->clockSet()) {
        this->initialClockSet = true;
    }
}

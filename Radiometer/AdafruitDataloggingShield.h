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

    Last Modified By : Benjamin Kleynhans
    Last Modified Date : June 24, 2020
    Filename : AdafruitDataloggingShield.h
*/

#ifndef AdafruitDataloggingShield_h
#define AdafruitDataloggingShield_h

#include <Arduino.h>
#include <RTClib.h>
#include <SPI.h>
#include <SD.h>

class AdafruitDataloggingShield
{
public:
    AdafruitDataloggingShield(char* pSiteName, HardwareSerial* pSerial, int* baud, char* pHeadingString);
    AdafruitDataloggingShield(char* pSiteName, HardwareSerial* pSerial, int* baud);
    ~AdafruitDataloggingShield();

    //// Data Management
    //// Methods
    bool write(char* filename, char* data);
    void setHeading(char* pHeadingString);
    char* getHeading();
    void setSiteName(char* pSiteName);
    char* getSiteName();
    File* getOpenedFile();
    
    //// Hardware Management
    // Retrieve the chip used for SD card management
    byte getChipSelect();
    
    // Realtime clock object
    const RTC_PCF8523 rtc;
    
    // Display a directory of the sd-card contents
    void dir();
    
    // Return true or false based on whether the clock has been set since device startup
    bool clockSet();
    
    // Set the hardware clock
    void setClock(int yyyy, int mo, int dd, int hh, int mm, int ss);
    
    // Data management
    bool openFile(char accessType, char* filename);
    void createFile(char* filename);
    bool fileExists(char* filename);
    void closeFile();

private:
    //// VARIABLES
    // Define a hardware serial for constructor parameter
    HardwareSerial* pSerial = nullptr;
    
    // Define the updated chip select pin for the Adafruit Datalogging Shield
    const byte chipSelect = 2;
    
    // Define the baud rate from constructor
    int* pBaud = nullptr;
    
    // Pointer to the site and heading strings
    char* pSiteName = nullptr;
    char* pHeadingString = nullptr;
    
    // If debugging is true, display serial output
    bool debug = true;
    
    // Has the realtime clock been set since device startup?
    bool initialClockSet = false;
    
    // Keep track of initialized physical components
    bool sdCardInitialized = false;
    bool rtcInitialized = false;

    // Variable declarations based on SD utility library functions
    Sd2Card sdCard;
    SdVolume sdVolume;
    SdFile sdRoot;
    
    File* pOpenedFile = nullptr;    

    //// METHODS
    // Hardware management
    void openSerial();
    void initializeSdCard();
    
    //~ // Data management
    //~ bool openFile(char accessType, char* filename);
    //~ void createFile(char* filename);
    //~ bool fileExists(char* filename);
    //~ void closeFile();
};
#endif // AdafruitDataloggingShield_h

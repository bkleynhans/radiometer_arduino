/*
    Enter project description here

    Program Description : This is the core module developed by and for
        Rochester Institute of Technology for a field-deployable radiometer
        unit with data sampling and upload capabilities.
    Created By : Benjamin Kleynhans
    Creation Date : June 4, 2020
    Authors : Benjamin Kleynhans

    Last Modified By : Benjamin Kleynhans
    Last Modified Date : July 1, 2020
    Filename : Radiometer.ino
*/

// Import modules
#include <stdio.h>
#include <string.h>

#include "ExtendedADCShield.h"
#include "AdafruitDataloggingShield.h"
#include "Botletics_LTE_GPS_Shield.h"

//// ---> MEMORY CHECKING
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__
//// ---> MEMORY CHECKING

// Define constants
// Provide the site name
const char* pSITE_NAME = "Henrietta 1";

// The site code is a unique, 2-digit code
const char* pSITE_CODE = "H1";

// Thermobile internal temperature pins
const char* pins[8] = {"","A0", "", "A1", "", "", "", "A2"};

// Array containing data headings
const char* pHEADING_STRING = "ch1,ch2,tp1,ch3,ch4,tp2,ch5,ch6,ch7,ch8,tp3,Year,Month,Day,Hour,Minutes,Seconds";

// Define FTP server connection properties
const char* serverIP = "";
const byte serverPort = 21;
const char* username = "anonymous";
const char* password = "";

// Create instances of all required componenets
const ExtendedADCShield* pExtendedADCShield = nullptr;
const AdafruitDataloggingShield* pDataloggingShield = nullptr;
const Botletics_LTE_GPS_Shield* pBotletics_LTEGPS = nullptr;

// Extended ADC shield interface pins
const byte CONVST = 5;
const byte RD = 4;
const byte BUSY = 3;
const byte NUMBER_BITS = 16;

// Botletics LTE/GPS shield interface pins
const uint8_t FONA_PWRKEY = 6;
const uint8_t FONA_RST = 7;
const uint8_t FONA_RX = 10;
const uint8_t FONA_TX = 11;

// Define variables
// Keeps track of whether the device was just switched on
bool initialStartup = true;

// Define sizes of variables used for collection
const int collectionSize = 7;
const int titleSize = 20;
const int positionSize = 66;
const int stringSize = 100;

// Define the variable used for data collection
long chX;
float tempVal;
char chValue[collectionSize];
char titleString[titleSize];
char positionString[positionSize];
char collectionString[stringSize];

// Define the sample timer variables
unsigned long currentTime;
unsigned long previousTime;

// Define the baud rate
const int baud = 9600;

// Current day, used for data upload
uint8_t currentDay;

// Current minute used for testing
uint8_t currentMinute;

// Define the filename used to store the data
char filename[13];

// Define whether data needs to be uploaded during this cycle
bool dataUpload = false;

// the setup function runs once when you press reset or power the board
void setup()
{
    Serial.begin(baud);
    
    // Perform Confguration and Setup functions for each shield
    //// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //// !!!! BOARDS NEED TO BE INITIALIZED IN THE ORDER THEY ARE STACKED !!!!
    //// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    setUpAdcShield();
    setUpDataloggingShield();
    setUpBotleticsShield();
}

// the loop function runs over and over again until power down or reset
void loop()
{    
    // Check if this is the initial boot of the device, and pre-set some values
    if (!pDataloggingShield->clockSet() || initialStartup) {
        
        Serial.println(F("\n --- Running startup configuration checks ---"));
        
        // Turn on the Botletics LTE/GPS shield
        pBotletics_LTEGPS->powerOn();
        
        // If we need to upload a data file, upload it
        if (dataUpload)
        {
            uploadData();
        }
                
        // Update the clock from the Cellular service
        setClock();
        
        // Create the filename for data to append to
        buildFilename();
        
        // Set the headings for the new file
        buildHeading();
    
        initialStartup = false;
                
        // Turn off the Botletics LTE/GPS shield
        pBotletics_LTEGPS->powerOff();
    }
    
    if (currentDay != pDataloggingShield->rtc.now().day()) {
    //~ if (pDataloggingShield->rtc.now().minute() >= currentMinute + 11) {
        
        Serial.println(F("--> Day has changed, updating configuration and creating new file"));
        
        // Update the current day        
        currentDay = pDataloggingShield->rtc.now().day();
        //~ currentMinute = pDataloggingShield->rtc.now().minute();
        
        // Upload the data files to the online storage service
        dataUpload = true;
        
        // Set initial startup to true, which will force a re-sync of the clock and create a new file for the new day.
        initialStartup = true;
    }
    
    // Used to check for 1 second time intervals
    currentTime = millis();
    
    // If a second has passed, read the sensor data
    if (currentTime >= previousTime + 1000 && initialStartup == false) {
        Serial.print(F("Free Memory : "));
        Serial.print(freeMemory());
        Serial.print(F("  <|>  "));
        
        readExtendedADCShield();
        pDataloggingShield->write(filename, collectionString);
        
        previousTime = currentTime;
    }
}

void setUpAdcShield()
{
    Serial.print(F("\n --- Initializing Mayhew ---"));
    delay(100);
    
    // Create an ADC Shield instance and set up first channel for sampling
    pExtendedADCShield = new ExtendedADCShield(CONVST, RD, BUSY, NUMBER_BITS);
    pExtendedADCShield->analogReadConfigNext(0, SINGLE_ENDED, UNIPOLAR, RANGE5V);
}

void setUpDataloggingShield()
{
    Serial.print(F("\n --- Initializing Datalogger ---"));
    delay(100);
    
    // buildHeading();
    
    // Create a Datalogging Shield instance for writing to SD card and RTC
    pDataloggingShield = new AdafruitDataloggingShield(pSITE_NAME, &Serial, &baud);
    
    // Test and activate the realtime clock
    if (!pDataloggingShield->rtc.begin()) {
        Serial.println(F("\n !!! Couldn't find RTC !!! \n"));
        Serial.flush();
        abort();
    }
    
    // Assign the starting time for sample interval measurement
    currentTime = millis();
    previousTime = currentTime;
    
    // Save the current day
    currentDay = pDataloggingShield->rtc.now().day();
    //~ currentMinute = pDataloggingShield->rtc.now().minute();
}

void setUpBotleticsShield()
{
    Serial.print(F("\n --- Initializing Botletics LTE/GPS ---"));
    delay(100);
    
    // Create a Software Serial instance for the Botletics FONA LTE/GPS
    pBotletics_LTEGPS = new Botletics_LTE_GPS_Shield(&Serial, &baud, &FONA_PWRKEY, &FONA_RST, &FONA_TX, &FONA_RX);
}

// Update the RTC on the datalogging shield from GPS UTC time
void setClock()
{
    Serial.println(F("\n --- Setting Clock ---"));
    delay(100);
    
    //                             yyyy, mo, dd, hh, mm, ss
    // pDataloggingShield->setClock(2020, 06, 11, 11, 07, 00);
    pDataloggingShield->setClock(
        pBotletics_LTEGPS->getYear(),                // Year
        pBotletics_LTEGPS->getMonth(),               // Month
        pBotletics_LTEGPS->getDay(),                 // Day
        pBotletics_LTEGPS->getHours(),               // Hour
        pBotletics_LTEGPS->getMinutes(),             // Minute
        pBotletics_LTEGPS->getSeconds()              // Second
    );
}

// Upload the data file to the cloud
void uploadData()
{
    return 0;
}

// Read the analog inputs from the Mayhew Extended ADC Shield
void readExtendedADCShield()
{
    // Clean the collection string variable
    memset(collectionString, 0, sizeof(collectionString));
                
    for (byte i = 0; i < 8; i++) {
        // Clean the channel variable
        chX = 0;
        tempVal = 0;
        
        /* The measurement of the Mayhew works by setting up one channel, and reading another.
            
            I'll use the following commands to explain:
            
            1 --> analogReadConfigNext(1, SINGLE_ENDED, UNIPOLAR, RANGE5V);
            2 --> analogReadConfigNext(0, SINGLE_ENDED, UNIPOLAR, RANGE5V);
            
            Command 1 sets up channel 1 for reading, and reads the value of channel 0
            Command 2 sets up channel 0 for reading, and reads the value of channel 7
        */ 
        
        // Multiply value by 100 000 to convert from float to long
        if (i != collectionSize) {
            tempVal = pExtendedADCShield->analogReadConfigNext((i + 1), SINGLE_ENDED, UNIPOLAR, RANGE5V);
        } else {
            tempVal = pExtendedADCShield->analogReadConfigNext(0, SINGLE_ENDED, UNIPOLAR, RANGE5V);
        }
        
        chX = tempVal * 100000.0f;
        
        // Convert int/char to string
        dtostrf(chX, 6, 0, chValue);
        
        if (i == 0) {
            // Copy the value into the collection string
            strcpy(collectionString, chValue);
        } else {
            // Append the value to the current collection string
            strcat(collectionString, chValue);
        }        
        
        strcat(collectionString, ",");
        
        if (i == 1 || i == 3 || i == 7) {
            // Read the value from the associated analog pin
            chX = analogRead(pins[i]);
            
            // Convert int/char to string
            dtostrf(chX, 6, 0, chValue);
            
            // Append the value to the current collection string
            strcat(collectionString, chValue);
            
            strcat(collectionString, ",");
        }
    }
    
    addDate();
    
    Serial.println(collectionString);
}

// Build the filename to be used for data upload
void buildFilename()
{
    Serial.println(F("\n --- Building Filename ---"));
    delay(100);
    
    // Clean the filename variable
    memset(filename, 0, sizeof(filename));
    
    snprintf(filename + strlen(filename),
        13 - strlen(filename),
        "%s%02d%02d%02d.csv",
        pSITE_CODE,
        (pDataloggingShield->rtc.now().year() % 100),
        pDataloggingShield->rtc.now().month(),
        pDataloggingShield->rtc.now().day()
        //~ pDataloggingShield->rtc.now().minute()
    );
}

// Build the heading to be used in each new file
void buildHeading()
{    
    Serial.println(F("\n --- Building Heading ---\n"));
    delay(100);
    
    buildTitleString();    
    buildPositionString();
    
    Serial.println(titleString);
    Serial.println(positionString);
    Serial.println(pHEADING_STRING);
    
    pDataloggingShield->write(filename, pHEADING_STRING);
}

// Build the titleString
void buildTitleString()
{
    // Clean the current titleString
    memset(titleString, 0, sizeof(titleString));
    
    snprintf(
        titleString + strlen(titleString),
        50 - strlen(titleString),
        "Site Name: %s",
        pSITE_NAME
    );
    
    pDataloggingShield->write(filename, titleString);    
}

// Build the Geo coordinates heading
void buildPositionString()
{    
    // Clean the current positionString
    memset(positionString, 0, sizeof(positionString));
    
    snprintf(
        positionString,
        positionSize - strlen(positionString),
        "Latitude: %s, Longitude: %s, Altitude %s",
        pBotletics_LTEGPS->getLatitudeStr(),
        pBotletics_LTEGPS->getLongitudeStr(),
        pBotletics_LTEGPS->getAltitudeStr()
    );
    
    pDataloggingShield->write(filename, positionString);
}

// Add the date to the measurement data
void addDate()
{    
    snprintf(
        collectionString + strlen(collectionString),
        stringSize - strlen(collectionString),
        "%d,%d,%d,%d,%d,%d",
        pDataloggingShield->rtc.now().year(),
        pDataloggingShield->rtc.now().month(),
        pDataloggingShield->rtc.now().day(),
        pDataloggingShield->rtc.now().hour(),
        pDataloggingShield->rtc.now().minute(),
        pDataloggingShield->rtc.now().second()
    );
}

/// MEMORY CHECKING
int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}
/// MEMORY CHECKING

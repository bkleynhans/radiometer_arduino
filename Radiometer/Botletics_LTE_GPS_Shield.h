/*
    

    Program Description : 
    
    Created By : Benjamin Kleynhans
    Creation Date : June 16, 2020
    Authors : Benjamin Kleynhans

    Last Modified By : Benjamin Kleynhans
    Last Modified Date : June 26, 2020
    Filename : Botletics_LTE_GPS_Shield.h
*/

#ifndef Botletics_LTE_GPS_Shield_h
#define Botletics_LTE_GPS_Shield_h

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_FONA.h>

class Botletics_LTE_GPS_Shield
{
public:
    Botletics_LTE_GPS_Shield(HardwareSerial* pSerial, int* pBaud, uint8_t* pPWRKEY, uint8_t* pRST, uint8_t* pTX, uint8_t* pRX);
    ~Botletics_LTE_GPS_Shield();
    
    
    //// Debugging
    bool debug = false;

    //// Data Management
    //// Getters and Setters
    // Geo location
    // We don't set these values as they are populated via memory reference
    float getLatitude();
    char* getLatitudeStr();
    float getLongitude();
    char* getLongitudeStr();
    float getSpeedKph();
    char* getSpeedKphStr();
    float getHeading();
    char* getHeadingStr();
    float getAltitude();
    char* getAltitudeStr();
    
    // Date and Time
    void setYear(uint16_t year);
    uint16_t getYear();
    void setMonth(uint8_t month);
    uint8_t getMonth();
    void setDay(uint8_t day);
    uint8_t getDay();
    void setHours(uint8_t hours);
    uint8_t getHours();
    void setMinutes(uint8_t minutes);
    uint8_t getMinutes();
    void setSeconds(float seconds);
    float getSeconds();
    
    //// Variables    
    //// Hardware Management
    // Methods
    void powerOn();
    void powerOff();
    
    // Variables

private:
    //// VARIABLES
    // Define the baud rate from constructor
    int* pBaud = nullptr;
    
    // Define a hardware serial for constructor parameter
    HardwareSerial* pSerial = nullptr;
    
    // Define the communication variables
    uint8_t* pPWRKEY = nullptr;
    uint8_t* pRST = nullptr;
    uint8_t* pTX = nullptr;
    uint8_t* pRX = nullptr;
    
    // Define Software Serial
    // Conversions from example:
    //      fonaSS      --> *this->pFonaSS
    //      fonaSerial  --> this->pFonaSerial
    //      fona        --> fona
    
    SoftwareSerial* pFonaSS = nullptr;
    Adafruit_FONA_LTE fona = Adafruit_FONA_LTE();
    
    // Define variables for GPS information
    float latitude = 0.0f;
    float longitude = 0.0f;
    float speed_kph = 0.0f;
    float heading = 0.0f;
    float altitude = 0.0f;
    
    // Latitude, Longitude and Altitude converted from float to char
    char latitudeStr[11];
    char longitudeStr[11];
    char altitudeStr[11];
    char speedKphStr[11];
    char headingStr[11];
    
    // Define variables for time information    
    uint16_t year = 0;
    uint8_t month = 0;
    uint8_t day = 0;
    uint8_t hours = 0;
    uint8_t minutes = 0;
    float seconds = 0.0f;
    
    // Variable to monitor network connectivity
    bool connected = false;
    uint8_t netStatus = 0;
        
    //// METHODS
    // Hardware management
    void resetDevice();
    void initializeDevice();    
    void turnGpsOn();
    void turnGpsOff();
    void turnGprsOn();
    void turnGprsOff();
    
    // Software management
    void updateBaud();
    void getSignalStrength();
    void getNetworkStatus();
    void updateGeoData();
    void uploadDataFile();
    void resetVariables();    
    
};
#endif // Botletics_LTE_GPS_Shield_h

/*
    

    Program Description : 
    
    Created By : Benjamin Kleynhans
    Creation Date : June 16, 2020
    Authors : Benjamin Kleynhans
    
    NOTE: Many functions and user feedback have been commented out due to 
            space restrictions on the Arduino Uno/Adafruit Metro the code
            is being used on.

    Last Modified By : Benjamin Kleynhans
    Last Modified Date : June 26, 2020
    Filename : Botletics_LTE_GPS_Shield.cpp
*/

#include "Botletics_LTE_GPS_Shield.h"

Botletics_LTE_GPS_Shield::Botletics_LTE_GPS_Shield(HardwareSerial* pSerial, int* pBaud, uint8_t* pPWRKEY, uint8_t* pRST, uint8_t* pTX, uint8_t* pRX)
{
    this->pBaud = pBaud;
    this->pSerial = pSerial;
    
    this->pPWRKEY = pPWRKEY;
    this->pRST = pRST;
    this->pTX = pTX;
    this->pRX = pRX;
       
    this->initializeDevice();
    //~ this->updateGeoData();
}

Botletics_LTE_GPS_Shield::~Botletics_LTE_GPS_Shield() {}

// Initialize connections
void Botletics_LTE_GPS_Shield::initializeDevice()
{    
    this->pSerial->println(F("Initializing Device"));
    delay(100);

    //Instantiate the Software Serial interface
    this->pFonaSS = new SoftwareSerial(*this->pRX, *this->pTX);
    
    // Configure reset 
    pinMode(*this->pRST, OUTPUT);
    digitalWrite(*this->pRST, HIGH);         // Set default state to high
    
    // Configure power key
    pinMode(*this->pPWRKEY, OUTPUT);
    
    //~ // Turn on the module
    //~ this->powerOn();
    
    //~ // Get network status after power-on
    //~ this->getNetworkStatus();
}

// Turn the Botletics_LTE_GPS_Shield on
void Botletics_LTE_GPS_Shield::powerOn()
{
    this->pSerial->println(F("\n        --- Turning on Botletics LTE/GPS shield ---"));
    delay(100);
    
    // The module is powered on by pulsing the PWRKEY low for a few milliseconds.  The
    // amount of time depends on the module being used (Reference documentation for details).
    digitalWrite(*this->pPWRKEY, LOW);
    delay(100);
    
    digitalWrite(*this->pPWRKEY, HIGH);
    delay(3000);    // SIM7000 takes about 3 seconds to turn on
    
    this->pSerial->println(F("\n        --- Powered On ---"));
    
    // According to maker, the SIM7000 baud seems to reset after being power cycled
    // (SIMCom firmware related).
    this->updateBaud();
    
    // Set modem to FULL functionality (AT+CFUN=1)
    this->fona.setFunctionality(1);
    
    // Configure the network settings (APN)
    this->fona.setNetworkSettings(F("hologram"));
    
    // Get network status after power-on
    this->getNetworkStatus();
    
    // Update the geographic data
    this->updateGeoData();
}

// Update the baud and set to 9600
void Botletics_LTE_GPS_Shield::updateBaud()
{    
    this->pFonaSS->begin(115200);
    this->pFonaSS->println("AT+IPR=9600");      // Set baud rate to 9600
    delay(100);
    this->pFonaSS->begin(9600);
    
    this->pSerial->println(F("\n        --- Baud Set ---\n"));
    
    // Test if the device is reachable after changing the baud rate
    if (!this->fona.begin(*this->pFonaSS)) {
        this->pSerial->println(F("\n        !!! Couldn't find FONA !!!\n"));
        while(1);
    }
}

// Turn the Botletics_LTE_GPS_Shield off_type
void Botletics_LTE_GPS_Shield::powerOff()
{
    this->pSerial->println(F("\n        --- Turning off Botletics LTE/GPS shield ---\n"));
    
    this->fona.powerDown();
    delay(5000);
    
    this->pSerial->println(F("\n      --> Botletics LTE/GPS shield is off"));
}

float Botletics_LTE_GPS_Shield::getLatitude()
{
    return this->latitude;
}

char* Botletics_LTE_GPS_Shield::getLatitudeStr()
{
    return this->latitudeStr;
}

float Botletics_LTE_GPS_Shield::getLongitude()
{
    return this->longitude;
}

char* Botletics_LTE_GPS_Shield::getLongitudeStr()
{
    return this->longitudeStr;
}

float Botletics_LTE_GPS_Shield::getAltitude()
{
    return this->altitude;
}

char* Botletics_LTE_GPS_Shield::getAltitudeStr()
{    
    return this->altitudeStr;
}

float Botletics_LTE_GPS_Shield::getSpeedKph()
{
    return this->speed_kph;
}

char* Botletics_LTE_GPS_Shield::getSpeedKphStr()
{
    return this->speedKphStr;
}

float Botletics_LTE_GPS_Shield::getHeading()
{
    return this->heading;
}

char* Botletics_LTE_GPS_Shield::getHeadingStr()
{
    return this->headingStr;
}

void Botletics_LTE_GPS_Shield::setYear(uint16_t year)
{
    this->year = year;
}

uint16_t Botletics_LTE_GPS_Shield::getYear()
{
    return this->year;
}

void Botletics_LTE_GPS_Shield::setMonth(uint8_t month)
{
    this->month = month;
}

uint8_t Botletics_LTE_GPS_Shield::getMonth()
{
    return this->month;
}

void Botletics_LTE_GPS_Shield::setDay(uint8_t day)
{
    this->day = day;
}

uint8_t Botletics_LTE_GPS_Shield::getDay()
{
    return this->day;
}

void Botletics_LTE_GPS_Shield::setHours(uint8_t hours)
{
    this->hours = hours;
}

uint8_t Botletics_LTE_GPS_Shield::getHours()
{
    return this->hours;
}

void Botletics_LTE_GPS_Shield::setMinutes(uint8_t minutes)
{
    this->minutes = minutes;
}

uint8_t Botletics_LTE_GPS_Shield::getMinutes()
{
    return this->minutes;
}

void Botletics_LTE_GPS_Shield::setSeconds(float seconds)
{
    this->seconds = seconds;
}

float Botletics_LTE_GPS_Shield::getSeconds()
{
    return this->seconds;
}

// Turn the GPS on
void Botletics_LTE_GPS_Shield::turnGpsOn()
{
    this->pSerial->println(F("\n      --> Turning GPS on"));
    this->fona.enableGPS(true);
    delay(100);
}

// Turn the GPS off
void Botletics_LTE_GPS_Shield::turnGpsOff()
{
    this->pSerial->println(F("\n      --> Turning GPS off"));
    this->fona.enableGPS(false);
    delay(100);
}

// Turn the LTE on
void Botletics_LTE_GPS_Shield::turnGprsOn()
{
    this->pSerial->println(F("\n      --> Turning GPRS on"));
    this->fona.enableGPRS(true);
    delay(100);
}

// Turn the LTE off
void Botletics_LTE_GPS_Shield::turnGprsOff()
{
    this->pSerial->println(F("\n      --> Turning GPRS off"));
    this->fona.enableGPRS(false);
    delay(100);
}

// Get the current signal strength
void Botletics_LTE_GPS_Shield::getSignalStrength()
{
    uint8_t n = this->fona.getRSSI();
    int8_t r;
    
    this->pSerial->print(F("RSSI = "));
    this->pSerial->print(n);
    this->pSerial->print(F(": "));
    
    switch (n) {
        case 0:
            r = -115;
            
            break;
        case 1:
            r = -111;
            
            break;
        case 31:
            r = -52;
            
            break;
        default:
            r = map(n, 2, 30, -110, -54);
    }
    
    this->pSerial->print(r);
    this->pSerial->println(F(" dBm"));
}

// Check whether the device has successfully connected and registered to the cellular network
void Botletics_LTE_GPS_Shield::getNetworkStatus()
{
    byte resetCounter = 0;
    
    // Wait for the network to connect before continuing
    while (!this->connected) {
        // read the network/cellular status
        this->netStatus = fona.getNetworkStatus();
        
        this->pSerial->print(F("Network status "));
        this->pSerial->print(this->netStatus);
        this->pSerial->print(F(": "));
        
        switch (this->netStatus) {
            case 0:
                this->pSerial->println(F("Not registered"));
                
                break;
            case 1:
                this->pSerial->println(F("Registered (home)"));
                
                break;
            case 2:
                this->pSerial->println(F("Not registered (searching)"));
                
                break;
            case 3:
                this->pSerial->println(F("Denied"));
                
                break;
            case 4:
                this->pSerial->println(F("Unknown"));
                
                break;
            case 5:
                this->pSerial->println(F("Registered roaming"));
                
                break;
            default:
                break;
        }
        
        // Set the connected status to true
        if (this->netStatus == 1 || this->netStatus == 5) {
            this->connected = true;
        } else {
            resetCounter++;
        }
        
        if (resetCounter == 10) {
            this->pSerial->println(F("The device has not connected after 10 attempts. Performing reset"));
            
            this->powerOff();
            delay(5000);
            
            this->powerOn();
            delay(5000);
            
            resetCounter = 0;
        }
        
        delay(1000);
    }
}

// Get the current latitude, longitude and altitude
void Botletics_LTE_GPS_Shield::updateGeoData()
{
    // Provide user feedback
    this->pSerial->println(F("\n --- Updating location ---\n"));
    
    // Reset the variables
    this->resetVariables();
    
    // Turn GPS on
    this->turnGpsOn();
    
    // This variable is used to ensure the GPS has connected before leaving the function
    bool validLocation = false;
    
    // Get the location data
    while (!validLocation) {
        this->fona.getGPS(
            &this->latitude,
            &this->longitude,
            &this->speed_kph,
            &this->heading,
            &this->altitude,
            &this->year,
            &this->month,
            &this->day,
            &this->hours,
            &this->minutes,
            &this->seconds
        );
        
        if (this->latitude != 0 && this->longitude != 0 && this->altitude != 0) {
            validLocation = true;
        }
        
        delay(1000);
    }
    
    // Create char-based variable from float variable    
    dtostrf(this->getLatitude(), 4, 6, this->latitudeStr);
    dtostrf(this->getLongitude(), 4, 6, this->longitudeStr);
    dtostrf(this->getAltitude(), 4, 6, this->altitudeStr);
    dtostrf(this->getSpeedKph(), 4, 6, this->speedKphStr);
    dtostrf(this->getHeading(), 4, 6, this->headingStr);
    
    // Print current location
    this->pSerial->println(F("\n        --- Current Location ---\n"));
    this->pSerial->print(F("Latitude    : "));
    this->pSerial->println(this->getLatitude(), 6);
    this->pSerial->print(F("Longitude    : "));
    this->pSerial->println(this->getLongitude(), 6);
    this->pSerial->print(F("Altitude    : "));
    this->pSerial->println(this->getAltitude(), 6);
    this->pSerial->println();
    this->pSerial->println(F("\n        --- Current Date and Time ---\n"));
    this->pSerial->print(this->getYear());
    this->pSerial->print(F("/"));
    this->pSerial->print(this->getMonth());
    this->pSerial->print(F("/"));
    this->pSerial->print(this->getDay());
    this->pSerial->print(F("/"));
    this->pSerial->print(this->getHours());
    this->pSerial->print(F(":"));
    this->pSerial->print(this->getMinutes());
    this->pSerial->print(F(":"));
    this->pSerial->println(this->getSeconds());
    
    delay(1000);
    
    // Turn GPS off
    this->turnGpsOff();
}

void Botletics_LTE_GPS_Shield::resetVariables()
{
    this->latitude = 0.0f;
    this->longitude = 0.0f;
    this->speed_kph = 0.0f;
    this->heading = 0.0f;
    this->altitude = 0.0f;    
    
    this->setYear(0);
    this->setMonth(0);
    this->setDay(0);
    this->setHours(0);
    this->setMinutes(0);
    this->setSeconds(0.0f);
}

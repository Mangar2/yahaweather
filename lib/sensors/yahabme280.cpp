/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Provides a class to connect to WLAN
 */
#include "debug.h"
#include "message.h"
#include "yahabme280.h"

void YahaBME280::activate(uint8_t pin)
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
}

void YahaBME280::init(uint16_t bmeWireAddress)
{
    // Wire.begin();
    _bmeAvailable = bme.begin(bmeWireAddress);

    if (!_bmeAvailable)
    {
        PRINT_IF_DEBUG("Could not find a valid BME280; Sensor-ID: ");
        PRINT_IF_DEBUG(bme.sensorID())
        PRINTLN_IF_DEBUG()
        IF_DEBUG(ScanI2CBus();)
    }
    else
    {
        PRINTLN_IF_DEBUG("BME280 found, start measuring");
    }
}

float YahaBME280::readTemperature()
{
    return bme.readTemperature();
}
float YahaBME280::readHumidity()
{
    return bme.readHumidity();
}
float YahaBME280::readPressure()
{
    return bme.readPressure();
}

Message YahaBME280::getTemperatureMessage()
{
    const Message temperatureMessage(
        _baseTopic + "/temperature",
        String(readTemperature()),
        "send by yaha weather station");

    return temperatureMessage;
}

Message YahaBME280::getHumidityMessage()
{
    const Message humidityMessage(
        _baseTopic + "/humidity",
        String(readHumidity()),
        "send by yaha weather station");

    return humidityMessage;
}

Message YahaBME280::getPressureMessage()
{
    const Message pressureMessage(
        _baseTopic + "/pressure", 
        String(readPressure()), 
        "send by yaha weather station");

    return pressureMessage;
}

Messages_t YahaBME280::getMessages() {
    Messages_t result;
    if (isBMEAvailable()) {
        result.push_back(getTemperatureMessage());
        result.push_back(getHumidityMessage());
        result.push_back(getPressureMessage());
    }
    return result;
}

void printScanInfo(String info, uint8_t address)
{
    Serial.print(info);
    Serial.print("at address 0x");
    if (address < 16)
    {
        Serial.print("0");
    }
    Serial.println(address, HEX);
}

void YahaBME280::ScanI2CBus()
{
    uint8_t error, address;
    int nDevices;
    Serial.println("Scanning hardware I2C ...");
    nDevices = 0;
    for (address = 1; address < 127; address++)
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0)
        {
            printScanInfo("I2C device found at address", address);
        }
        else if (error == 4)
        {
            printScanInfo("Unknown error at address", address);
        }
    }
    Serial.println(String(nDevices) + " I2C devices found\n");
}

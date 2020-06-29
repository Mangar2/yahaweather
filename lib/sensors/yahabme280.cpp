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

#define __DEBUG
#include "debug.h"
#include "message.h"
#include "yahabme280.h"

void YahaBME280::activate(uint8_t pin)
{
    if (pin != 0) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);
    }
}

void YahaBME280::init(uint16_t bmeWireAddress)
{
    // Wire.begin();
    _bmeAvailable = bme.begin(bmeWireAddress);
    PRINT_IF_DEBUG("Initializing BME280 ... ")
    if (!_bmeAvailable)
    {
        PRINT_IF_DEBUG("could not find a valid sensor; Sensor-ID: ");
        PRINT_IF_DEBUG(bme.sensorID())
        PRINTLN_IF_DEBUG()
        IF_DEBUG(ScanI2CBus();)
    }
    else
    {
        PRINTLN_IF_DEBUG("sensor found, start measuring");
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

HtmlPageInfo YahaBME280::getHtmlPage() {
    return HtmlPageInfo(
        R"htmlweather(
        <form>
        <label for="temperature">Temperature</label>
        <input type="text" id="temperature" readonly [value]="sensor/temperature">
        <label for="humidity">Humidity</label>
        <input type="text" id="humidity" readonly [value]="sensor/humidity">
        <label for="pressure">Barometric pressure</label>
        <input type="text" id="pressure" readonly [value]="sensor/pressure">
        <label for="battery">Battery voltage</label>
        <input type="text" id="voltage" readonly [value]="battery/voltage">
        </form>
        )htmlweather",
        "/weather",
        "Weather"
    );
}

void YahaBME280::run() {
    if (isValid()) {
        sendMessageToDevices("sensor/temperature", String(bme.readTemperature()));
        sendMessageToDevices("sensor/humidity", String(bme.readHumidity()));
        sendMessageToDevices("sensor/pressure", String(bme.readPressure()));
    }
}

Messages_t YahaBME280::getMessages(const String& baseTopic) {
    Messages_t result;
    if (isValid()) {
        const Message pressureMessage(baseTopic + "/sensor/pressure", String(readPressure()), "send by ESP8266");
        const Message humidityMessage(baseTopic + "/sensor/humidity", String(readHumidity()), "send by ESP8266");
        const Message temperatureMessage(baseTopic + "/sensor/temperature", String(readTemperature()), "send by ESP8266");
        result.push_back(temperatureMessage);
        result.push_back(humidityMessage);
        result.push_back(pressureMessage);
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
            nDevices++;
        }
        else if (error == 4)
        {
            printScanInfo("Unknown error at address", address);
        }
    }
    Serial.println(String(nDevices) + " I2C devices found\n");
}

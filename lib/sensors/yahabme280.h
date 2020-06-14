/**
 * @license
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @documentation
 * Provides a wrapper to read bme values
 */

#pragma once

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <idevice.h>

class YahaBME280 : public IDevice
{
public:
    /**
     * Initializes the bme sensor
     * @param bmeWireAddress I2C address of the bme
     * @param pin pin providing the Supply voltage for the sensor (power on pin). If it is zero
     * then no pin is activated (bme is always supplied with power)
     */
    YahaBME280(uint16_t bmeWireAddress = 0x76, uint8_t pin = 0)
    {
        activate(pin);
        init(bmeWireAddress);
    }

    /**
     * Reads the temperature
     */
    float readTemperature();

    /**
     * Reads the humidity
     */
    float readHumidity();

    /**
     * Reads the pressure
     */
    float readPressure();

    /**
     * Gets all messages to publish
     */
    virtual Messages_t getMessages(const String& baseTopic);

    /**
     * Running first time on setup
     */
    virtual void setup() { run(); };

    /**
     * Called on loop
     */
    virtual void run();

    /**
     * Gets the weather information page
     */
    HtmlPageInfo getHtmlPage();

    /**
     * Checks, if a bme sensor has been found
     */
    virtual bool isValid() const { return _bmeAvailable; };

    /**
     * Scans the I2CBus and prints addresses of found I2C devices
     */
    void ScanI2CBus();

private:

    /**
     * Initializes the bme
     * @param bmeWireAddress I2C address of the Sensor
     */
    void init(uint16_t bmeWireAddress = 0x76);

    /**
     * Activates the power supply of the BME
     * Optional feature for very low power consuming sensor configuration
     * @param pin Power pin for the sensor
     */
    void activate(uint8_t pin);


    Adafruit_BME280 bme; // I2C
    bool _bmeAvailable;
};



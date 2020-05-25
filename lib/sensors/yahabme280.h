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
#include "message.h"

class YahaBME280
{
public:
    YahaBME280()
    {
    }

    /**
     * Sets the base topic for the device. The base topic is the start of the topic
     * sent to the mqtt broker
     */
    void setBaseTopic(String baseTopic) {
        _baseTopic = baseTopic;
    }

    /**
     * Activates the power supply of the BME
     * Optional feature for very low power consuming sensor configuration
     * @param pin Power pin for the sensor
     */
    void activate(uint8_t pin);


    /**
     * Initializes the bme
     * @param bmeWireAddress I2C address of the Sensor
     */
    void init(uint16_t bmeWireAddress = 0x76);

    /**
     * Checks, if a bme sensor has been found
     */
    bool isBMEAvailable() { return _bmeAvailable; }

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
     * Creates a temperature message to publishh
     */
    Message getTemperatureMessage();

    /**
     * Creates a huidity message to publish
     */
    Message getHumidityMessage();

    /**
     * Creates a pressure message to publish
     */
     Message getPressureMessage();

    /**
     * Scans the I2CBus and prints addresses of found I2C devices
     */
    void ScanI2CBus();

private:
    Adafruit_BME280 bme; // I2C
    String _baseTopic;
    bool _bmeAvailable;
};



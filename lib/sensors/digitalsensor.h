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
#include <idevice.h>

class DigitalSensor : public IDevice
{
public:
    /**
     * Initializes a digital sensor
     * @param inputPin pin to read the digital input
     * @param [powerPin=0] pin to provide power to the digital sensor, unused, if 0
     * then no pin is activated (bme is always supplied with power)
     */
    DigitalSensor(uint8_t inputPin, uint8_t powerPin = 0)
    {
        activate(powerPin);
        _inputPin = inputPin;
    }

    /**
     * Reads the temperature
     */
    bool readInput();

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
     * Gets digital sensor html page
     */
    HtmlPageInfo getHtmlPage();

    /**
     * Checks, if the sensor is valid (always true)
     */
    virtual bool isValid() const { return true; };


private:

    /**
     * Initializes the sensor
     */
    void init();

    /**
     * Activates the power supply of the sensor
     * Optional feature for very low power consuming sensor configuration
     * @param pin Power pin for the sensor
     */
    void activate(uint8_t pin);

    uint8_t _inputPin;
};



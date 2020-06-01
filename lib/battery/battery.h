/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Provides a class to integrate battery control to a battery powered ESP8266 
 */
#pragma once

#include <Arduino.h>
#include <message.h>
#include <map>

class Battery
{
public:
    struct Configuration
    {
        uint16_t normalVoltageSleepTimeInSeconds;
        uint16_t highVoltageSleepTimeInSeconds;
        uint16_t lowVoltageSleepTimeInSeconds;
        float voltageCalibrationDivisor;
        float highVoltage;
        float lowVoltage;

        /**
         * Gets the configuration as key/value map
         */
        std::map<String, String> get();

        /**
         * Sets the configuration from a key/value map
         * @param config configuration settings in a map
         */
        void set(std::map<String, String> config);
    };
    Battery(){};
    Battery(const Configuration &configuration) : _configuration(configuration) {}

    /**
     * Sets the battery configuration
     */
    void setConfiguration(const Configuration &configuration) { _configuration = configuration; }

    /**
     * Measures battery voltage
     */
    float measureVoltage();
    /**
     * @returns true, if the battery voltage is low
     */
    bool isLowVoltage()
    {
        return measureVoltage() < _configuration.lowVoltage;
    }

    /**
     * @returns true, if the battery voltage is high
     */
    bool isHighVoltage()
    {
        return measureVoltage() >= _configuration.highVoltage;
    }

    /**
     * @returns the sleep time in seconds depending on the battery voltage
     */
    uint16_t getSleepTimeInSeconds();

    /**
     * Gets a yaha messages to send the battery voltage
     */
    Messages_t getMessages(const String &baseTopic);

    static const char* htmlForm;

private:
    static const uint8_t BATTERY_PIN = A0;
    Configuration _configuration;
};
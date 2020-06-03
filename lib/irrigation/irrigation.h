/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Provides a class to steer irrigation based on humidity measurement
 */
#pragma once

#include <Arduino.h>
#include <message.h>
#include <map>

class Irrigation
{
public:
    struct Configuration
    {
        Configuration();
        uint16_t lowHumidity;
        uint16_t lowIrrigationDurationInSeconds;
        uint16_t lowWakeupUntilIrrigation;
        uint16_t midIrrigationDurationInSeconds;
        uint16_t midWakeupUntilIrrigation;
        uint16_t highHumidity;
        uint16_t highIrrigationDurationInSeconds;
        uint16_t highWakeupUntilIrrigation;
        float pump2Factor;
        uint8_t pump1Pin;
        uint8_t pump2Pin;

        /**
         * Gets the config as key/value map
         */
        std::map<String, String> get();

        /**
         * Sets the configuration from a key/value map
         * @param config config settings in a map
         */
        void set(std::map<String, String> config);
    };
    Irrigation(){};
    Irrigation(const Configuration &config) : _config(config) {}

    /**
     * Sets the battery config
     */
    void setConfiguration(const Configuration &config) { _config = config; }

    /**
     * Gets the battery configuraiton
     */
    Configuration getConfiguration() { return _config; }


    /**
     * @param humidity currend measured humidity in percent
     * @returns true, if the battery voltage is low
     */
    bool isLowHumidity(float humidity)
    {
        return humidity < _config.lowHumidity;
    }

    /**
     * @param humidity currend measured humidity in percent
     * @returns true, if the humidity is above or equal the high threshold
     */
    bool isHighHumidity(float humidity)
    {
        return humidity >= _config.highHumidity;
    }

    /**
     * @param humidity currend measured humidity in percent
     * @param pumpNo number of the pump to switch (either 1 or 2)
     * @returns the time for irrigation in seconds
     */
    uint16_t getIrrigationTimeInSeconds(float humidity, uint8_t pumpNo);

    /**
     * @param humidity currend measured humidity in percent
     * Gets a yaha messages to send the battery voltage
     */
    Messages_t getMessages(const String &baseTopic, float humidity);

    /**
     * @param humidity currend measured humidity in percent
     */
    void runIrrigation(float humidity);

    static const char* htmlForm;

private:
    Configuration _config;
};
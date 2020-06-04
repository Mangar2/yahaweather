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
    Irrigation(uint8_t pump1Pin = D6, uint8_t pump2Pin = D7) : _pump1Pin(pump1Pin), _pump2Pin(pump2Pin) {};
    Irrigation(const Configuration &config) : _config(config) {}

    /**
     * Sets the battery config
     */
    void setConfiguration(const Configuration &config) { _config = config; }

    /**
     * Gets the battery configuraiton
     */
    Configuration& getConfiguration() { return _config; }


    /**
     * @param humidity currend measured humidity in percent
     * @param pumpNo number of the pump to switch (either 1 or 2)
     * @returns the time for irrigation in seconds
     */
    uint16_t getIrrigationDurationInSeconds(float humidity, uint8_t pumpNo);

    /**
     * Checks, if irrigation should be done
     * @param humidity currend measured humidity in percent
     * @param wakeupAmount amount of wakeups since last irrigation
     * @returns true, if irrigation should be done
     */
    bool doIrrigation(float humidity, uint16_t wakeupAmount);

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
    uint8_t _pump1Pin;
    uint8_t _pump2Pin;
};
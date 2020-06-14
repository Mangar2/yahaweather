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
#include <idevice.h>

class Irrigation : public IDevice
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
    Irrigation(uint8_t pump1Pin = D6, uint8_t pump2Pin = D7); 

    /**
     * Sets configuration from a key/value map
     * @param config map containing the configuration
     */
    virtual void setConfig(jsonObject_t config);
    
    /**
     * Gets configuration as key/value map
     * @returns configuration 
     */
    virtual jsonObject_t getConfig() { return _config.get(); };

    /**
     * @returns true, if the device is initialized and working
     */
    virtual bool isValid() const { return true; };

    /**
     * Gets messages to send
     * @param baseTopic start topic to be used to create the message topic
     * @returns a list of messages to send with topic, value and reason
     */
    virtual Messages_t getMessages(const String& baseTopic);

    /**
     * Checks, if irrigation should be done
     * @returns true, if irrigation should be done
     */
    bool doIrrigation();

    /**
     * Runs the irrigation
     */
    virtual void run();

    static const char* htmlForm;

private:

    /**
     * @param pumpNo number of the pump to switch (either 1 or 2)
     * @returns the time for irrigation in seconds
     */
    uint16_t getIrrigationDurationInSeconds(uint8_t pumpNo);

    Configuration _config;
    uint8_t _pump1Pin;
    uint8_t _pump2Pin;
    float _humidity;
    uint16_t _wakeupAmount;
};
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
        uint16_t lowDurationInSeconds;
        uint16_t lowWakeup;
        uint16_t highDurationInSeconds;
        uint16_t highWakeup;
        float pump2Factor;

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
    virtual void setConfig(jsonObject_t& config);
    
    /**
     * Gets configuration as key/value map
     * @returns configuration 
     */
    virtual jsonObject_t getConfig() { return _config.get(); };

    /**
     * Writes the configuration to EEPROM
     * @param EEPROMAddress EEPROM address to write to
     * @returns EEPROM address for the next device
     */
    virtual uint16_t writeConfigToEEPROM(uint16_t EEPROMAddress);

    /**
     * Reads configuration from EEPROM
     * @param EEPROMAddress EEPROM address to read from
     * @returns EEPROM address for the next device
     */
    virtual uint16_t readConfigFromEEPROM(uint16_t EEPROMAddress);

    /**
     * Gets messages to send
     * @param baseTopic start topic to be used to create the message topic
     * @returns a list of messages to send with topic, value and reason
     */
    virtual Messages_t getMessages(const String& baseTopic);

    
    /**
     * Stores messages important for irrigation
     * sensor/humidity the current humidity
     * rtc/wakeupAmount the amount of wakeups since last irrigation
     * @param key message identifier
     * @param value message value
     */
    virtual void handleMessage(const String& key, const String& value);

    /**
     * Checks, if irrigation should be done
     * @returns true, if irrigation should be done
     */
    bool doIrrigation();

    /**
     * Runs the irrigation
     */
    virtual void run();

    /**
     * Gets an info about the matching html page
     */
    virtual HtmlPageInfo getHtmlPage() { return HtmlPageInfo(htmlForm, "/irrigation", "Irrigation"); }


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
    static const char* htmlForm;
};
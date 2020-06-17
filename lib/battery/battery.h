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
#include <idevice.h>

class Battery : public IDevice
{
public:
    struct Configuration
    {
        Configuration();
        uint16_t normalVoltageSleepTimeInSeconds;
        uint16_t highVoltageSleepTimeInSeconds;
        uint16_t lowVoltageSleepTimeInSeconds;
        uint8_t batteryMode;
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

    /**
     * Sets the battery configuration
     */
    virtual void setConfig(jsonObject_t& config) { 
        _config.set(config); 
        setBatteryMode(_config.batteryMode);
    }

    /**
     * Sets battery mode on fast reset
     * @param config all relevant data
     */
    virtual void handleMessage(const String& key, const String& value) {
        if (key == "rtc/startType" && value == "fastReset") { 
            setBatteryMode(false);
        }
    }

    /**
     * Gets the battery configuraiton
     */
    virtual jsonObject_t getConfig() { 
        return _config.get();
    }

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
     * Sends voltage measurement to all devices on setup
     */
    virtual void setup() {
        run();
    }

    /**
     * Sends voltage measurement to all devices on loop
     */
    virtual void run() {
        sendMessageToDevices("battery/voltage", String(measureVoltage()));
        sendMessageToDevices("battery/sleepTimeInSeconds", String(getSleepTimeInSeconds()));
    }

    /**
     * @returns the sleep time in seconds depending on the battery voltage
     */
    uint16_t getSleepTimeInSeconds();

    /**
     * Gets a yaha messages to send the battery voltage
     */
    virtual Messages_t getMessages(const String &baseTopic);

    /**
     * Gets an info about the matching html page
     */
    virtual HtmlPageInfo getHtmlPage() { return HtmlPageInfo(htmlForm, "/battery", "Battery"); }

private:

    static const char* htmlForm;

    /**
     * @returns true, if battery mode is activated
     */
    void setBatteryMode(bool mode) {
        _config.batteryMode = mode ? 1 : 0;
        sendMessageToDevices("battery/mode", String(_config.batteryMode));
    }

    /**
     * Measures battery voltage
     */
    float measureVoltage();

    /**
     * @returns true, if the battery voltage is low
     */
    bool isLowVoltage()
    {
        return measureVoltage() < _config.lowVoltage;
    }

    /**
     * @returns true, if the battery voltage is high
     */
    bool isHighVoltage()
    {
        return measureVoltage() >= _config.highVoltage;
    }

    static const uint8_t BATTERY_PIN = A0;

    Configuration _config;
};
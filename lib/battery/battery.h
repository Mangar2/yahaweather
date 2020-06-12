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
    virtual void setConfig(jsonObject_t config) { _config.set(config); }

    /**
     * Gets the battery configuraiton
     */
    virtual jsonObject_t getConfig() { 
        auto result = _config.get();
        result["voltage"] = String(measureVoltage());
        return result; 
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

    /**
     * @returns true, if battery mode is activated
     */
    bool isBatteryMode() {
        return _config.batteryMode == 1;
    }

    
    /**
     * @returns true, if battery mode is activated
     */
    void setBatteryMode(bool mode) {
        _config.batteryMode = mode ? 1 : 0;
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
     * Battery device is always valid
     */
    virtual bool isValid() const { return true; }

    static const char* htmlForm;

private:
    static const uint8_t BATTERY_PIN = A0;
    Configuration _config;
};
/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Yaha station based on ESP8266
 */

#pragma once

#include <Arduino.h>
#include <imessagebroker.h>
#include <idevice.h>
#include <wlan.h>
#include <brokerproxy.h>
#include <mqttserver.h>
#include <eepromaccess.h>
#include <runtime.h>

class YahaServer : public IMessageBroker {
public:
    YahaServer() : _isBatteryMode(true), _isPowerOn(false) {
        MQTTServer::registerOnUpdateFunction(updateConfig);
        addDevice(&wlan);
        addDevice(&brokerProxy);
    }

    /**
     * Setup everything
     * @param stationSSID name of the station set up, if WLAN connection is not possible
     */
    void setup(const String stationSSID = "YahaESP8266");

    /**
     * Close down and go to sleep
     */
    void closeDown();

    /**
     * loop action
     */
    void loop();

    void addDevice(IDevice* device) {
        device->setMessageBroker(this);
        _devices.push_back(device);
    }

    /**
     * Updates the configuration, stores it to eeprom
     */
    static void updateConfig(std::map<String, String> config);

    /**
     * Initializes the eeprom, reads the configuration and initializes the objects
     */
    static void setupEEPROM();

    /**
     * Sends a key/value message to all registered devices
     * @param key indetifier of the message
     * @param value value of the message
     */
    virtual void sendMessageToDevices(const String& key, const String& value);

    static BrokerProxy brokerProxy;
    static WLAN wlan;

private:

    static void setDeviceConfigFromJSON(jsonObject_t& config);

    static const uint16_t EEPROM_START_ADDR = 0;
    static std::vector<IDevice*> _devices;

    bool _isBatteryMode;
    bool _isPowerOn;
    uint16_t _sleepTimeInSeconds;
    Runtime _runtime;


};

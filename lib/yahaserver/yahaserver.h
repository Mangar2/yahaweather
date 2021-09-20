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
#include "imessagebroker.h"
#include "idevice.h"
#include "wlan.h"
#include "brokerproxy.h"
#include "mqttserver.h"
#include "eepromaccess.h"
#include "runtime.h"

class YahaServer : public IMessageBroker {
public:
    YahaServer() : _isBatteryMode(false), _isPowerOn(false) {
        MQTTServer::registerOnUpdateFunction(updateConfig);
        addDevice(&wlan);
        addDevice(&brokerProxy);
    }

    /**
     * Setup everything
     * @param APSSID name of the access point set up, if WLAN connection is not possible
     */
    void setup(const String APSSID = "YahaESP8266");

    /**
     * Close down and go to sleep
     */
    void closeDown();

    /**
     * loop action
     */
    void loop();

    /**
     * Adds a device
     * @param device pointer to a device object
     * @param priority setup priority (0, setup called after yahaserver setup, 1 => setup called before)
     */
    void addDevice(IDevice* device, uint8_t priority = 0) {
        device->setMessageBroker(this);
        _devices.push_back(device);
        _priority.push_back(priority);
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

    /**
     * Setup all devices of a priority
     * @param priority of the devices to set up
     */
    void setupDevices(uint8_t priority);

    static void setDeviceConfigFromJSON(jsonObject_t& config);

    static const uint16_t EEPROM_START_ADDR = 0;
    static std::vector<IDevice*> _devices;
    static std::vector<uint8_t> _priority;

    bool _isBatteryMode;
    bool _isPowerOn;
    uint16_t _sleepTimeInSeconds;
    Runtime _runtime;


};

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

#define __DEBUG
#include "yahaserver.h"
#include <debug.h>

BrokerProxy YahaServer::brokerProxy;
WLAN YahaServer::wlan;
std::vector<IDevice*> YahaServer::_devices;

void YahaServer::sendMessageToDevices(const String& key, const String& value) {
    for (auto const& device: _devices) {
        device->handleMessage(key, value);
    }
    MQTTServer::setData(key, value);
    if (key == "battery/mode") {
        _isBatteryMode = value == "on";
    }
    if (key == "battery/sleepTimeInSeconds") {
        _sleepTimeInSeconds = value.toInt();
    }
    if (key == "rtc/isPowerOn") {
        _isPowerOn = value == "true";
    }
}

void YahaServer::setup(const String stationSSID) {
    setupEEPROM();
    MQTTServer::begin();
    wlan.connect(stationSSID);
    for (auto const& device: _devices) {
        MQTTServer::addForm(device->getHtmlPage());
        device->setup();
    }
    PRINTLN_VARIABLE_IF_DEBUG(system_get_free_heap_size())
}

void YahaServer::closeDown() {
    const uint32_t DEEP_SLEEP_ONE_SECOND = 1000000;
    if (wlan.isConnected()) {
        brokerProxy.publishMessage(_runtime.getMessage(brokerProxy.getBaseTopic()));
    }
    for (auto const& device: _devices) {
        device->closeDown();
    }
    if (wlan.isConnected()) {
        wlan.disconnect(); 
    }
    PRINTLN_IF_DEBUG("\nDisconnected from WiFi, going to sleep for " + String(_sleepTimeInSeconds) + " seconds ...")
    IF_DEBUG(delay(100);)
    ESP.deepSleep(_sleepTimeInSeconds * DEEP_SLEEP_ONE_SECOND); 
}

void YahaServer::loop() {
    if (wlan.isConnected()) {
        for(auto const& device: _devices) {
            brokerProxy.publishMessages(device->getMessages(brokerProxy.getBaseTopic()));
        }
        PRINT_IF_DEBUG("Waiting for broker to send messages, ... ")
        for (uint16_t i = 0; i < 50; i++) {
            MQTTServer::handleClient();
            delay(10);
        }
        PRINTLN_IF_DEBUG(" Done")
        if (MQTTServer::isChanged()) {
            brokerProxy.publishMessages(MQTTServer::getMessages(brokerProxy.getBaseTopic()));
            MQTTServer::setChanged(false);
        }
    }
    for (auto const& device: _devices) {
        device->run();
    }
    bool noWLANAfterPowerOn = _isPowerOn && !wlan.isConnected();
    if (!noWLANAfterPowerOn && (_isBatteryMode || _sleepTimeInSeconds == 0)) {
        closeDown();
    } else {
        for (uint16_t i = 0; i < 5000; i++) {
            MQTTServer::handleClient();
            delay(10);
        }
    }
}


void YahaServer::setDeviceConfigFromJSON(jsonObject_t& config) {
    for (auto const& device: _devices) {
        device->setConfig(config);
    }
}

void YahaServer::updateConfig(jsonObject_t config) {
    PRINTLN_IF_DEBUG("update Configuration")

    uint16_t EEPROMAddress = EEPROM_START_ADDR;

    for (auto const& device: _devices) {
        device->setConfig(config);
        EEPROMAddress = device->writeConfigToEEPROM(EEPROMAddress);
    }
    EEPROMAccess::commit();
    PRINTLN_IF_DEBUG("Configuration committed")
}

void YahaServer::setupEEPROM() {
    // Initializes MQTT data from default values

    EEPROMAccess::init();
    uint16_t EEPROMAddress = EEPROM_START_ADDR;

    for (auto const& device: _devices) {
        // Save initialized values, if eeprom is not initialized
        MQTTServer::setData(device->getConfig());
        EEPROMAddress = device->readConfigFromEEPROM(EEPROMAddress);
    }

    if (wlan.isInitialized()) {
        PRINTLN_IF_DEBUG("Setup configuration from EEPROM")
        for (auto const& device: _devices) {
            MQTTServer::setData(device->getConfig());
        }
    } else {
        PRINTLN_IF_DEBUG("No configuration in EEPROM, please configure the device")
        wlan.clear();
        // Restore saved values
        setDeviceConfigFromJSON(MQTTServer::getData());
    }
}



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

YahaServer::Configuration YahaServer::_config;
BrokerProxy YahaServer::brokerProxy;
WLAN YahaServer::wlan;
std::vector<IDevice*> YahaServer::_devices;

void YahaServer::sendMessageToDevices(const String& key, const String& value) {
    for (auto const& device: _devices) {
        device->handleMessage(key, value);
    }
    MQTTServer::setData(key, value);
    if (key == "battery/mode") {
        _isBatteryMode = value == "1";
    }
    if (key == "battery/sleepTimeInSeconds") {
        _sleepTimeInSeconds = value.toInt();
    }
}

void YahaServer::setup(const String stationSSID) {
    MQTTServer::registerOnUpdateFunction(updateConfig);
    addDevice(&brokerProxy);
    addDevice(&wlan);
    setupEEPROM();
    MQTTServer::begin();
    WLAN::connect(_config.wlan, stationSSID);
    brokerProxy.connect();
    MQTTServer::addForm("/wlan", "Wlan", WLAN::htmlForm);
    for (auto const& device: _devices) {
        MQTTServer::addForm(device->getHtmlPage());
        device->setup();
    }
    PRINTLN_VARIABLE_IF_DEBUG(system_get_free_heap_size())
}

void YahaServer::closeDown() {
    const uint32_t DEEP_SLEEP_ONE_SECOND = 1000000;
    for (auto const& device: _devices) {
        device->closeDown();
    }
    if (WLAN::isConnected()) {
        brokerProxy.publishMessage(_runtime.getMessage(brokerProxy.getBaseTopic()));
        brokerProxy.disconnect();
        delay(500);
        WLAN::disconnect(); 
    }
    String message = "\nDisconnected from WiFi, going to sleep for " + String(_sleepTimeInSeconds) + " seconds ...";
    PRINTLN_IF_DEBUG(message)
    IF_DEBUG(delay(100);)
    ESP.deepSleep(_sleepTimeInSeconds * DEEP_SLEEP_ONE_SECOND); 
}

void YahaServer::loop() {
    if (WLAN::isConnected()) {
        for (uint16_t i = 0; i < 30; i++) {
            MQTTServer::handleClient();
            delay(10);
        }
        for(auto const& device: _devices) {
            brokerProxy.publishMessages(device->getMessages(brokerProxy.getBaseTopic()));
        }
        if (MQTTServer::isChanged()) {
            brokerProxy.publishMessages(MQTTServer::getMessages(brokerProxy.getBaseTopic()));
        }
    }
    for (auto const& device: _devices) {
        device->run();
    }
    if (_isBatteryMode) {
        closeDown();
    } else {
        for (uint16_t i = 0; i < 5000; i++) {
            MQTTServer::handleClient();
            delay(10);
        }
    }
}

void YahaServer::setMQTTDataFromEEPROMConfig() {
    MQTTServer::setData(_config.wlan.get());
}

void YahaServer::setEEPROMConfigFromJSON(jsonObject_t& config) {
    _config.wlan.set(config);
}

void YahaServer::setDeviceConfigFromJSON(jsonObject_t& config) {
    for (auto const& device: _devices) {
        device->setConfig(config);
    }
}

void YahaServer::updateConfig(jsonObject_t config) {
    PRINTLN_IF_DEBUG("update Configuration")
    setEEPROMConfigFromJSON(config);
    setDeviceConfigFromJSON(config);
    uint16_t EEPROMAddress = EEPROM_START_ADDR;
    EEPROMAccess::write(EEPROM_START_ADDR, (uint8_t*) &_config, sizeof(_config));
    EEPROMAddress += sizeof(_config);

    for (auto const& device: _devices) {
        EEPROMAddress = device->writeConfigToEEPROM(EEPROMAddress);
    }
        
    EEPROMAccess::commit();
    PRINTLN_IF_DEBUG("Configuration committed")
}

void YahaServer::setupEEPROM() {
    // Initializes MQTT data from default values
    setMQTTDataFromEEPROMConfig();

    PRINTLN_IF_DEBUG("Setup EEPROM")
    EEPROMAccess::init();
    uint16_t EEPROMAddress = EEPROM_START_ADDR;
    EEPROMAccess::read(EEPROMAddress, (uint8_t*) &_config, sizeof(_config));
    EEPROMAddress += sizeof(_config);
    for (auto const& device: _devices) {
        EEPROMAddress = device->readConfigFromEEPROM(EEPROMAddress);
    }

    if (_config.wlan.isInitialized()) {
        // Overwrite MQTT data from EEPROM values
        setMQTTDataFromEEPROMConfig();
    } else {
        _config.wlan.clear();
    }
    setDeviceConfigFromJSON(MQTTServer::getData());
    for (auto const& device: _devices) {
        MQTTServer::setData(device->getConfig());
    }
    PRINTLN_IF_DEBUG("Setup EEPROM finished")
}



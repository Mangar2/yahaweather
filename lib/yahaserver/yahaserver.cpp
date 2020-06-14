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
#include <rtc.h>

YahaServer::Configuration YahaServer::_config;
Runtime YahaServer::runtime;
BrokerProxy YahaServer::brokerProxy;
Irrigation YahaServer::irrigation;
Switch YahaServer::digitalSwitch;
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
    addDevice(new Battery());
    addDevice(new RTC());
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
        brokerProxy.publishMessage(runtime.getMessage(brokerProxy.getBaseTopic()));
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

    for (auto const& device: _devices) {
        device->run();
    }
    if (WLAN::isConnected()) {
        for(auto const& device: _devices) {
            brokerProxy.publishMessages(device->getMessages(brokerProxy.getBaseTopic()));
        }
        if (MQTTServer::isChanged()) {
            brokerProxy.publishMessages(MQTTServer::getMessages(brokerProxy.getBaseTopic()));
        }
        for (uint16_t i = 0; i < 30; i++) {
            MQTTServer::handleClient();
            delay(10);
        }
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

void YahaServer::updateConfig(std::map<String, String> config) {
    PRINTLN_IF_DEBUG("update Configuration")
    _config.wlan.set(config);
    _config.broker.set(config);
    _config.battery.set(config);
    _config.irrigation.set(config);
    digitalSwitch.set(config);
    MQTTServer::setData(digitalSwitch.get());
    brokerProxy.setConfiguration(_config.broker);
    battery.setConfig(config);
    irrigation.setConfig(config);
    EEPROMAccess::write(EEPROM_START_ADDR, (uint8_t*) &_config, sizeof(_config));
    EEPROMAccess::commit();
    PRINTLN_IF_DEBUG("Configuration committed")
}

void YahaServer::setupEEPROM() {
    PRINTLN_IF_DEBUG("Setup EEPROM")
    EEPROMAccess::init();
    EEPROMAccess::read(EEPROM_START_ADDR, (uint8_t*) &_config, sizeof(_config));
    if (_config.wlan.isInitialized()) {
        battery.setConfig(_config.battery.get());
        brokerProxy.setConfiguration(_config.broker);
        irrigation.setConfig(_config.irrigation.get());
    } else {
        _config.wlan.clear();
        _config.battery.set(battery.getConfig());
        _config.broker = brokerProxy.getConfiguration();
        _config.irrigation.set(irrigation.getConfig());
    }
    for (auto const& device: _devices) {
        MQTTServer::setData(device->getConfig());
    }
    MQTTServer::setData(_config.broker.get());
    MQTTServer::setData(_config.wlan.get());
    MQTTServer::setData(_config.irrigation.get());
    MQTTServer::setData(digitalSwitch.get());
    PRINTLN_IF_DEBUG("Setup EEPROM finished")
}



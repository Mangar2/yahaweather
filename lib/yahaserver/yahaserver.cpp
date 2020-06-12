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
#include "debug.h"

YahaServer::Configuration YahaServer::_config;
Runtime YahaServer::runtime;
BrokerProxy YahaServer::brokerProxy;
Irrigation YahaServer::irrigation;
Switch YahaServer::digitalSwitch;
Battery YahaServer::battery;


void YahaServer::setup(const String stationSSID) {
    MQTTServer::registerOnUpdateFunction(updateConfig);
    setupEEPROM();
    RTC::initWakeupCounter();
    MQTTServer::begin();
    WLAN::connect(_config.wlan, stationSSID);
    brokerProxy.connect();
    RTC::incWakeupAmount();
    if (RTC::isFastReset()) {
        battery.setBatteryMode(false);
    }
    addDevice(&battery);
    MQTTServer::addForm("/battery", "Battery", Battery::htmlForm);
    MQTTServer::addForm("/wlan", "Wlan", WLAN::htmlForm);
    PRINTLN_VARIABLE_IF_DEBUG(system_get_free_heap_size())
}

void YahaServer::closeDown() {
    const uint32_t DEEP_SLEEP_ONE_SECOND = 1000000;
    if (WLAN::isConnected()) {
        brokerProxy.publishMessage(runtime.getMessage(brokerProxy.getBaseTopic()));
        brokerProxy.disconnect();
        delay(500);
        WLAN::disconnect(); 
    }
    String message = "\nDisconnected from WiFi, going to sleep for " + String(battery.getSleepTimeInSeconds()) + " seconds ...";
    PRINTLN_IF_DEBUG(message)
    IF_DEBUG(delay(100);)
    ESP.deepSleep(battery.getSleepTimeInSeconds() * DEEP_SLEEP_ONE_SECOND); 
}

void YahaServer::loop() {

    for (auto const& device: _devices) {
        MQTTServer::setData(device->getConfig());
    }
    if (WLAN::isConnected()) {
        for(auto const& device: _devices) {
            brokerProxy.publishMessages(device->getMessages(brokerProxy.getBaseTopic()));
        }
    }
    if (WLAN::isConnected()) {
        PRINTLN_VARIABLE_IF_DEBUG(RTC::getWakeupAmount());
        Message wakeupAmount(brokerProxy.getBaseTopic() + "/wakeupAmount", String(RTC::getWakeupAmount()), String("send by ESP8266"));
        brokerProxy.publishMessage(wakeupAmount);
        if (MQTTServer::isChanged()) {
            brokerProxy.publishMessages(MQTTServer::getMessages(brokerProxy.getBaseTopic()));
        }
        for (uint16_t i = 0; i < 30; i++) {
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
    MQTTServer::setData(battery.getConfig());
    MQTTServer::setData(_config.broker.get());
    MQTTServer::setData(_config.wlan.get());
    MQTTServer::setData(_config.irrigation.get());
    MQTTServer::setData(digitalSwitch.get());
    PRINTLN_IF_DEBUG("Setup EEPROM finished")
}



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
Battery YahaServer::battery;
Runtime YahaServer::runtime;
BrokerProxy YahaServer::brokerProxy;


void YahaServer::setup(const String stationSSID) {
    MQTTServer::registerOnUpdateFunction(updateConfig);
    setupEEPROM();
    RTC::initWakeupCounter();
    MQTTServer::begin();
    WLAN::connect(_config.wlan, stationSSID);
    brokerProxy.connect();
    RTC::incWakeupAmount();

    MQTTServer::addForm("/battery", Battery::htmlForm);
    MQTTServer::addForm("/wlan", WLAN::htmlForm);
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
    MQTTServer::setData("voltage", String(battery.measureVoltage()));
    if (WLAN::isConnected()) {
        brokerProxy.publishMessages(battery.getMessages(brokerProxy.getBaseTopic()));
        for (uint16_t i = 0; i < 5; i++) {
            MQTTServer::handleClient();
            delay(10);
        }
    }

    if (!RTC::isFastReset()) {
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
    brokerProxy.setConfiguration(_config.broker);
    battery.setConfiguration(_config.battery);
    EEPROMAccess::write(EEPROM_START_ADDR, (uint8_t*) &_config, sizeof(_config));
    EEPROMAccess::commit();
    PRINTLN_IF_DEBUG("Configuration committed")
}

void YahaServer::setupEEPROM() {
    PRINTLN_IF_DEBUG("Setup EEPROM")
    EEPROMAccess::init();
    EEPROMAccess::read(EEPROM_START_ADDR, (uint8_t*) &_config, sizeof(_config));
    battery.setConfiguration(_config.battery);
    brokerProxy.setConfiguration(_config.broker);
    MQTTServer::setData(_config.battery.get());
    MQTTServer::setData(_config.broker.get());
    MQTTServer::setData(_config.wlan.get());
    PRINTLN_IF_DEBUG("Setup EEPROM finished")
}



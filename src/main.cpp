#define __DEBUG
#include <Arduino.h>
#include <wlan.h>
#include <yahabme280.h>
#include <brokerproxy.h>
#include <mqttserver.h>
#include <json.h>
#include <eepromaccess.h>
#include <rtc.h>
#include <runtime.h>

#include "htmlweatherform.h"

const uint8_t ACTIVATE_BME280_PIN = 14;
const uint32_t SERIAL_SPEED = 115200; 
uint32_t HTTP_REST_PORT = 80;
const uint16_t EEPROM_START_ADDR = 0;
const uint32_t MILLISECONDS_IN_A_SECOND = 1000;
const uint32_t DEEP_SLEEP_ONE_MINUTE = 60 *  1000000;
const uint32_t DEEP_SLEEP_TIME = 60 * DEEP_SLEEP_ONE_MINUTE;
const char* STATION_NAME = "ESP8266_weather";

YahaBME280 bme;
BrokerConfiguration brokerConfiguration;
BrokerProxy brokerProxy;
WLANConfiguration wlanConfiguration;
Runtime runtime;

void writeConfig() {
    uint16_t startAddress = EEPROM_START_ADDR;
    EEPROMAccess::write(startAddress, (uint8_t*) &brokerConfiguration, sizeof(brokerConfiguration));
    startAddress += sizeof(brokerConfiguration);
    EEPROMAccess::write(startAddress, (uint8_t*) &wlanConfiguration, sizeof(WLANConfiguration));
    EEPROMAccess::commit();
}

void setupMQTTServer() {
    MQTTServer::begin(HTTP_REST_PORT);
    MQTTServer::addForm("/", htmlWeatherForm);
    MQTTServer::on("/wlan", []() {
        wlanConfiguration.uuid = wlanConfiguration.getUUID();
        wlanConfiguration.ssid = MQTTServer::getArgValue("ssid");
        wlanConfiguration.password = MQTTServer::getArgValue("password");
        writeConfig();
    });
    MQTTServer::on("/broker", []() {
        brokerConfiguration.brokerHost = MQTTServer::getArgValue("brokerHost");
        brokerConfiguration.brokerPort = MQTTServer::getArgValue("brokerPort");
        writeConfig();
    });
    MQTTServer::on("/client", []() {
        brokerConfiguration.clientName = MQTTServer::getArgValue("clientName");
        brokerConfiguration.baseTopic = MQTTServer::getArgValue("baseTopic");
        brokerConfiguration.subscribeTo = MQTTServer::getArgValue("subscribeTo");
        writeConfig();
    });
}

void setupEEPROM() {
    EEPROMAccess::init();
    uint16_t startAddress = EEPROM_START_ADDR;
    EEPROMAccess::read(startAddress, (uint8_t*) &brokerConfiguration, sizeof(brokerConfiguration));
    startAddress += sizeof(brokerConfiguration);
    EEPROMAccess::read(startAddress, (uint8_t*) &wlanConfiguration, sizeof(WLANConfiguration));
}

/**
 * Setup the broker configuration from configuration
 */
void setupBrokerProxy() {
    MQTTServer::setData("brokerHost", brokerConfiguration.brokerHost);
    MQTTServer::setData("brokerPort", brokerConfiguration.brokerPort);
    MQTTServer::setData("clientName", brokerConfiguration.clientName);
    MQTTServer::setData("baseTopic", brokerConfiguration.baseTopic);
    MQTTServer::setData("subscribeTo", brokerConfiguration.subscribeTo);
    brokerProxy.setConfiguration(brokerConfiguration, WLAN::getLocalIP(), String(HTTP_REST_PORT));
    brokerProxy.connect();
    brokerProxy.subscribe(brokerConfiguration.subscribeTo, 1);
}

/**
 * setup the WLAN from configuration
 */
void setupWLAN() {
    MQTTServer::setData("ssid", wlanConfiguration.ssid);
    WLAN::connect(wlanConfiguration, STATION_NAME);
}

/**
 * Setup everything
 */
void setup() {
    INIT_SERIAL_IF_DEBUG(SERIAL_SPEED)
    RTC::initWakeupCounter();
    setupEEPROM();
    setupWLAN();
    setupMQTTServer();
    setupBrokerProxy();
    // bme.activate(ACTIVATE_BME280_PIN);
    bme.setBaseTopic(brokerConfiguration.baseTopic);
    bme.init(0x77);
    RTC::incWakeupAmount();
    PRINTLN_VARIABLE_IF_DEBUG(system_get_free_heap_size())
}

/**
 * Close down and go to sleep
 */
void closeDown() {
    if (WLAN::isConnected()) {
        brokerProxy.publishMessage(runtime.getMessage(brokerConfiguration.baseTopic));
        brokerProxy.disconnect();
        delay(500);
        WLAN::disconnect(); 
    }
    PRINTLN_IF_DEBUG("\nDisconnected from WiFi, going to sleep ...");
    ESP.deepSleep(DEEP_SLEEP_TIME); 
}

/**
 * main loop
 */
void loop() {
    if (bme.isBMEAvailable()) {
        PRINTLN_IF_DEBUG(bme.getTemperatureMessage().toPublishString())
        PRINTLN_IF_DEBUG(bme.getHumidityMessage().toPublishString())
        PRINTLN_IF_DEBUG(bme.getPressureMessage().toPublishString())
    }

    if (WLAN::isConnected()) {
        MQTTServer::handleClient();
        brokerProxy.publishMessage(bme.getTemperatureMessage());
        brokerProxy.publishMessage(bme.getHumidityMessage());
        brokerProxy.publishMessage(bme.getPressureMessage());
        MQTTServer::setData("temperature", String(bme.readTemperature()));
        MQTTServer::setData("humidity", String(bme.readHumidity()));
        MQTTServer::setData("pressure", String(bme.readPressure()));
        for (uint16_t i = 0; i < 5; i++) {
            MQTTServer::handleClient();
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
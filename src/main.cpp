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
#include <battery.h>
#include <htmlBatteryForm.h>


#include "htmlweatherform.h"

const uint8_t ACTIVATE_BME280_PIN = 14;
const uint32_t SERIAL_SPEED = 115200; 
uint32_t HTTP_REST_PORT = 80;
const uint16_t EEPROM_START_ADDR = 0;
const uint32_t DEEP_SLEEP_ONE_SECOND = 1000000;
const char* STATION_NAME = "ESP8266_weather";

YahaBME280 bme;
BrokerProxy brokerProxy;
Runtime runtime;
Battery battery;

struct Configuration {
    BrokerConfiguration broker;
    WLANConfiguration wlan;
    Battery::Configuration battery;
};

Configuration configuration;

void writeConfig() {
    EEPROMAccess::write(EEPROM_START_ADDR, (uint8_t*) &configuration, sizeof(configuration));
    EEPROMAccess::commit();
}

void setupEEPROM() {
    EEPROMAccess::init();
    EEPROMAccess::read(EEPROM_START_ADDR, (uint8_t*) &configuration, sizeof(configuration));
    battery.setConfiguration(configuration.battery);
    MQTTServer::setData(configuration.battery.get());
}

void setupMQTTServer() {
    MQTTServer::begin(HTTP_REST_PORT);
    MQTTServer::addForm("/", htmlWeatherForm);
    MQTTServer::addForm("/battery", htmlBatteryForm);
    MQTTServer::on("/wlan", []() {
        configuration.wlan.uuid = configuration.wlan.getUUID();
        configuration.wlan.ssid = MQTTServer::getArgValue("ssid");
        configuration.wlan.password = MQTTServer::getArgValue("password");
        writeConfig();
    });
    MQTTServer::on("/broker", []() {
        configuration.broker.brokerHost = MQTTServer::getArgValue("brokerHost");
        configuration.broker.brokerPort = MQTTServer::getArgValue("brokerPort");
        writeConfig();
    });
    MQTTServer::on("/client", []() {
        configuration.broker.clientName = MQTTServer::getArgValue("clientName");
        configuration.broker.baseTopic = MQTTServer::getArgValue("baseTopic");
        configuration.broker.subscribeTo = MQTTServer::getArgValue("subscribeTo");
        writeConfig();
    });
    MQTTServer::on("/battery", []() {
        configuration.battery.set(MQTTServer::getConfigMap());
        battery.setConfiguration(configuration.battery);
        writeConfig();
    });
}

/**
 * Setup the broker configuration from configuration
 */
void setupBrokerProxy() {
    MQTTServer::setData("brokerHost", configuration.broker.brokerHost);
    MQTTServer::setData("brokerPort", configuration.broker.brokerPort);
    MQTTServer::setData("clientName", configuration.broker.clientName);
    MQTTServer::setData("baseTopic", configuration.broker.baseTopic);
    MQTTServer::setData("subscribeTo", configuration.broker.subscribeTo);
    brokerProxy.setConfiguration(configuration.broker, WLAN::getLocalIP(), String(HTTP_REST_PORT));
    brokerProxy.connect();
    brokerProxy.subscribe(configuration.broker.subscribeTo, 1);
}

/**
 * setup the WLAN from configuration
 */
void setupWLAN() {
    MQTTServer::setData("ssid", configuration.wlan.ssid);
    WLAN::connect(configuration.wlan, STATION_NAME);
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
    bme.setBaseTopic(configuration.broker.baseTopic);
    bme.init(0x77);
    RTC::incWakeupAmount();
    PRINTLN_VARIABLE_IF_DEBUG(system_get_free_heap_size())
}

/**
 * Close down and go to sleep
 */
void closeDown() {
    if (WLAN::isConnected()) {
        brokerProxy.publishMessage(runtime.getMessage(configuration.broker.baseTopic));
        brokerProxy.disconnect();
        delay(500);
        WLAN::disconnect(); 
    }
    String message = "\nDisconnected from WiFi, going to sleep for " + String(battery.getSleepTimeInSeconds()) + " seconds ...";
    PRINTLN_IF_DEBUG(message)
    ESP.deepSleep(battery.getSleepTimeInSeconds() * DEEP_SLEEP_ONE_SECOND); 
}

/**
 * main loop
 */
void loop() {
    MQTTServer::setData("temperature", String(bme.readTemperature()));
    MQTTServer::setData("humidity", String(bme.readHumidity()));
    MQTTServer::setData("pressure", String(bme.readPressure()));
    MQTTServer::setData("voltage", String(battery.measureVoltage()));

    if (WLAN::isConnected()) {
        brokerProxy.publishMessages(bme.getMessages());
        brokerProxy.publishMessages(battery.getMessages(configuration.broker.baseTopic));
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
/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Yaha station
 */

#define __DEBUG
#define __IRRIGATION
#define __BME
#define __SWITCH

#include <yahaserver.h>
#include <debug.h>
#include <message.h>

#ifdef __IRRIGATION
#include <irrigation.h>
Irrigation irrigation;
#endif

#ifdef __BME
#include <yahabme280.h>
#include "htmlweatherform.h"
YahaBME280 bme;
#endif

const uint8_t ACTIVATE_BME280_PIN = 14;
const uint32_t SERIAL_SPEED = 115200;
const char* STATION_NAME = "YahaIrrigation";

YahaServer server;


/**
 * Setup everything
 */
void setup() {
    INIT_SERIAL_IF_DEBUG(SERIAL_SPEED)
    server.setup(STATION_NAME);
    #ifdef __BME
    MQTTServer::addForm("/", "Weather", htmlWeatherForm);
    bme.activate(ACTIVATE_BME280_PIN);
    bme.setBaseTopic(server.brokerProxy.getBaseTopic());
    bme.init(0x76);
    #endif
    #ifdef __IRRIGATION
    MQTTServer::addForm("/irrigation", "Irrigation", irrigation.htmlForm);
    #endif
    #ifdef __SWITCH
    MQTTServer::addForm("/switch", "Switch", server.digitalSwitch.htmlForm);
    #endif

}

/**
 * main loop
 */
void loop() {
    #ifdef __BME
    float humidity = bme.readHumidity();
    MQTTServer::setData("temperature", String(bme.readTemperature()));
    MQTTServer::setData("humidity", String(bme.readHumidity()));
    MQTTServer::setData("pressure", String(bme.readPressure()));
    if (WLAN::isConnected()) {
        server.brokerProxy.publishMessages(bme.getMessages());
    }
    #endif

    #ifdef __IRRIGATION
    if (WLAN::isConnected()) {
         if (server.irrigation.doIrrigation(humidity, RTC::getWakeupAmount())) {
            server.brokerProxy.publishMessages(server.irrigation.getMessages(server.brokerProxy.getBaseTopic(), humidity));
            server.irrigation.runIrrigation(humidity);
            RTC::setWakeupAmount(0);
        }
    }
    #endif
    server.loop();

    if (server.battery.isBatteryMode()) {
        server.closeDown();
    } else {
        for (uint16_t i = 0; i < 5000; i++) {
            MQTTServer::handleClient();
            delay(10);
        }
    }
}

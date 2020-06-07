/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Yaha weather station based on ESP8266 and BME280
 */

#include <yahaserver.h>
#include <yahabme280.h>
#include <debug.h>
#include "htmlweatherform.h"

const uint8_t ACTIVATE_BME280_PIN = 14;
const uint32_t SERIAL_SPEED = 115200;
const char* STATION_NAME = "YahaWeather";

YahaBME280 bme;
YahaServer server;


/**
 * Setup everything
 */
void setup() {
    INIT_SERIAL_IF_DEBUG(SERIAL_SPEED)
    server.setup(STATION_NAME);
    MQTTServer::addForm("/", "Weather", htmlWeatherForm);

    bme.activate(ACTIVATE_BME280_PIN);
    bme.setBaseTopic(server.brokerProxy.getBaseTopic());
    bme.init(0x76);
}

/**
 * main loop
 */
void loop() {
    float humidity = bme.readHumidity();
    MQTTServer::setData("temperature", String(bme.readTemperature()));
    MQTTServer::setData("humidity", String(humidity));
    MQTTServer::setData("pressure", String(bme.readPressure()));
    if (WLAN::isConnected()) {
        server.brokerProxy.publishMessages(bme.getMessages());
    }
    server.loop();

    if (!RTC::isFastReset()) {
        server.closeDown();
    } else {
        for (uint16_t i = 0; i < 5000; i++) {
            MQTTServer::handleClient();
            delay(10);
        }
    }
}

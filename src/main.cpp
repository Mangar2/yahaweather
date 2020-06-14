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

#include <vector>
#include <debug.h>
#include <yahaserver.h>

#ifdef __BME
#include <yahabme280.h>
#include "htmlweatherform.h"
const uint16_t BME_I2C_ADDRESS = 0x76;
const uint8_t ACTIVATE_BME280_PIN = 14;
YahaBME280 bme(BME_I2C_ADDRESS, ACTIVATE_BME280_PIN);
#endif

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
    server.addDevice(&bme);
    MQTTServer::addForm("/", "Weather", htmlWeatherForm);
    #endif
    #ifdef __IRRIGATION
    MQTTServer::addForm("/irrigation", "Irrigation", server.irrigation.htmlForm);
    #endif
    #ifdef __SWITCH
    MQTTServer::addForm("/switch", "Switch", server.digitalSwitch.htmlForm);
    #endif

}

/**
 * main loop
 */
void loop() {

    #ifdef __IRRIGATION
    if (WLAN::isConnected()) {
         if (server.irrigation.doIrrigation()) {
            server.brokerProxy.publishMessages(server.irrigation.getMessages(server.brokerProxy.getBaseTopic()));
            server.irrigation.run();
            server.sendMessageToDevices("rtc/wakeupAmount", "0");
        }
    }
    #endif
    server.loop();

}

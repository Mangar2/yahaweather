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

#pragma once

#include <Arduino.h>
#include <wlan.h>
#include <brokerproxy.h>
#include <mqttserver.h>
#include <eepromaccess.h>
#include <rtc.h>
#include <runtime.h>
#include <battery.h>
#include <irrigation.h>
#include <switch.h>

class YahaServer {
public:
    /**
     * Setup everything
     * @param stationSSID name of the station set up, if WLAN connection is not possible
     */
    void setup(const String stationSSID = "YahaESP8266");

    /**
     * Close down and go to sleep
     */
    void closeDown();

    /**
     * loop action
     */
    void loop();

    /**
     * Updates the configuration, stores it to eeprom
     */
    static void updateConfig(std::map<String, String> config);

    /**
     * Initializes the eeprom, reads the configuration and initializes the objects
     */
    static void setupEEPROM();

    static Battery battery;
    static Runtime runtime;
    static BrokerProxy brokerProxy;
    static Irrigation irrigation;
    static Switch digitalSwitch;

private:

    struct Configuration {
        BrokerProxy::Configuration broker;
        WLAN::Configuration wlan;
        Battery::Configuration battery;
        Irrigation::Configuration irrigation;
    };

    static Configuration _config;

    static const uint16_t EEPROM_START_ADDR = 0;

};

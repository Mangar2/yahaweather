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

#include <ESP8266WiFi.h>        // Include the Wi-Fi library

// #define __DEBUG
#define __SOFT_AP   // Always add a access point additionally to the WLAN connection
// #define __BME
// #define __IRRIGATION
// #define __SWITCH
// #define __MOTION
// #define __BATTERY
// #define __RTC
// #define __RAIN

#include <vector>
#include <debug.h>

#ifdef __RTC
#include <rtc.h>
#endif

#ifdef __BATTERY
#include <battery.h>
#endif 

#include <yahaserver.h>

#ifdef __BME
#include <yahabme280.h>
const uint16_t BME_I2C_ADDRESS = 0x76;
const uint8_t ACTIVATE_BME280_PIN = 14;
#endif

#ifdef __RAIN
#include "digitalsensor.h"
const uint8_t RAIN_PIN = D6;
const uint8_t ACTIVATE_RAIN_PIN = D7;
#endif

#ifdef __IRRIGATION
#include "irrigation.h"
#endif

#ifdef __SWITCH
#include "switch.h"
#endif

#ifdef __MOTION
#include "motion.h"
#endif

#ifdef __SOFT_AP
#include "softap.h"
#endif

const uint32_t SERIAL_SPEED = 115200;
const char* AP_NAME = "YAHA_ESP_AP";

YahaServer server;

/**
 * Setup everything
 */
void setup() {
    Serial.begin(SERIAL_SPEED);
    delay(10);
    PRINTLN_IF_DEBUG("Setup started")
    #ifdef __BATTERY
    server.addDevice(new Battery());
    #endif
    #ifdef __RTC
    server.addDevice(new RTC());
    #endif
    #ifdef __RAIN
    server.addDevice(new DigitalSensor(RAIN_PIN, ACTIVATE_RAIN_PIN));
    #endif
    #ifdef __BME
    server.addDevice(new YahaBME280(BME_I2C_ADDRESS, ACTIVATE_BME280_PIN));
    #endif
    #ifdef __IRRIGATION
    server.addDevice(new Irrigation());
    #endif
    #ifdef __SWITCH
    server.addDevice(new Switch());
    #endif
    #ifdef __MOTION
    server.addDevice(new Motion());
    #endif
    #ifdef __SOFT_AP
    // AP must be created before connecting to WLAN. This is done by applying priority 1
    server.addDevice(new SoftAP(), 1);
    #endif
    server.setup(AP_NAME);
}

/**
 * main loop
 */
void loop() {
    server.loop();
}


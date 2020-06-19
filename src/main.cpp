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
// #define __BME
// #define __IRRIGATION
// #define __SWITCH
#define __MOTION

#include <vector>
#include <debug.h>
#include <rtc.h>
#include <battery.h>
#include <yahaserver.h>

#ifdef __BME
#include <yahabme280.h>
const uint16_t BME_I2C_ADDRESS = 0x77;
const uint8_t ACTIVATE_BME280_PIN = 14;
#endif

#ifdef __IRRIGATION
#include <irrigation.h>
#endif

#ifdef __SWITCH
#include <switch.h>
#endif

#ifdef __MOTION
#include <motion.h>
#endif

const uint32_t SERIAL_SPEED = 115200;
const char* STATION_NAME = "YahaIrrigation";

YahaServer server;


/**
 * Setup everything
 */
void setup() {
    INIT_SERIAL_IF_DEBUG(SERIAL_SPEED)
    server.addDevice(new Battery());
    server.addDevice(new RTC());
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
    server.setup(STATION_NAME);
}

/**
 * main loop
 */
void loop() {
    server.loop();
}
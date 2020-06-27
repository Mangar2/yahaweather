/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Provides a class to connect to WLAN
 */

#define __DEBUG
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <eepromaccess.h>
#include "wlan.h"

const char* WLAN::htmlForm = 
R"htmlwlan(
<form action="/" method="POST">
<label for="ssid">Wlan name</label>
<input type="text" id="ssid" name="wlan/ssid" placeholder="ssid..." [value]="wlan/ssid">
<label for="passwd">Wlan Password</label>
<input type="password" id="passwd" name="wlan/password" placeholder="password...">
<input type="submit" value="Submit">
</form>
)htmlwlan";

/**
 * Gets the configuration as key/value map
 */
std::map<String, String> WLAN::Configuration::get()
{
    std::map<String, String> result;
    result["wlan/ssid"] = ssid;
    result["wlan/password"] = password;
    return result;
}

/**
 * Sets the configuration from a key/value map
 * @param config configuration settings in a map
 */
void WLAN::Configuration::set(std::map<String, String> config)
{
    ssid = config["wlan/ssid"];
    password = config["wlan/password"];
    uuid = getUUID();
}

uint16_t WLAN::writeConfigToEEPROM(uint16_t EEPROMAddress) {
    EEPROMAccess::write(EEPROMAddress, (uint8_t*) &_config, sizeof(_config));
    return EEPROMAddress + sizeof(_config);
}

uint16_t WLAN::readConfigFromEEPROM(uint16_t EEPROMAddress) { 
    EEPROMAccess::read(EEPROMAddress, (uint8_t*) &_config, sizeof(_config));
    return EEPROMAddress + sizeof(_config);
}

/**
 * Checks if the WLAN connection is established
 */
bool WLAN::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

/**
 * connects to the wlan, if currently not connected
 */
void WLAN::reconnect() {
    if (!isConnected()) {
        connect();
    }
}

 bool WLAN::connect(const String& softAPssid) {
     bool result = connect();
     if (!result) {
         softAP(softAPssid);
         delay(2000);
     }
     return result;
}

void WLAN::disconnect() {
    WiFi.disconnect();
    delay(1);
    //WiFi.softAPdisconnect(wifioff)
}

bool WLAN::softAP(const String& ssid) {
    PRINTLN_IF_DEBUG("Sett up Wifi station in soft ap mode, IP: 192.168.4.1, ssid: " + ssid + " password: admin")
    
    bool station = WiFi.softAP(ssid, "admin");
    PRINTLN_VARIABLE_IF_DEBUG(station)
    return station;
}


bool WLAN::connect() {
    uint8_t tries = 0;
    bool isConnectedToWLAN = false;
    WiFi.forceSleepWake();
    delay(1);
    WiFi.persistent(false);
    PRINT_IF_DEBUG("Connect to WLAN, ssid: ")
    PRINT_IF_DEBUG(_config.ssid);
    PRINT_IF_DEBUG(" password: ")
    PRINTLN_IF_DEBUG(_config.password);
    WiFi.begin(_config.ssid, _config.password);
    while (tries < MAX_TRIES && WiFi.status() != WL_CONNECTED) {
        delay(200);
        PRINT_IF_DEBUG(".");
        tries++;
    }
    PRINTLN_IF_DEBUG()
    switch (WiFi.status()) {
        case WL_CONNECTED:
            isConnectedToWLAN = true;
            PRINT_IF_DEBUG("Connected with IP : ");
            PRINTLN_IF_DEBUG(WiFi.localIP());
            break;
        case WL_NO_SSID_AVAIL:
            PRINT_IF_DEBUG("SSID unknown: ");
            PRINTLN_IF_DEBUG(_config.ssid);
            break;
        case WL_CONNECT_FAILED: 
            PRINTLN_IF_DEBUG("Invalid password");
            break;
        case WL_DISCONNECTED:
            PRINTLN_IF_DEBUG("Module is not in station mode");
            break;
        case WL_IDLE_STATUS:
            PRINTLN_IF_DEBUG("Wi-Fi in process of changing between statuses");
            break;
        default:
            PRINTLN_IF_DEBUG("Undefined error code");
            break;
    }  
    return isConnectedToWLAN;
    
}

String WLAN::getLocalIP() {
    return WiFi.localIP().toString();
}
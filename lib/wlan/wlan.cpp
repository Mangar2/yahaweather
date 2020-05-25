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
#include "wlan.h"

String WLAN::_ssid;
String WLAN::_password;

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

 bool WLAN::connect(const WLANConfiguration& configuration) {
    _ssid = configuration.ssid;
    _password = configuration.password;
    return connect();
}

void WLAN::disconnect() {
    WiFi.disconnect();
}


bool WLAN::connect() {
    uint8_t tries = 0;
    bool isConnectedToWLAN = false;
    WiFi.forceSleepWake();
    WiFi.begin(_ssid, _password);
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
            PRINTLN_IF_DEBUG(_ssid);
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
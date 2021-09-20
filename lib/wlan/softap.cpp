/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Provides a class to connect to SoftAP
 */

#define __DEBUG
#include "eepromaccess.h"
#include "yahaserver.h"
#include "softap.h"
#include "ESP8266WebServer.h"
#include "runtime.h"
#include "debug.h"
#include "message.h"


const char* SoftAP::htmlForm = 
R"htmlap(
<form action="/" method="POST">
<label for="ssid">Access Point name (ssid)</label>
<input type="text" id="ssid" name="ap/ssid" placeholder="ssid..." [value]="ap/ssid">
<label for="passwd">Access Point Password</label>
<input type="password" id="passwd" name="ap/password" placeholder="password...">
<label for="ip">Access Point IP</label>
<input type="text" id="ip" name="ap/ip" placeholder="ip..." [value]="ap/ip">
<label for="gateway">Gateway</label>
<input type="text" id="gateway" name="ap/gateway" placeholder="gateway" [value]="ap/gateway">
<label for="subnet">Subnet mask</label>
<input type="text" id="subnet" name="ap/subnet" placeholder="gateway" [value]="ap/subnet">
<input type="submit" value="Submit">
</form>
)htmlap";

/**
 * Gets the configuration as key/value map
 */
std::map<String, String> SoftAP::Configuration::get()
{
    std::map<String, String> result;
    result["ap/ssid"] = ssid;
    result["ap/password"] = password;
    result["ap/ip"] = ip;
    result["ap/gateway"] = gateway;
    result["ap/subnet"] = subnet;
    return result;
}

/**
 * Sets the configuration from a key/value map
 * @param config configuration settings in a map
 */
void SoftAP::Configuration::set(std::map<String, String> config)
{
    if (config["ap/ssid"] != "") {
        ssid = config["ap/ssid"];
    }
    if (config["ap/password"] != "") {
        password = config["ap/password"];
    }
    if (config["ap/ip"] != "") {
        ip = config["ap/ip"];
    }
    gateway = config["ap/gateway"];
    subnet = config["ap/subnet"];
}

uint16_t SoftAP::writeConfigToEEPROM(uint16_t EEPROMAddress) {
    EEPROMAccess::write(EEPROMAddress, (uint8_t*) &_config, sizeof(_config));
    return EEPROMAddress + sizeof(_config);
}

uint16_t SoftAP::readConfigFromEEPROM(uint16_t EEPROMAddress) { 
    EEPROMAccess::read(EEPROMAddress, (uint8_t*) &_config, sizeof(_config));
    return EEPROMAddress + sizeof(_config);
}

void SoftAP::setup() {
    YahaServer::wlan.setAPAndStationMode();
    createAP();
}

bool SoftAP::createAP() {
    if (YahaServer::wlan.hasAP()) {
        YahaServer::wlan.apDisconnect();
    }
    YahaServer::wlan.softAPConfig(_config.ip, _config.gateway, _config.subnet);
    bool created = YahaServer::wlan.softAP(_config.ssid, _config.password);
    return created;
}

void apDisconnect() {
    YahaServer::wlan.apDisconnect();
}

String SoftAP::getLocalIP() {
    return YahaServer::wlan.getAPIP();
}
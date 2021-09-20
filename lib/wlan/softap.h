/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @documentation
 * Provides configuration for an access point
 */
#pragma once
#include <Arduino.h>
#include <map>
#include "idevice.h"
#include "staticstring.h"

class SoftAP : public IDevice {
public:

    struct Configuration {
        StaticString<32> ssid;
        StaticString<32> password;
        StaticString<17> ip;
        StaticString<17> gateway;
        StaticString<17> subnet;

        void clear() { 
            ssid = "yahasoftap"; 
            password = "yahaadmin"; 
            ip = "192.168.4.2";
            gateway = "192.168.4.4";
            subnet = "255.255.255.0";
        }

        /**
         * Gets the configuration as key/value map
         */
        std::map<String, String> get();

        /**
         * Sets the configuration from a key/value map
         * @param config configuration settings in a map
         */
        void set(std::map<String, String> config);
    };

    /**
     * Sets the configuration
     */
    virtual void setConfig(jsonObject_t& config) { 
        _config.set(config); 
    }

    /**
     * Gets the battery configuraiton
     */
    virtual jsonObject_t getConfig() { 
        return _config.get();
    }

    /**
     * Clears the configuration
     */
    void clear() { _config.clear(); }

    /**
     * Called on setup
     */
    virtual void setup();

    /**
     * Writes the configuration to EEPROM
     * @param EEPROMAddress EEPROM address to write to
     * @returns EEPROM address for the next device
     */
    virtual uint16_t writeConfigToEEPROM(uint16_t EEPROMAddress);

    /**
     * Reads configuration from EEPROM
     * @param EEPROMAddress EEPROM address to read from
     * @returns EEPROM address for the next device
     */
    virtual uint16_t readConfigFromEEPROM(uint16_t EEPROMAddress);

    /**
     * Gets an info about the matching html page
     */
    HtmlPageInfo getHtmlPage() { return HtmlPageInfo(htmlForm, "/", "Access Point"); }

    /**
     * Checks if the WLAN AP is active
     */
    static bool isActive();

    /**
     * Connects to wlan, creates the access point
    */
    bool createAP();

    /**
     * Disconnects from wlan
     */
    bool apDisconnect();

    /**
     * Gets the ip address of this device
     */
    static String getLocalIP();

    /**
     * Form to enter/change wlan settings
     */
    static const char* htmlForm;

private:


    Configuration _config;

};


/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @documentation
 * Provides a method to connect to WLAN
 */
#pragma once
#include <Arduino.h>
#include <map>
#include <debug.h>
#include <message.h>
#include <idevice.h>
#include "staticstring.h"



class WLAN : public IDevice {
public:

    struct Configuration {
        String getUUID() const { return "11896e60-6f3a-46ef-b718-839df2380de5"; }
        void initUUDI() { uuid = getUUID(); }
        bool isInitialized() const { return uuid == getUUID(); }
        StaticString<38> uuid;
        StaticString<32> ssid;
        StaticString<32> password;

        void clear() { ssid = ""; password = ""; }
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
     * Constructor
     */
    WLAN() : _hasAP(false) {}

    /**
     * Sets the configuration
     */
    virtual void setConfig(jsonObject_t& config) { 
        _config.set(config); 
    }

    /**
     * Gets the configuraiton
     */
    virtual jsonObject_t getConfig() { 
        return _config.get();
    }

    /**
     * Clears the configuration
     */
    void clear() { _config.clear(); }

    /**
     * Checks, if the WLAN configuration is initialized
     * @returns true, if initialized
     */
    bool isInitialized() { return _config.isInitialized(); }

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
    HtmlPageInfo getHtmlPage() { return HtmlPageInfo(htmlForm, "/", "WLan"); }

    /**
     * Checks if the WLAN connection is established
     */
    static bool isConnected();

    /**
     * connects to the wlan, if currently not connected
     */
    void reconnect();

    /**
     * Connects to wlan, creates a station, if WLAN configuration is invalid
     * @param softAPssid ssid of a station, if the WLAN connection is not available
    */
    bool connect(const String& softAPssid);

    /**
     * Sets the mode to simultaneous Access Point and station mode
     */
    void setAPAndStationMode();

    /**
     * Configures the soft AP
     * @param ip ip address of the soft AP
     * @param gateway gateway ip address
     * @param subnet subnet mask
     */
    void softAPConfig(String ip, String gateway, String subnet);

    /**
     * Creates a station used for configuration purpouse
     * @param ssid ssid of the station 
     * @param password password to log into the station
     */
    bool softAP(const String ssid = "Yaha Station", const String password = "yahaadmin");

    /**
     * @returns true, if a ap is active at present
     */
    bool hasAP() { return _hasAP; }

    /**
     * Closes the access point
     */ 
    bool apDisconnect();

    /**
     * Disconnects from wlan
     */
    void disconnect();

    /**
     * @returns ip address of the access point
     */
    static String getAPIP();

    /**
     * Gets the ip address of this device
     */
    static String getLocalIP();

    /**
     * Form to enter/change wlan settings
     */
    static const char* htmlForm;

private:
    static const uint8_t MAX_TRIES = 10 * 5;


    /**
     * Internal connect function
     * Tries to connect to wlan, prints error codes on failure
     */
    bool connect();

    bool _hasAP;

    Configuration _config;

};


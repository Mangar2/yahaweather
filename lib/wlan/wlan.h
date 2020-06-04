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
#include "debug.h"
#include "staticstring.h"



class WLAN {
public:

    struct Configuration {
        String getUUID() const { return "11896e60-6f3a-46ef-b718-839df2380de5"; }
        void initUUDI() { uuid = getUUID(); }
        bool isInitialized() const { return uuid == getUUID(); }
        StaticString<38> uuid;
        StaticString<32> ssid;
        StaticString<32> password;

        /**
         * Gets the configuration as key/value map
         */
        std::map<String, String> get()
        {
            std::map<String, String> result;
            result["ssid"] = ssid;
            result["password"] = password;
            return result;
        }

        /**
         * Sets the configuration from a key/value map
         * @param config configuration settings in a map
         */
        void set(std::map<String, String> config)
        {
            ssid = config["ssid"];
            password = config["password"];
            uuid = getUUID();
        }
    };

    /**
     * Checks if the WLAN connection is established
     */
    static bool isConnected();

    /**
     * connects to the wlan, if currently not connected
     */
    static void reconnect();

    /**
     * Connects to wlan, creates a station, if WLAN configuration is invalid
     * @param configuration WLAN configuraiton object with ssid and password
     * @param softAPssid ssid of a station, if the WLAN connection is not available
    */
    static bool connect(const Configuration& configuration, const String& softAPssid);

    /**
     * Creates a station used for configuration purpouse
     */
    static bool softAP(const String& ssid);

    /**
     * Disconnects from wlan
     */
    static void disconnect();

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
    static bool connect();

    static String _ssid;
    static String _password;

};


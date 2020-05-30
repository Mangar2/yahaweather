/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Provides a web server waiting for yaha mqtt commands
 */

#pragma once
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <map>
#include "json.h"

typedef void (*callback)(const JSON&);
typedef void (*setCallback)(const String&, const String&);

class MQTTServer {
public:

    /**
     * Starts the server
     */
    static void begin(uint32_t port);
    
    /**
     * Enables client processing
     */
    static void handleClient();

    /**
     * Registeres a function called in an on publish request
     */
    static void registerOnPublishFunction(callback func);

    /**
     * Registeres a function to set the WLAN
     */
    static void registerSetWLAN(setCallback setWlanFunc) { _onWLANFunc = setWlanFunc; }

    /**
     * Registeres a function to set broker connection information
     */
    static void registerSetBroker(setCallback setBrokerFunc) { _onBrokerFunc = setBrokerFunc; }

    /**
     * Registeres a function to set client configuration
     */
    static void registerSetClient(setCallback setClientFunc) { _onClientFunc = setClientFunc; }

    /**
     * Sets data for forms
     */
    static void setData(const String& key, const String& value) { data[key] = value; }

private:
    MQTTServer() {
    }

    /**
     * Handles a http PUT publish command, replies with PUBACK and returns the id in the header to 
     * identify the right mesage
     */
    static void onPublish();

    /**
     * Handles a http POST wlan command to receive form data.
     */
    static void onWlan();

    /**
     * Handles a http POST broker command to receive form data.
     */
    static void onBroker();

    /**
     * Handles a http POST client command to receive form data.
     */
    static void onClient();


    /**
     * routes the rest messages to a matching function
     */
    static void restServerRouting();

    /**
     * Replaces values in a form
     */
    static String replaceFormValues(const String& form); 

    static ESP8266WebServer* _httpServer;
    static callback _onPublishFunc;
    static setCallback _onWLANFunc;
    static setCallback _onBrokerFunc;
    static setCallback _onClientFunc;
    static std::map<String, String> data;
};

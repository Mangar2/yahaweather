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
typedef std::function<void()> THandlerFunction;

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
     * Sets data for forms
     */
    static void setData(const String& key, const String& value) { _data[key] = value; }

    /**
     * Gets the value of an Argument handed over by a http GET or POST call
     * @param argName name of the argument
     */
    static String getArgValue(const String& argName);

    /**
     * Registers a function beeing called on http/https request
     * @param uri link the function is registered to
     * @param handler function to be called
     */
    static void on(const String& uri, THandlerFunction handler);

    /**
     * Adds a form to the mqtt server
     * @param uri link to access the form
     * @param form html form including surrounding container - div
     */
    static void addForm(const String& uri, const String& form);

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
    static std::map<String, String> _data;
    static std::map<String, String> _forms;
};

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
#include <message.h>
#include "json.h"

typedef std::function<void(std::map<String, String>&)> TOnUpdateFunction;
typedef std::function<void()> THandlerFunction;

class MQTTServer {
public:

    /**
     * Starts the server
     */
    static void begin(uint32_t port = 80);
    
    /**
     * Enables client processing
     */
    static void handleClient();

    /**
     * Registeres a function called in an on publish request
     */
    static void registerOnUpdateFunction(TOnUpdateFunction handler);

    /**
     * Sets data for forms
     */
    static void setData(const String& key, const String& value) { _data[key] = value; }
    static void setData(std::map<String, String> configuration) { 
        for (auto const& x: configuration) {
            _data[x.first] = x.second;
        }
    }

    /**
     * Registers a function beeing called on http/https request
     * @param uri link the function is registered to
     */
    static void on(const String& uri);

    /**
     * Adds a form to the mqtt server
     * @param uri link to access the form
     * @param name Name of the form shown in the menu
     * @param form html form including surrounding container - div
     */
    static void addForm(const String& uri, const String& name, const String& form);

    /**
     * Gets a key/value store of all configuration data
     */
    static std::map<String, String> getConfigMap() { return _data; }

    /**
     * Gets an array of messages to be send to the broker
     * @param baseTopic start string of the topic
     * @returns a list of all available values beside password as messages
     */
    static Messages_t getMessages(const String& baseTopic);

    /**
     * Returns true, if settings has been changed
     */
    static bool isChanged() { return _isChanged; }

private:
    MQTTServer() {
    }

    /**
     * Handles a http PUT publish command, replies with PUBACK and returns the id in the header to 
     * identify the right mesage
     */
    static void onPublish();

    /**
     * routes the rest messages to a matching function
     */
    static void restServerRouting();

    /**
     * Replaces values in a form
     */
    static String replaceFormValues(const String& form); 

    /**
     * Creates a form
     */
    static String createForm(const String& uri);

    /**
     * Creates the navigation menu
     * @param activeLink relative link of the currently active page
     */
    static String createTopNav(const String& activeLink);

    static ESP8266WebServer* _httpServer;
    static TOnUpdateFunction _onUpdateFunction;
    static std::map<String, String> _data;
    static std::map<String, String> _formNames;
    static std::map<String, String> _forms;
    static bool _isChanged;
};

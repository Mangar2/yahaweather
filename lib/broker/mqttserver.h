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
#include "json.h"

typedef void (*callback)(const JSON&);

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

private:
    MQTTServer() {
    }

    /**
     * Handles a http POST publish command, replies with PUBACK and returns the id in the header to 
     * identify the right mesage
     */
    static void onPublish();

    /**
     * routes the rest messages to a matching function
     */
    static void restServerRouting();

    /**
     * sends back a not found error
     */
    static void handleNotFound();

    static ESP8266WebServer* _httpServer;
    static callback _onPublishFunc;
};

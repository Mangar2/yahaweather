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

class MQTTServer {
public:
    MQTTServer(uint32_t port) {
        _httpServer = new ESP8266WebServer(port);
    }

    /**
     * Starts the server
     */
    static void begin();

    /**
     * Handles a http POST publish command, replies with PUBACK and returns the id in the header to 
     * identify the right mesage
     */
    static void onPublish();

    /**
     * Enables client processing
     */
    static void handleClient();

    /**
     * routes the rest messages to a matching function
     */
    void restServerRouting();

private:

    /**
     * sends back a not found error
     */
    static void handleNotFound();

    static ESP8266WebServer* _httpServer;
};

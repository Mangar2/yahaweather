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

#define __DEBUG
#include "debug.h"
#include "mqttserver.h"

ESP8266WebServer* MQTTServer::_httpServer;
    
void MQTTServer::onPublish() {
    String postBody = _httpServer->arg("plain");
    PRINTLN_IF_DEBUG("Received POST publish command body:");
    PRINTLN_IF_DEBUG(postBody);

    String id = _httpServer->header("id");
    PRINTLN_VARIABLE_IF_DEBUG(id)

    _httpServer->sendHeader("id", id);
    _httpServer->send(200, "text/html", "PUBACK");
    delay(10);
}

void MQTTServer::handleClient() {
    _httpServer->handleClient();
}

void MQTTServer::handleNotFound() {
    String message = "Resource not found\n";
    message += "URI: ";
    message += _httpServer->uri();
    message += "\n";
    _httpServer->send(404, "text/plain", message);
}

void MQTTServer::restServerRouting() {
    _httpServer->on("/", HTTP_GET, []() {
        _httpServer->send(200, "text/html", "mqtt server ready ...");
    });
    _httpServer->on("/publish", HTTP_PUT, onPublish);
    _httpServer->onNotFound(handleNotFound);
}

void MQTTServer::begin() {
    const char* headers[] = {"id"};
    _httpServer->collectHeaders(headers, sizeof(headers)/ sizeof(headers[0]));
    _httpServer->begin();
}


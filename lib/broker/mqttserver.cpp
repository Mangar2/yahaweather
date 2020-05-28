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
#include "htmlpage.h"

ESP8266WebServer* MQTTServer::_httpServer;
callback MQTTServer::_onPublishFunc = 0;
    
void MQTTServer::onPublish() {
    String postBody = _httpServer->arg("plain");
    PRINTLN_IF_DEBUG("Received PUT publish command body:");
    PRINTLN_IF_DEBUG(postBody);
    JSON json(postBody);
    if (_onPublishFunc != 0) {
        _onPublishFunc(json);
    }

    String id = _httpServer->header("id");
    PRINTLN_VARIABLE_IF_DEBUG(id)

    _httpServer->sendHeader("id", id);
    _httpServer->send(200, "text/html", "PUBACK");
    delay(10);
}

void MQTTServer::onPost() {
    String postBody = _httpServer->arg("basetopic");
    PRINTLN_IF_DEBUG("Received POST publish command body:");
    PRINTLN_IF_DEBUG(postBody);
    JSON json(postBody);
    if (_onPublishFunc != 0) {
        _onPublishFunc(json);
    }

    _httpServer->send(200, "text/html", "Values stored");
    delay(10);
}

void MQTTServer::handleClient() {
    _httpServer->handleClient();
}

void MQTTServer::registerOnPublishFunction(callback func) {
    _onPublishFunc = func;
}

void MQTTServer::restServerRouting() {
    PRINTLN_IF_DEBUG("REST SERVER ROUTING")
    _httpServer->on("/", HTTP_GET, []() {
        PRINTLN_IF_DEBUG("HTTP GET")
        _httpServer->sendHeader("Accept", "application/json");
        _httpServer->send(200, "text/html", htmlPage);
    });
    _httpServer->on("/publish", HTTP_PUT, onPublish);
    _httpServer->on("/", HTTP_POST, onPost);
    _httpServer->onNotFound([]() {
        String message = "Resource not found\n URI: " + _httpServer->uri() + "\n";
        PRINTLN_VARIABLE_IF_DEBUG(message)
        _httpServer->send(404, "text/plain", message);
    });
}

void MQTTServer::begin(uint32_t port) {
    _httpServer = new ESP8266WebServer(port);
    restServerRouting();
    const char* headers[] = {"id"};
    _httpServer->collectHeaders(headers, sizeof(headers)/ sizeof(headers[0]));
    _httpServer->begin();
}


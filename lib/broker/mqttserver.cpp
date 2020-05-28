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
#include "htmlpages.h"

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

void MQTTServer::onWlan() {
    String ssid = _httpServer->arg("ssid");
    String password = _httpServer->arg("password");
    PRINTLN_IF_DEBUG(ssid);
    PRINTLN_IF_DEBUG(password);

    _httpServer->send(200, "text/html", htmlPage + "Values stored</body></html>");
    delay(10);
}

void MQTTServer::onBroker() {
    String host = _httpServer->arg("brokerhost");
    String port = _httpServer->arg("brokerport");
    PRINTLN_IF_DEBUG(host);
    PRINTLN_IF_DEBUG(port);

    _httpServer->send(200, "text/html", htmlPage + "Values stored</body></html>");
    delay(10);
}

void MQTTServer::onClient() {
    String clientName = _httpServer->arg("clientname");
    String baseTopic = _httpServer->arg("basetopic");
    PRINTLN_IF_DEBUG(clientName);
    PRINTLN_IF_DEBUG(baseTopic);

    _httpServer->send(200, "text/html", htmlPage + "Values stored</body></html>");
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
        _httpServer->send(200, "text/html", htmlPage + htmlWeatherForm);
    });
    _httpServer->on("/wlan", HTTP_GET, []() {
        _httpServer->send(200, "text/html", htmlPage + htmlWLANForm);
    });
    _httpServer->on("/broker", HTTP_GET, []() {
        _httpServer->send(200, "text/html", htmlPage + htmlBrokerForm);
    });
    _httpServer->on("/client", HTTP_GET, []() {
        _httpServer->send(200, "text/html", htmlPage + htmlClientForm);
    });
    _httpServer->on("/publish", HTTP_PUT, onPublish);
    _httpServer->on("/wlan", HTTP_POST, onWlan);
    _httpServer->on("/broker", HTTP_POST, onBroker);
    _httpServer->on("/client", HTTP_POST, onClient);
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


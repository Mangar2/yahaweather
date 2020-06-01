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

ESP8266WebServer* MQTTServer::_httpServer = 0;
TOnUpdateFunction MQTTServer::_onUpdateFunction = 0;
std::map<String, String> MQTTServer::_data;
    
void MQTTServer::onPublish() {
    String postBody = _httpServer->arg("plain");
    PRINTLN_IF_DEBUG("Received PUT publish command body:");
    PRINTLN_IF_DEBUG(postBody);
    JSON json(postBody);
    const String topic = json.getElement("topic");
    if (topic == "$SYS/outdoor/ESP8266/weather/broker/set") {
        setData("brokerHost", json.getElement("value.brokerHost"));
        setData("brokerPort", json.getElement("value.brokerPort"));
        setData("clientName", json.getElement("value.clientName"));
        setData("baseTopic", json.getElement("value.baseTopic"));
    }
    if (topic == "$SYS/outdoor/ESP8266/weather/wlan/set") {
        setData("ssid", json.getElement("value.ssid"));
        setData("password", json.getElement("value.password"));
    }
    _onUpdateFunction(_data);
    String id = _httpServer->header("id");
    PRINTLN_VARIABLE_IF_DEBUG(id)

    _httpServer->sendHeader("id", id);
    _httpServer->send(200, "text/html", "PUBACK");
    delay(10);
}


String MQTTServer::replaceFormValues(const String& form) {
    String result = form;
    for (auto const& x: _data) {
        String searchString = "[value]=\"" + x.first + "\"";
        result.replace(searchString, "value=\"" + x.second + "\"");
    }
    return result;
}

String setActiveNav(const String& nav, const String& link) {
    String result = nav;
    const String stringToReplace = "href=\"" + link + "\"";
    const String replaceBy = "class=\"active\" " + stringToReplace;
    result.replace(stringToReplace, replaceBy);
    return result;
}

void MQTTServer::on(const String& uri, THandlerFunction handler) {
    _httpServer->on(uri, HTTP_POST, [uri, handler]() {
        PRINTLN_VARIABLE_IF_DEBUG(uri)
        for (uint16_t i = 0; i < _httpServer->args(); i++) {
            _data[_httpServer->argName(i)] = _httpServer->arg(i);
        }
        handler();
        String topNav = setActiveNav(htmlTopNav, uri);
        // $SYS/ESP8266/weather
        _httpServer->send(200, "text/html", htmlPage + topNav + "Values stored</body></html>");
    });
}

void MQTTServer::addForm(const String& uri, const String& form) {
    _httpServer->on(uri, HTTP_GET, [uri, form]() {
        String bodyForm = "<div class=\"container\">" + replaceFormValues(form) + "</div></body></html>";
        String topNav = setActiveNav(htmlTopNav, uri);
        _httpServer->send(200, "text/html", htmlPage + topNav + bodyForm);
    });

}

void MQTTServer::handleClient() {
    _httpServer->handleClient();
}

void MQTTServer::registerOnUpdateFunction(TOnUpdateFunction handler) {
    _onUpdateFunction = handler;
}

void MQTTServer::restServerRouting() {
    PRINTLN_IF_DEBUG("REST SERVER ROUTING")
    addForm("/broker", htmlBrokerForm);
    addForm("/client", htmlClientForm);
    _httpServer->on("/publish", HTTP_PUT, onPublish);
    on("/wlan", []() {
        _onUpdateFunction(_data);
    });
    on("/broker", []() {
        _onUpdateFunction(_data);
    });
    on("/client", []() {
        _onUpdateFunction(_data);
    });
    on("/battery", []() {
        _onUpdateFunction(_data);
    });

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


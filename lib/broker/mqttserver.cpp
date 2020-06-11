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
std::map<String, String> MQTTServer::_forms;
bool MQTTServer::_isChanged;
    
void MQTTServer::onPublish() {
    String postBody = _httpServer->arg("plain");
    PRINTLN_IF_DEBUG("Received PUT publish command body:");
    PRINTLN_IF_DEBUG(postBody);
    JSON json(postBody);
    String topic = json.getElement("topic");
    String value = json.getElement("value");
    topic.replace("/set", "");
    uint16_t lastChunkStart = topic.lastIndexOf("/");
    String propertyName = topic.substring(lastChunkStart + 1);
    setData(propertyName, value);
    _onUpdateFunction(_data);
    String id = _httpServer->header("id");
    _isChanged = true;
    PRINTLN_VARIABLE_IF_DEBUG(id)

    _httpServer->sendHeader("id", id);
    _httpServer->send(200, "text/html", "PUBACK");
    delay(10);
}


String MQTTServer::replaceFormValues(const String& form) {
    String result = form;
    for (auto const& x: _data) {
        String valueString = "[value]=\"" + x.first + "\"";
        String checkedString = "[checked]=\"" + x.first + "\"";
        result.replace(valueString, "value=\"" + x.second + "\"");
        result.replace(checkedString, x.second == "on" ? "checked=\"checked\"" : "");
    }
    return result;
}

String MQTTServer::createTopNav(const String& activeLink) {
    String topNav = "<div class=\"topnav\">";
    for (auto const& form: _forms) {
        topNav += "<a";
        if (form.first == activeLink) {
            topNav += " class=\"active\"";
        }
        topNav += " href=\"";
        topNav += form.first;
        topNav += "\">";
        topNav += form.second;
        topNav += "</a>";
    }
    topNav += "</div>";
    return topNav;
}

void MQTTServer::on(const String& uri) {
    THandlerFunction handler = []() { _onUpdateFunction(_data); };
    _httpServer->on(uri, HTTP_POST, [uri, handler]() {
        PRINTLN_VARIABLE_IF_DEBUG(uri)
        _isChanged = true;
        for (uint16_t i = 0; i < _httpServer->args(); i++) {
            PRINT_VARIABLE_IF_DEBUG(_httpServer->argName(i))
            PRINT_IF_DEBUG(" = ")
            PRINTLN_VARIABLE_IF_DEBUG(_httpServer->arg(i))
            _data[_httpServer->argName(i)] = _httpServer->arg(i);
        }
        handler();
        String topNav = createTopNav(uri);
        // $SYS/ESP8266/weather
        _httpServer->send(200, "text/html", htmlPage + topNav + "Values stored</body></html>");
    });
}

void MQTTServer::addForm(const String& uri, const String& name, const String& form) {
    _forms[uri] = name;
    on(uri);
    _httpServer->on(uri, HTTP_GET, [uri, form]() {
        String bodyForm = "<div class=\"container\">" + replaceFormValues(form) + "</div></body></html>";
        String topNav = createTopNav(uri);
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
    addForm("/broker", "Broker", htmlBrokerForm);
    addForm("/client", "Client", htmlClientForm);
    _httpServer->on("/publish", HTTP_PUT, onPublish);

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


Messages_t MQTTServer::getMessages(const String& baseTopic) {
    Messages_t result;
    for (auto const& property: _data) {
        String lowerCasePropertyName = property.first;
        lowerCasePropertyName.toLowerCase();
        if (lowerCasePropertyName == "password") {
            continue;
        }
        const Message propertyMessage(baseTopic + "/" + property.first, property.second, "info from ESP8266");
        result.push_back(propertyMessage);
    }
    return result;
}


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
#include "json.h"

ESP8266WebServer* MQTTServer::_httpServer = 0;
TOnUpdateFunction MQTTServer::_onUpdateFunction = 0;
std::map<String, String> MQTTServer::_data;
std::map<String, String> MQTTServer::_forms;
std::map<String, String> MQTTServer::_formNames;
bool MQTTServer::_isChanged;
    
void MQTTServer::onPublish() {
    String postBody = _httpServer->arg("plain");
    PRINTLN_IF_DEBUG("Received PUT publish command body:");
    PRINTLN_IF_DEBUG(postBody);
    JSON json(postBody);
    String topic = json.getElement("message.topic");
    PRINTLN_VARIABLE_IF_DEBUG(topic)
    String value = json.getElement("message.value");
    PRINTLN_VARIABLE_IF_DEBUG(value)
    topic.replace("/set", "");
    uint16_t lastChunkStart = topic.lastIndexOf("/");
    uint16_t secondLastChungStart = topic.lastIndexOf("/", lastChunkStart - 1);
    String propertyName = topic.substring(secondLastChungStart + 1);
    setData(propertyName, value);
    _onUpdateFunction(_data);
    for (uint16_t i = 0; i < _httpServer->headers(); i++) {
        PRINT_IF_DEBUG(_httpServer->headerName(i))
        PRINT_IF_DEBUG("=")
        PRINTLN_IF_DEBUG(_httpServer->header(i))
    }
    String packetid = _httpServer->header("packetid");
    setChanged(true);
    PRINTLN_VARIABLE_IF_DEBUG(packetid)

    _httpServer->sendHeader("packetid", packetid);
    _httpServer->sendHeader("packet", "puback");
    _httpServer->send(204, "text/plain", "");
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
    for (auto const& form: _formNames) {
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

String MQTTServer::createForm(const String& uri) {
    String form = _forms[uri];
    String bodyForm = "<div class=\"container\">" + replaceFormValues(form) + "</div></body></html>";
    String topNav = createTopNav(uri);
    return htmlPage + topNav + bodyForm;
}

void MQTTServer::on(const String& uri) {
    THandlerFunction handler = []() { _onUpdateFunction(_data); };
    _httpServer->on(uri, HTTP_POST, [uri, handler]() {
        PRINTLN_VARIABLE_IF_DEBUG(uri)
        setChanged(true);
        for (uint16_t i = 0; i < _httpServer->args(); i++) {
            PRINT_VARIABLE_IF_DEBUG(_httpServer->argName(i))
            PRINT_IF_DEBUG(" = ")
            PRINTLN_VARIABLE_IF_DEBUG(_httpServer->arg(i))
            bool isPlainArgumentList = _httpServer->argName(i) == "plain";
            if (!isPlainArgumentList) {
                _data[_httpServer->argName(i)] = _httpServer->arg(i);
            }
        }
        handler();
        String form = createForm(uri);
        _httpServer->send(200, "text/html", form);
    });
}



void MQTTServer::addForm(const String& uri, const String& name, const String& form) {
    _formNames[uri] = name;
    _forms[uri] = form;
    on(uri);
    _httpServer->on(uri, HTTP_GET, [uri]() {
        String filledForm = createForm(uri);
        _httpServer->send(200, "text/html", filledForm);
    });

}

void MQTTServer::handleClient() {
    _httpServer->handleClient();
}

void MQTTServer::registerOnUpdateFunction(TOnUpdateFunction handler) {
    _onUpdateFunction = handler;
}

void MQTTServer::restServerRouting() {
    _httpServer->on("/publish", HTTP_PUT, onPublish);
    _httpServer->on("/css.css", HTTP_GET, []() {
        _httpServer->sendHeader("Cache-Control", "max-age=3600");
        _httpServer->send(200, "text/css", cssFile);
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
    // headers are not automatically collected
    // All headers needed must be listed here in the header list to be "collected"
    const char* headers[] = {"packetid", "dup", "version"};
    _httpServer->collectHeaders(headers, sizeof(headers)/ sizeof(headers[0]));
    _httpServer->begin();
}


Messages_t MQTTServer::getMessages(const String& baseTopic) {
    Messages_t result;
    for (auto const& property: _data) {
        String lowerCasePropertyName = property.first;
        lowerCasePropertyName.toLowerCase();
        if (lowerCasePropertyName.endsWith("password")) {
            continue;
        }
        const Message propertyMessage(baseTopic + "/" + property.first, property.second, "info from ESP8266");
        result.push_back(propertyMessage);
    }
    return result;
}


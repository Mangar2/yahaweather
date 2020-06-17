/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Provides a class to connect to WLAN
 */

#define __DEBUG
#include <Arduino.h>
#include <debug.h>
#include <eepromaccess.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <map>
#include "brokerproxy.h"
#include "json.h"


BrokerProxy::Configuration::Configuration() {
    brokerHost = "192.168.1.1";
    brokerPort = "8183";
    clientName = "ESP8266/Weather/Station";
    baseTopic = "outdoor/garden/main/weather"; 
    subscribeTo = "outdoor/garden/main/weather/+/set";
}

std::map<String, String> BrokerProxy::Configuration::get()
{
    std::map<String, String> result;
    result["broker/host"] = brokerHost;
    result["broker/port"] = brokerPort;
    result["broker/clientName"] = clientName;
    result["broker/baseTopic"] = baseTopic;
    result["broker/subscribeTo"] = subscribeTo;
    return result;
}

void BrokerProxy::Configuration::set(jsonObject_t& config)
{
    brokerHost = config["broker/host"];
    brokerPort = config["broker/port"];
    clientName = config["broker/clientName"];
    baseTopic = config["broker/baseTopic"];
    subscribeTo = config["broker/subscribeTo"];
}

const char* BrokerProxy::htmlForm = R"htmlform(
    <form action="/broker" method="POST">
    <label for="brokerhost">Broker host</label>
    <input type="text" id="brokerhost" name="broker/host" placeholder="Broker host..." [value]="broker/host">
    <label for="brokerport">Broker port</label>
    <input type="number" id="brokerport" name="broker/port" [value]="broker/port">
    <label for="clientname">Client name</label>
    <input type="text" id="clientname" name="broker/clientName" [value]="broker/clientName">
    <label for="basetopic">Base topic</label>
    <input type="text" id="basetopic" name="broker/baseTopic" [value]="broker/baseTopic">
    <label for="subscribeto">Subscribe topic</label>
    <input type="text" id="subscribeto" name="broker/subscribeTo" [value]="broker/subscribeTo">
    <input type="submit" value="Submit">
    </form>
    )htmlform";

uint16_t BrokerProxy::writeConfigToEEPROM(uint16_t EEPROMAddress) {
    EEPROMAccess::write(EEPROMAddress, (uint8_t*) &_config, sizeof(_config));
    return EEPROMAddress + sizeof(_config);
}

uint16_t BrokerProxy::readConfigFromEEPROM(uint16_t EEPROMAddress) { 
    EEPROMAccess::read(EEPROMAddress, (uint8_t*) &_config, sizeof(_config));
    return EEPROMAddress + sizeof(_config);
}

void BrokerProxy::sendToServer(String urlWithoutHost, String jsonBody, headers_t headers) {
    WiFiClient client;
    HTTPClient http;
    String url = String("http://") + _config.brokerHost + ":" + _config.brokerPort + urlWithoutHost;
    
    PRINT_IF_DEBUG(url);
    PRINT_IF_DEBUG(" ");
    PRINT_VARIABLE_IF_DEBUG(jsonBody);
    
    delay(5);
    
    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("cache-control", "no-cache");
    for (auto const& header: headers) {
        http.addHeader(header.first, header.second);
    }

    IF_DEBUG(uint16_t httpCode =) http.PUT(jsonBody);
    PRINT_VARIABLE_IF_DEBUG(httpCode);
    IF_DEBUG(http.writeToStream(&Serial);)
    PRINTLN_IF_DEBUG("");
    http.end();
    delay(10);
}

void BrokerProxy::connect(const String& port) {
    String host = WLAN::getLocalIP();
    String body = "{" + 
        jsonStringProperty("clientId", _config.clientName) + "," + 
        jsonStringProperty("clean", "false") + "," + 
        jsonStringProperty("host", host) + "," + 
        jsonStringProperty("port", port) + "," +
        jsonStringProperty("clean", "false") + "," + 
        jsonStringProperty("keepAlive", "100") + "}";
    String urlWithoutHost = "/connect";
    sendToServer(urlWithoutHost, body);
    subscribe(_config.subscribeTo, 1);
}

void BrokerProxy::disconnect() {
    String body = "{" + jsonStringProperty("clientId", _config.clientName) + "}";
    String urlWithoutHost = "/disconnect";
    sendToServer(urlWithoutHost, body);
}

void BrokerProxy::subscribe(String topic, uint8_t qos) {
    String body = "{" + 
        jsonStringProperty("clientId", _config.clientName) + "," + 
        jsonObjectProperty("subscribe", "{" + 
            jsonStringProperty("QoS", String(qos)) + "," + 
            jsonStringProperty("topics", topic) + "}")
        + "}";

    String bodyV1 = "{" +
        jsonStringProperty("clientId", _config.clientName) + "," + 
        jsonObjectProperty("subscribe", "{" + 
            jsonStringProperty(topic, String(qos))
        ) + "}";
        
    String urlWithoutHost = "/subscribe";
    sendToServer(urlWithoutHost, body);
}

void BrokerProxy::publishMessage(const Message& message, bool retain) {
    String body = message.toPublishString();
    String urlWithoutHost = "/publish";
    headers_t headers;
    headers["qos"] = "0";
    headers["version"] = "1.0";
    headers["retain"] = retain ? "1" : "0";
    sendToServer(urlWithoutHost, body, headers);
}

void BrokerProxy::publishMessages(const Messages_t& messages, bool retain) {
    for (auto const& message: messages) {
        publishMessage(message, retain);
    }
}

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
#include "debug.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include "brokerproxy.h"
#include "json.h"

BrokerProxy::Configuration::Configuration() {
    brokerHost = "192.168.1.1";
    brokerPort = "8183";
    clientName = "ESP2688/Weather/Station";
    baseTopic = "outdoor/garden/main/weather"; 
    subscribeTo = "$SYS/ESP8266/weather";
}

/**
 * Gets the configuration as key/value map
 */
std::map<String, String> BrokerProxy::Configuration::get()
{
    std::map<String, String> result;
    result["brokerHost"] = brokerHost;
    result["brokerPort"] = brokerPort;
    result["clientName"] = clientName;
    result["baseTopic"] = baseTopic;
    result["subscribeTo"] = subscribeTo;
    return result;
}

/**
 * Sets the configuration from a key/value map
 * @param config configuration settings in a map
 */
void BrokerProxy::Configuration::set(std::map<String, String> config)
{
    brokerHost = config["brokerHost"];
    brokerPort = config["brokerPort"];
    clientName = config["clientName"];
    baseTopic = config["baseTopic"];
    subscribeTo = config["subscribeTo"];
}

void BrokerProxy::sendToServer(String urlWithoutHost, String jsonBody, String QoS) {
    WiFiClient client;
    HTTPClient http;
    uint16_t httpCode;
    String url = String("http://") + _config.brokerHost + ":" + _config.brokerPort + urlWithoutHost;
    
    PRINT_IF_DEBUG(url);
    PRINT_IF_DEBUG(" ");
    PRINT_VARIABLE_IF_DEBUG(jsonBody);
    PRINT_VARIABLE_IF_DEBUG(QoS);
    
    delay(5);
    
    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("cache-control", "no-cache");
    if (QoS != "") {
        http.addHeader("QoS", QoS);
    }

    httpCode = http.PUT(jsonBody);
    PRINT_VARIABLE_IF_DEBUG(httpCode);
    IF_DEBUG(http.writeToStream(&Serial);)
    PRINTLN_IF_DEBUG("");
    http.end();
    delay(10);
}

void BrokerProxy::connect() {
    String body = "{" + 
        jsonStringProperty("clientId", _config.clientName) + "," + 
        jsonStringProperty("clean", "false") + "," + 
        jsonStringProperty("host", _IPAddress) + "," + 
        jsonStringProperty("port", _port) + "}";
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
        
    String urlWithoutHost = "/subscribe";
    sendToServer(urlWithoutHost, body);
}

void BrokerProxy::publishMessage(const Message& message) {
    String body = message.toPublishString();
    String urlWithoutHost = "/publish";
    sendToServer(urlWithoutHost, body, "0");
}

void BrokerProxy::publishMessages(const Messages_t& messages) {
    for (auto const& message: messages) {
        publishMessage(message);
    }
}

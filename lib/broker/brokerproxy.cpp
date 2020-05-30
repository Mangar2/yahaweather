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

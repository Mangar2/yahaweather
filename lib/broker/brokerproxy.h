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

#pragma once

#include <Arduino.h>
#include <map>
#include "staticstring.h"
#include "message.h"
#include "wlan.h"

typedef std::map<String, String> headers_t;

class BrokerProxy {
public:

    struct Configuration {
        StaticString<32> brokerHost;
        StaticString<8> brokerPort;
        StaticString<24> clientName;
        StaticString<64> baseTopic;
        StaticString<64> subscribeTo;

        Configuration();

        /**
         * Gets the configuration as key/value map
         */
        std::map<String, String> get();

        /**
         * Sets the configuration from a key/value map
         * @param config configuration settings in a map
         */
        void set(std::map<String, String> config);
    };

    BrokerProxy() {};
    
    /**
     * Sets the configuration
     */
    void setConfiguration(const Configuration& config) {
        _config = config;
    }

    /**
     * Gets the configuration
     */
    Configuration& getConfiguration() { return _config; }

    /**
     * Connects to the yaha "near-mqtt" broker
     */
    void connect(const String& port = "80");

    /**
     * Disconnects from the broker
     */
    void disconnect();

    /**
     * Subscribes a topic
     * @param topic topic to subscribe
     * @param qos subscribe quality of service
     */
    void subscribe(String topic, uint8_t qos);

    /**
     * Publishes a message to the broker
     * @param message message to publish
     * @param retain if true, the broker will retain the message
     */
    void publishMessage(const Message& message, bool retain = false);

    /**
     * Publishes several messages to the broker
     * @param message messages to publish
     * @param retain if true, the broker will retain the messages
     */
    void publishMessages(const Messages_t& messages, bool retain = false);

    /**
     * Gets the base topic for sending messages to the broker
     */
    String getBaseTopic() { return _config.baseTopic; }

private:
    /**
     * Sends an info to the mqtt broker
     * @param urlWithoutHost url 
     * @param jsonBody body of the message in json format
     * @param headers list of headers
     */
    void sendToServer(String urlWithoutHost, String jsonBody, headers_t headers = headers_t()); 

    Configuration _config;
    String _IPAddress;
    String _port;

};
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
#include "staticstring.h"
#include "message.h"

struct BrokerConfiguration {
    StaticString<32> brokerHost;
    StaticString<8> brokerPort;
    StaticString<24> clientName;
    StaticString<64> baseTopic;
};

class BrokerProxy {
public:
    BrokerProxy() {};
    
    /**
     * Sets the configuration
     */
    void setConfiguration(const BrokerConfiguration& config, String myIPAddress, String myPort) {
        _IPAddress = myIPAddress;
        _port = myPort;
        _config = config;
    }

    /**
     * Connects to the yaha "near-mqtt" broker
     */
    void connect();

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
     */
    void publishMessage(const Message& message);

private:
    void sendToServer(String urlWithoutHost, String jsonBody, String QoS = ""); 

    BrokerConfiguration _config;
    String _IPAddress;
    String _port;

};
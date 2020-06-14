/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @documentation
 * Provides a wrapper to read bme values
 */

#pragma once

#include <Arduino.h>
#include <vector>
#include <json.h>

class Message {
public:
    Message(String topic, String value, String message = "") {
        _topic = topic;
        _value = value;
        _message = message;
    }

    /**
     * Creates the string to publish the message
     * @returns message in yaha mqtt format
     */
    String toPublishString() const {
        const String reasonArray = "[{" +
            jsonStringProperty("message", _message) + 
        "}]";
        const String result = "{" +
            jsonStringProperty("topic", _topic) + "," +
            jsonStringProperty("value", _value) + "," +
            jsonObjectProperty("reason", reasonArray) +
        "}";
        return result;
    }

    
private:

    String _topic;
    String _value;
    String _message;

};

typedef std::vector<Message> Messages_t;

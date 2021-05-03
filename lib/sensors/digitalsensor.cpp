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
#include "debug.h"
#include "message.h"
#include "digitalsensor.h"

void DigitalSensor::activate(uint8_t pin)
{
    if (pin != 0) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);
    }
}

void DigitalSensor::init()
{
}

HtmlPageInfo DigitalSensor::getHtmlPage() {
    return HtmlPageInfo(
        R"htmldigital(
        <form>
        <label for="rain">Rain</label>
        <input type="text" id="rain" readonly [value]="sensor/rain">
        </form>
        )htmldigital",
        "/digital",
        "Digital"
    );
}

void DigitalSensor::run() {
    if (isValid()) {
        uint8_t rain = !digitalRead(_inputPin);
        sendMessageToDevices("sensor/rain", String(rain));
    }
}

Messages_t DigitalSensor::getMessages(const String& baseTopic) {
    Messages_t result;
    if (isValid()) {
        uint8_t rain = !digitalRead(_inputPin);
        const Message rainMessage(baseTopic + "/sensor/rain", String(rain), "send by ESP8266");
        result.push_back(rainMessage);
    }
    return result;
}

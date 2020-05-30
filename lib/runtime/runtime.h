/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @documentation
 * Provides a method to connect to WLAN
 */
#pragma once
#include <Arduino.h>
#include "debug.h"
#include "message.h"

class Runtime {
public:
    Runtime() {
        _startTime = millis();
    }

    Message getMessage(String baseTopic) {
        const float MILLISECONDS_IN_A_SECOND = 1000;
        const float runtime = float(millis() - _startTime) / MILLISECONDS_IN_A_SECOND;

        const Message runtimeMessage(
            baseTopic + "/runtime",
            String(runtime),
            "send by yaha ESP8266 module");

        return runtimeMessage;
    }

private:
    uint32_t _startTime;
    
};

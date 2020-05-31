/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 */

#include "debug.h"
#include "battery.h"

Messages_t Battery::getMessages(const String& baseTopic) {

    const Message runtimeMessage(
        baseTopic + "/battery",
        String(measureVoltage()),
        "send by yaha ESP8266 module");

    std::vector<Message> result;
    result.push_back(runtimeMessage);

    return result;
}

uint16_t Battery::getSleepTimeInSeconds() {
    if (isLowVoltage()) {
        return _configuration.lowVoltageSleepTimeInSeconds;
    } else if (isHighVoltage()) {
        return _configuration.highVoltageSleepTimeInSeconds;
    } else {
        return _configuration.normalVoltageSleepTimeInSeconds;
    }
}

float Battery::measureVoltage() {
    float batteryValue = analogRead(BATTERY_PIN) * _configuration.voltageCalibrationFactor;
    PRINTLN_VARIABLE_IF_DEBUG(_configuration.voltageCalibrationFactor)
    PRINTLN_VARIABLE_IF_DEBUG(analogRead(BATTERY_PIN))
    return batteryValue;
}
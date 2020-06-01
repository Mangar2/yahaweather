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

std::map<String, String> Battery::Configuration::get()
{
    std::map<String, String> result;
    result["voltageCalibrationDivisor"] = voltageCalibrationDivisor;
    result["highVoltageSleepTimeInSeconds"] = highVoltageSleepTimeInSeconds;
    result["lowVoltageSleepTimeInSeconds"] = lowVoltageSleepTimeInSeconds;
    result["normalVoltagesleepTimeInSeconds"] = normalVoltageSleepTimeInSeconds;
    result["highVoltage"] = highVoltage;
    result["lowVoltage"] = lowVoltage;
    return result;
}

void Battery::Configuration::set(std::map<String, String> config)
{
    voltageCalibrationDivisor = config["voltageCalibrationDivisor"].toFloat();
    highVoltageSleepTimeInSeconds = config["highVoltageSleepTimeInSeconds"].toFloat();
    lowVoltageSleepTimeInSeconds = config["lowVoltageSleepTimeInSeconds"].toFloat();
    normalVoltageSleepTimeInSeconds = config["normalVoltagesleepTimeInSeconds"].toFloat();
    highVoltage = config["highVoltage"].toFloat();
    lowVoltage = config["lowVoltage"].toFloat();
}

const char* Battery::htmlForm = 
    R"htmlform(
    <form action="/battery" method="POST">
    <label for="voltage">Current voltage</label>
    <input type="text" id="voltage" name="voltage" [value]="voltage">
    <label for="calibration">Voltage calibration divisor</label>
    <input type="text" id="calibration" name="voltageCalibrationDivisor" [value]="voltageCalibrationDivisor">
    <label for="highVoltage">High voltage</label>
    <input type="text" id="highVoltage" name="highVoltage" [value]="highVoltage">
    <label for="lowVoltage">Low voltage</label>
    <input type="text" id="lowVoltage" name="lowVoltage" [value]="lowVoltage">
    <label for="highTime">High voltage sleep time in seconds</label>
    <input type="text" id="highTime" name="highVoltageSleepTimeInSeconds" [value]="highVoltageSleepTimeInSeconds">
    <label for="normalTime">Normal voltage sleep time in seconds</label>
    <input type="text" id="normalTime" name="normalVoltagesleepTimeInSeconds" [value]="normalVoltagesleepTimeInSeconds">
    <label for="normalTime">Low voltage sleep time in seconds</label>
    <input type="text" id="lowTime" name="lowVoltageSleepTimeInSeconds" [value]="lowVoltageSleepTimeInSeconds">
    <input type="submit" value="Submit">
    </form>
    )htmlform";

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
    float divisor = _configuration.voltageCalibrationDivisor == 0 ? 1 : _configuration.voltageCalibrationDivisor;
    float batteryValue = analogRead(BATTERY_PIN) / divisor;
    PRINTLN_VARIABLE_IF_DEBUG(_configuration.voltageCalibrationDivisor)
    PRINTLN_VARIABLE_IF_DEBUG(analogRead(BATTERY_PIN))
    return batteryValue;
}
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

Battery::Configuration::Configuration() {
    highVoltageSleepTimeInSeconds = 120;
    normalVoltageSleepTimeInSeconds = 900;
    lowVoltageSleepTimeInSeconds = 3600;
    voltageCalibrationDivisor = 24;
    highVoltage = 3.5;
    lowVoltage = 3.1;
    batteryMode = 0;
}

std::map<String, String> Battery::Configuration::get()
{
    std::map<String, String> result;
    result["voltageCalibrationDivisor"] = voltageCalibrationDivisor;
    result["highVoltageSleepTimeInSeconds"] = highVoltageSleepTimeInSeconds;
    result["lowVoltageSleepTimeInSeconds"] = lowVoltageSleepTimeInSeconds;
    result["normalVoltageSleepTimeInSeconds"] = normalVoltageSleepTimeInSeconds;
    result["highVoltage"] = highVoltage;
    result["lowVoltage"] = lowVoltage;
    result["batteryMode"] = batteryMode ? "on" : "";
    return result;
}

void Battery::Configuration::set(std::map<String, String> config)
{
    voltageCalibrationDivisor = config["voltageCalibrationDivisor"].toFloat();
    highVoltageSleepTimeInSeconds = config["highVoltageSleepTimeInSeconds"].toFloat();
    lowVoltageSleepTimeInSeconds = config["lowVoltageSleepTimeInSeconds"].toFloat();
    normalVoltageSleepTimeInSeconds = config["normalVoltageSleepTimeInSeconds"].toFloat();
    highVoltage = config["highVoltage"].toFloat();
    lowVoltage = config["lowVoltage"].toFloat();
    uint8_t newBatteryMode = config["batteryMode"] == "on" ? 1 : 0;
    batteryMode = newBatteryMode;
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
    <input type="text" id="normalTime" name="normalVoltageSleepTimeInSeconds" [value]="normalVoltageSleepTimeInSeconds">
    <label for="normalTime">Low voltage sleep time in seconds</label>
    <input type="text" id="lowTime" name="lowVoltageSleepTimeInSeconds" [value]="lowVoltageSleepTimeInSeconds">
    <input type="hidden" name="batteryMode" display="hidden" value="off">

    <label for="batteryMode">Battery mode enabled</label>
    <div class="sw">
    <input type="checkbox" name="batteryMode" class="sw-checkbox" id="batteryMode" tabindex="0" [checked]="batteryMode">
    <label class="sw-label" for="batteryMode">
        <span class="sw-inner"></span>
        <span class="sw-switch"></span>
    </label>
    </div>

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
        return _config.lowVoltageSleepTimeInSeconds;
    } else if (isHighVoltage()) {
        return _config.highVoltageSleepTimeInSeconds;
    } else {
        return _config.normalVoltageSleepTimeInSeconds;
    }
}

float Battery::measureVoltage() {
    float divisor = _config.voltageCalibrationDivisor == 0 ? 1 : _config.voltageCalibrationDivisor;
    float batteryValue = analogRead(BATTERY_PIN) / divisor;
    PRINTLN_VARIABLE_IF_DEBUG(_config.voltageCalibrationDivisor)
    PRINTLN_VARIABLE_IF_DEBUG(analogRead(BATTERY_PIN))
    return batteryValue;
}
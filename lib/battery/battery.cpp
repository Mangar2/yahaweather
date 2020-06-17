/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 */

#define __DEBUG
#include <debug.h>
#include <eepromaccess.h>
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
    result["battery/voltageCalibrationDivisor"] = voltageCalibrationDivisor;
    result["battery/highVoltageSleepTimeInSeconds"] = highVoltageSleepTimeInSeconds;
    result["battery/lowVoltageSleepTimeInSeconds"] = lowVoltageSleepTimeInSeconds;
    result["battery/normalVoltageSleepTimeInSeconds"] = normalVoltageSleepTimeInSeconds;
    result["battery/highVoltage"] = highVoltage;
    result["battery/lowVoltage"] = lowVoltage;
    result["battery/mode"] = batteryMode ? "on" : "";
    return result;
}

void Battery::Configuration::set(std::map<String, String> config)
{
    voltageCalibrationDivisor = config["battery/voltageCalibrationDivisor"].toFloat();
    highVoltageSleepTimeInSeconds = config["battery/highVoltageSleepTimeInSeconds"].toFloat();
    lowVoltageSleepTimeInSeconds = config["battery/lowVoltageSleepTimeInSeconds"].toFloat();
    normalVoltageSleepTimeInSeconds = config["battery/normalVoltageSleepTimeInSeconds"].toFloat();
    highVoltage = config["battery/highVoltage"].toFloat();
    lowVoltage = config["battery/lowVoltage"].toFloat();
    uint8_t newBatteryMode = config["battery/mode"] == "on" ? 1 : 0;
    batteryMode = newBatteryMode;
}

const char* Battery::htmlForm = 
    R"htmlform(
    <form action="/battery" method="POST">
    <label for="voltage">Current voltage</label>
    <input type="text" id="voltage" name="battery/voltage" [value]="battery/voltage">
    <label for="sleepTime">Resulting sleep time in seconds</label>
    <input type="text" id="sleepTime" name="battery/sleepTimeInSeconds" [value]="battery/sleepTimeInSeconds">
    <label for="calibration">Voltage calibration divisor</label>
    <input type="text" id="calibration" name="battery/voltageCalibrationDivisor" [value]="battery/voltageCalibrationDivisor">
    <label for="highVoltage">High voltage</label>
    <input type="text" id="highVoltage" name="battery/highVoltage" [value]="battery/highVoltage">
    <label for="lowVoltage">Low voltage</label>
    <input type="text" id="lowVoltage" name="battery/lowVoltage" [value]="battery/lowVoltage">
    <label for="highTime">High voltage sleep time in seconds</label>
    <input type="text" id="highTime" name="battery/highVoltageSleepTimeInSeconds" [value]="battery/highVoltageSleepTimeInSeconds">
    <label for="normalTime">Normal voltage sleep time in seconds</label>
    <input type="text" id="normalTime" name="battery/normalVoltageSleepTimeInSeconds" [value]="battery/normalVoltageSleepTimeInSeconds">
    <label for="normalTime">Low voltage sleep time in seconds</label>
    <input type="text" id="lowTime" name="battery/lowVoltageSleepTimeInSeconds" [value]="battery/lowVoltageSleepTimeInSeconds">
    <input type="hidden" name="battery/mode" display="hidden" value="off">

    <label for="batteryMode">Battery mode enabled</label>
    <div class="sw">
    <input type="checkbox" name="battery/mode" class="sw-checkbox" id="batteryMode" tabindex="0" [checked]="battery/mode">
    <label class="sw-label" for="batteryMode">
        <span class="sw-inner"></span>
        <span class="sw-switch"></span>
    </label>
    </div>

    <input type="submit" value="Submit">
    </form>
    )htmlform";

uint16_t Battery::writeConfigToEEPROM(uint16_t EEPROMAddress) {
    EEPROMAccess::write(EEPROMAddress, (uint8_t*) &_config, sizeof(_config));
    return EEPROMAddress + sizeof(_config);
}

uint16_t Battery::readConfigFromEEPROM(uint16_t EEPROMAddress) { 
    EEPROMAccess::read(EEPROMAddress, (uint8_t*) &_config, sizeof(_config));
    return EEPROMAddress + sizeof(_config);
}

Messages_t Battery::getMessages(const String& baseTopic) {

    const Message runtimeMessage(
        baseTopic + "/battery/voltage",
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
    return batteryValue;
}
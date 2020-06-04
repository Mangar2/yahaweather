/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 */

#define __DEBUG
#include "debug.h"
#include "irrigation.h"

Irrigation::Configuration::Configuration() {
        lowIrrigationDurationInSeconds = 30;
        lowWakeupUntilIrrigation = 24;
        highIrrigationDurationInSeconds = 30;
        highWakeupUntilIrrigation = 24;
        pump2Factor = 1;
}

std::map<String, String> Irrigation::Configuration::get()
{
    std::map<String, String> result;
    result["lowIrrigationDurationInSeconds"] = lowIrrigationDurationInSeconds;
    result["lowWakeupUntilIrrigation"] = lowWakeupUntilIrrigation;
    result["highIrrigationDurationInSeconds"] = highIrrigationDurationInSeconds;
    result["highWakeupUntilIrrigation"] = highWakeupUntilIrrigation;
    result["pump2Factor"] = pump2Factor;
    return result;
}

void Irrigation::Configuration::set(std::map<String, String> config)
{
    lowIrrigationDurationInSeconds = config["lowIrrigationDurationInSeconds"].toInt();
    lowWakeupUntilIrrigation = config["lowWakeupUntilIrrigation"].toInt();
    highIrrigationDurationInSeconds = config["highIrrigationDurationInSeconds"].toInt();
    highWakeupUntilIrrigation = config["highWakeupUntilIrrigation"].toInt();
    pump2Factor = config["pump2Factor"].toFloat();
}

const char* Irrigation::htmlForm = 
    R"htmlform(
    <form action="/irrigation" method="POST">
    <label for="Humidity">Current humidity</label>
    <input type="text" id="Humidity" name="humidity" [value]="humidity">

    <label for="lowIrrigationDurationInSeconds">Low humidity irrigation duration in seconds (30% rH)</label>
    <input type="text" id="lowIrrigationDurationInSeconds" name="lowIrrigationDurationInSeconds" [value]="lowIrrigationDurationInSeconds">
    <label for="lowWakeupUntilIrrigation">Low humidity amount of wakeups until irrigation (30% rH)</label>
    <input type="text" id="lowWakeupUntilIrrigation" name="lowWakeupUntilIrrigation" [value]="lowWakeupUntilIrrigation">

    <label for="highIrrigationDurationInSeconds">High humidity irrigation duration in seconds (60% rH)</label>
    <input type="text" id="highIrrigationDurationInSeconds" name="highIrrigationDurationInSeconds" [value]="highIrrigationDurationInSeconds">
    <label for="highWakeupUntilIrrigation">High humidity amount of wakeups until irrigation (60% rH)</label>
    <input type="text" id="highWakeupUntilIrrigation" name="highWakeupUntilIrrigation" [value]="highWakeupUntilIrrigation">

    <label for="pump2Factor">Duration factor for pump 2</label>
    <input type="text" id="pump2Factor" name="pump2Factor" [value]="pump2Factor">

    <input type="submit" value="Submit">
    </form>
    )htmlform";

Messages_t Irrigation::getMessages(const String& baseTopic, float humidity) {

    std::vector<Message> result;
    result.push_back(Message(
        baseTopic + "/irrigation/pump1",
        String(getIrrigationDurationInSeconds(humidity, 1)),
        "send by yaha ESP8266 module"
    ));
    result.push_back(Message(
        baseTopic + "/irrigation/pump2",
        String(getIrrigationDurationInSeconds(humidity, 2)),
        "send by yaha ESP8266 module"
    ));

    return result;
}

uint16_t Irrigation::getIrrigationDurationInSeconds(float humidity, uint8_t pumpNo) {
    float humidityDifference = 60 - 30;
    float irrigationDifference = _config.highIrrigationDurationInSeconds - _config.lowIrrigationDurationInSeconds;
    float relativeIrrigation = irrigationDifference / humidityDifference;
    uint16_t duration = (humidity - 30) * relativeIrrigation + _config.lowIrrigationDurationInSeconds;
    if (pumpNo == 2) {
        duration *= _config.pump2Factor;
    }
    return duration;
}

void Irrigation::runIrrigation(float humidity) {
    const uint32_t MILLISECONDS_IN_A_SECOND = 1000;
    for (int pumpNo = 1; pumpNo <= 2; pumpNo++) {
        const uint16_t timeInSeconds = getIrrigationDurationInSeconds(humidity, pumpNo);
        const uint8_t pumpPin = pumpNo == 1 ? _config.pump1Pin : _config.pump2Pin;
        PRINT_IF_DEBUG("Pump ");
        PRINT_IF_DEBUG(pumpNo)
        PRINT_IF_DEBUG(" on for ");
        PRINT_IF_DEBUG(timeInSeconds / 60);
        PRINT_IF_DEBUG(":");
        PRINT_IF_DEBUG(timeInSeconds % 60);
        digitalWrite(pumpPin, HIGH);
        for (uint16_t cnt = 0; cnt < timeInSeconds; cnt ++) {
            PRINT_IF_DEBUG(".");
            delay(MILLISECONDS_IN_A_SECOND);
        }
        digitalWrite(pumpPin, LOW);
        PRINTLN_IF_DEBUG(" Pump off");
    }
}

bool Irrigation::doIrrigation(float humidity, uint16_t wakeupAmount) {
    float humidityDifference = 60 - 30;
    float wakeupDifference = _config.highWakeupUntilIrrigation - _config.lowWakeupUntilIrrigation;
    float relativeWakeup = wakeupDifference / humidityDifference;
    uint16_t neededWakeupAmount = (humidity - 30) * relativeWakeup + _config.lowWakeupUntilIrrigation;
    return wakeupAmount >= neededWakeupAmount;
}
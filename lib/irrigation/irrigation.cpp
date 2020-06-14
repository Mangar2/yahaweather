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
        lowIrrigationDurationInSeconds = 0;
        lowWakeupUntilIrrigation = 24;
        highIrrigationDurationInSeconds = 0;
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

const char* swithPumpForm = 
    R"htmlswitch(
    
    )htmlswitch";

Irrigation::Irrigation(uint8_t pump1Pin, uint8_t pump2Pin)
 : _pump1Pin(pump1Pin), _pump2Pin(pump2Pin) {
    digitalWrite(pump1Pin, LOW); 
    digitalWrite(pump2Pin, LOW); 
    pinMode(pump1Pin, OUTPUT); 
    pinMode(pump2Pin, OUTPUT); 
    PRINTLN_VARIABLE_IF_DEBUG(pump1Pin);
    PRINTLN_IF_DEBUG("test high")
    digitalWrite(pump1Pin, HIGH);
    delay(2000);
    digitalWrite(pump1Pin, LOW);
    PRINTLN_IF_DEBUG("test low")
};

Messages_t Irrigation::getMessages(const String& baseTopic) {

    std::vector<Message> result;
    result.push_back(Message(
        baseTopic + "/irrigation/pump1",
        String(getIrrigationDurationInSeconds(1)),
        "send by yaha ESP8266 module"
    ));
    result.push_back(Message(
        baseTopic + "/irrigation/pump2",
        String(getIrrigationDurationInSeconds(2)),
        "send by yaha ESP8266 module"
    ));

    return result;
}

void Irrigation::setConfig(jsonObject_t config) { 
    _config.set(config); 
    _humidity = config["humidity"].toFloat();
    _wakeupAmount = config["rtc/wakeupAmount"].toFloat();
    PRINT_IF_DEBUG("Irrigation set Config ")
    PRINTLN_VARIABLE_IF_DEBUG(_wakeupAmount)
};

uint16_t Irrigation::getIrrigationDurationInSeconds(uint8_t pumpNo) {
    float humidityDifference = 60 - 30;
    float irrigationDifference = _config.highIrrigationDurationInSeconds - _config.lowIrrigationDurationInSeconds;
    float relativeIrrigation = irrigationDifference / humidityDifference;
    uint16_t duration = (_humidity - 30) * relativeIrrigation + _config.lowIrrigationDurationInSeconds;
    if (pumpNo == 2) {
        duration *= _config.pump2Factor;
    }
    return duration;
}

void Irrigation::run() {
    const uint32_t MILLISECONDS_IN_A_SECOND = 1000;
    for (int pumpNo = 1; pumpNo <= 2; pumpNo++) {
        const uint16_t timeInSeconds = getIrrigationDurationInSeconds(pumpNo);
        const uint8_t pumpPin = pumpNo == 1 ? _pump1Pin : _pump2Pin;
        PRINT_IF_DEBUG("Pump ");
        PRINT_IF_DEBUG(pumpNo)
        PRINT_IF_DEBUG(" on for ");
        PRINT_IF_DEBUG(timeInSeconds / 60);
        PRINT_IF_DEBUG(":");
        PRINT_IF_DEBUG(timeInSeconds % 60);
        PRINTLN_VARIABLE_IF_DEBUG(pumpPin)
        digitalWrite(pumpPin, HIGH);
        for (uint16_t cnt = 0; cnt < timeInSeconds; cnt ++) {
            PRINT_IF_DEBUG(".");
            delay(MILLISECONDS_IN_A_SECOND);
        }
        digitalWrite(pumpPin, LOW);
        PRINTLN_IF_DEBUG(" Pump off");
    }
}

bool Irrigation::doIrrigation() {
    float humidityDifference = 60 - 30;
    float wakeupDifference = _config.highWakeupUntilIrrigation - _config.lowWakeupUntilIrrigation;
    float relativeWakeup = wakeupDifference / humidityDifference;
    uint16_t neededWakeupAmount = (_humidity - 30) * relativeWakeup + _config.lowWakeupUntilIrrigation;
    PRINTLN_VARIABLE_IF_DEBUG(_wakeupAmount)
    PRINTLN_VARIABLE_IF_DEBUG(neededWakeupAmount)
    return _wakeupAmount >= neededWakeupAmount;
}
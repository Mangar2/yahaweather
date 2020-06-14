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
        lowDurationInSeconds = 0;
        lowWakeup = 24;
        highDurationInSeconds = 0;
        highWakeup = 24;
        pump2Factor = 1;
}

std::map<String, String> Irrigation::Configuration::get()
{
    std::map<String, String> result;
    result["irrigation/lowDurationInSeconds"] = lowDurationInSeconds;
    result["irrigation/lowWakeup"] = lowWakeup;
    result["irrigation/highDurationInSeconds"] = highDurationInSeconds;
    result["irrigation/highWakeup"] = highWakeup;
    result["irrigation/pump2Factor"] = pump2Factor;
    return result;
}

void Irrigation::Configuration::set(std::map<String, String> config)
{
    lowDurationInSeconds = config["irrigation/lowDurationInSeconds"].toInt();
    lowWakeup = config["irrigation/lowWakeup"].toInt();
    highDurationInSeconds = config["irrigation/highDurationInSeconds"].toInt();
    highWakeup = config["irrigation/highWakeup"].toInt();
    pump2Factor = config["irrigation/pump2Factor"].toFloat();
}

const char* Irrigation::htmlForm = 
    R"htmlform(
    <form action="/irrigation" method="POST">
    <label for="Humidity">Current humidity</label>
    <input type="text" id="Humidity" name="humidity" [value]="humidity">

    <label for="lowDuration">Low humidity irrigation duration in seconds (30% rH)</label>
    <input type="text" id="lowDuration" name="irrigation/lowDurationInSeconds" [value]="irrigation/lowDurationInSeconds">
    <label for="lowWakeup">Low humidity amount of wakeups until irrigation (30% rH)</label>
    <input type="text" id="lowWakeup" name="irrigation/lowWakeup" [value]="irrigation/lowWakeup">

    <label for="highDuration">High humidity irrigation duration in seconds (60% rH)</label>
    <input type="text" id="highDuration" name="irrigation/hightDurationInSeconds" [value]="irrigation/hightDurationInSeconds">
    <label for="highWakeup">High humidity amount of wakeups until irrigation (60% rH)</label>
    <input type="text" id="highWakeup" name="irrigation/highWakeup" [value]="irrigation/highWakeup">

    <label for="pump2Factor">Duration factor for pump 2</label>
    <input type="text" id="pump2Factor" name="irrigation/pump2Factor" [value]="irrigation/pump2Factor">

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
    float irrigationDifference = _config.highDurationInSeconds - _config.lowDurationInSeconds;
    float relativeIrrigation = irrigationDifference / humidityDifference;
    uint16_t duration = (_humidity - 30) * relativeIrrigation + _config.lowDurationInSeconds;
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
    float wakeupDifference = _config.highWakeup - _config.lowWakeup;
    float relativeWakeup = wakeupDifference / humidityDifference;
    uint16_t neededWakeupAmount = (_humidity - 30) * relativeWakeup + _config.lowWakeup;
    PRINTLN_VARIABLE_IF_DEBUG(_wakeupAmount)
    PRINTLN_VARIABLE_IF_DEBUG(neededWakeupAmount)
    return _wakeupAmount >= neededWakeupAmount;
}
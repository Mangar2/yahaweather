/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 */

#include "debug.h"
#include "irrigation.h"

Irrigation::Configuration::Configuration() {
        lowHumidity = 30;
        lowIrrigationDurationInSeconds = 30;
        lowWakeupUntilIrrigation = 24;
        midIrrigationDurationInSeconds = 20;
        midWakeupUntilIrrigation = 24;
        highHumidity = 50;
        highIrrigationDurationInSeconds = 30;
        highWakeupUntilIrrigation = 24;
        pump2Factor = 1;
        pump1Pin = D6;
        pump2Pin = D7;
}

std::map<String, String> Irrigation::Configuration::get()
{
    std::map<String, String> result;
    result["lowHumidity"] = lowHumidity;
    result["lowIrrigationDurationInSeconds"] = lowIrrigationDurationInSeconds;
    result["lowWakeupUntilIrrigation"] = lowWakeupUntilIrrigation;
    result["midIrrigationDurationInSeconds"] = midIrrigationDurationInSeconds;
    result["midWakeupUntilIrrigation"] = midWakeupUntilIrrigation;
    result["highHumidity"] = highHumidity;
    result["highIrrigationDurationInSeconds"] = highIrrigationDurationInSeconds;
    result["highWakeupUntilIrrigation"] = highWakeupUntilIrrigation;
    result["pump1Factor"] = pump2Factor;
    result["pump1Pin"] = pump1Pin;
    result["pump2Pin"] = pump2Pin;
    return result;
}

void Irrigation::Configuration::set(std::map<String, String> config)
{
    lowHumidity = config["lowHumidity"].toInt();
    lowIrrigationDurationInSeconds = config["lowIrrigationDurationInSeconds"].toInt();
    lowWakeupUntilIrrigation = config["lowWakeupUntilIrrigation"].toInt();
    midIrrigationDurationInSeconds = config["midIrrigationDurationInSeconds"].toInt();
    midWakeupUntilIrrigation = config["midWakeupUntilIrrigation"].toInt();
    highHumidity = config["highHumidity"].toInt();
    highIrrigationDurationInSeconds = config["highIrrigationDurationInSeconds"].toInt();
    highWakeupUntilIrrigation = config["highWakeupUntilIrrigation"].toInt();
    pump2Factor = config["pump2Factor"].toFloat();
    pump1Pin = config["pump1Pin"].toInt();
    pump2Pin = config["pump2Pin"].toInt();
}

const char* Irrigation::htmlForm = 
    R"htmlform(
    <form action="/irrigation" method="POST">
    <label for="Humidity">Current humidity</label>
    <input type="text" id="Humidity" name="humidity" [value]="humidity">

    <label for="lowHumidity">Low humidity in percent</label>
    <input type="text" id="lowHumidity" name="lowHumidity" [value]="lowHumidity">
    <label for="lowIrrigationDurationInSeconds">Low irrigation duration in seconds</label>
    <input type="text" id="lowIrrigationDurationInSeconds" name="lowIrrigationDurationInSeconds" [value]="lowIrrigationDurationInSeconds">
    <label for="lowWakeupUntilIrrigation">Low amount of wakeups until irrigation</label>
    <input type="text" id="lowWakeupUntilIrrigation" name="lowWakeupUntilIrrigation" [value]="lowWakeupUntilIrrigation">

    <label for="midIrrigationDurationInSeconds">Normal irrigation duration in seconds</label>
    <input type="text" id="midIrrigationDurationInSeconds" name="midIrrigationDurationInSeconds" [value]="midIrrigationDurationInSeconds">
    <label for="midWakeupUntilIrrigation">Normal amount of wakeups until irrigation</label>
    <input type="text" id="midWakeupUntilIrrigation" name="midWakeupUntilIrrigation" [value]="midWakeupUntilIrrigation">

    <label for="highHumidity">High humidity in percent</label>
    <input type="text" id="highHumidity" name="highHumidity" [value]="highHumidity">
    <label for="highIrrigationDurationInSeconds">High irrigation duration in seconds</label>
    <input type="text" id="highIrrigationDurationInSeconds" name="highIrrigationDurationInSeconds" [value]="highIrrigationDurationInSeconds">
    <label for="highWakeupUntilIrrigation">High amount of wakeups until irrigation</label>
    <input type="text" id="highWakeupUntilIrrigation" name="highWakeupUntilIrrigation" [value]="highWakeupUntilIrrigation">

    <label for="pump2Factor">Duration factor for pump 2</label>
    <input type="text" id="pump2Factor" name="pump2Factor" [value]="pump2Factor">

    <label for="pump1Pin">Pin for first pump</label>
    <input type="text" id="pump1Pin" name="pump1Pin" [value]="pump1Pin">
    <label for="pump2Pin">Pin for second pump</label>
    <input type="text" id="pump2Pin" name="pump2Pin" [value]="pump2Pin">

    <input type="submit" value="Submit">
    </form>
    )htmlform";

Messages_t Irrigation::getMessages(const String& baseTopic, float humidity) {

    std::vector<Message> result;
    result.push_back(Message(
        baseTopic + "/irrigation/pump1",
        String(getIrrigationTimeInSeconds(humidity, 1)),
        "send by yaha ESP8266 module"
    ));
    result.push_back(Message(
        baseTopic + "/irrigation/pump2",
        String(getIrrigationTimeInSeconds(humidity, 2)),
        "send by yaha ESP8266 module"
    ));

    return result;
}

uint16_t Irrigation::getIrrigationTimeInSeconds(float humidity, uint8_t pumpNo) {
    uint16_t duration;
    if (isLowHumidity(humidity)) {
        duration = _config.lowIrrigationDurationInSeconds;
    } else if (isHighHumidity(humidity)) {
        duration = _config.highIrrigationDurationInSeconds;
    } else {
        duration = _config.midIrrigationDurationInSeconds;
    }
    if (pumpNo == 2) {
        duration *= _config.pump2Factor;
    }
    return duration;
}

void Irrigation::runIrrigation(float humidity) {
    const uint32_t MILLISECONDS_IN_A_SECOND = 1000;
    for (int pumpNo = 1; pumpNo <= 2; pumpNo++) {
        const uint16_t timeInSeconds = getIrrigationTimeInSeconds(humidity, pumpNo);
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
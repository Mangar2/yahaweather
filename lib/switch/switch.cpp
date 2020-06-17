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
#include "switch.h"

const char* Switch::htmlForm = 
    R"htmlform(
    <form action="/switch" method="POST">
    <label class="tb">D4, GPIO12</label>
    <input type="hidden" name="switch/D4" id="D4" value="toggle">
    <input type="submit" class="tb" [value]="switch/D4">
    </form>
    <form action="/switch" method="POST">
    <label class="tb">D5, GPIO12</label>
    <input type="hidden" name="switch/D5" id="D5" value="toggle">
    <input type="submit" class="tb" [value]="switch/D5">
    </form>            
    <form action="/switch" method="POST">
    <label class="tb">D6, GPIO12</label>
    <input type="hidden" name="switch/D6" id="D6" value="toggle">
    <input type="submit" class="tb" [value]="switch/D6">
    </form>
    <form action="/switch" method="POST">
    <label class="tb">D7, GPIO13</label>
    <input type="hidden" name="switch/D7" id="D7" value="toggle">
    <input type="submit" class="tb" [value]="switch/D7">
    </form>
    )htmlform";


Switch::Switch() {
    pinMode(D4, OUTPUT); 
    pinMode(D5, OUTPUT); 
    pinMode(D6, OUTPUT); 
    pinMode(D7, OUTPUT); 
}

void Switch::togglePin(uint8_t pin, String name, const jsonObject_t& config) {
    if (!config.count(name) == 1) {
        return;
    }
    uint8_t pinState = digitalRead(pin);
    bool toggleCommand = config.at(name) == "toggle";
    bool changeState = 
        (config.at(name) == "off" && pinState == HIGH) ||
        (config.at(name) == "on" && pinState == LOW);

    if (toggleCommand || changeState) {
        uint8_t newValue = pinState == LOW ? HIGH : LOW;
        PRINT_IF_DEBUG(pin)
        PRINT_IF_DEBUG(" = ")
        PRINTLN_IF_DEBUG(newValue)
        digitalWrite(pin, newValue);
        sendMessageToDevices(name, newValue == HIGH ? "on" : "off");
    } 
}

void Switch::setConfig(jsonObject_t& config) {
    togglePin(D4, "switch/D4", config);
    togglePin(D5, "switch/D5", config);
    togglePin(D6, "switch/D6", config);
    togglePin(D7, "switch/D7", config);
    togglePin(D8, "switch/D8", config);
    togglePin(D9, "switch/D9", config);
    togglePin(D10, "switch/D10", config);
}

jsonObject_t Switch::getConfig() {
    jsonObject_t result;
    result["switch/D4"] = digitalRead(D4) == HIGH ? "on" : "off";
    result["switch/D5"] = digitalRead(D5) == HIGH ? "on" : "off";
    result["switch/D6"] = digitalRead(D6) == HIGH ? "on" : "off";
    result["switch/D7"] = digitalRead(D7) == HIGH ? "on" : "off";
    return result;
}


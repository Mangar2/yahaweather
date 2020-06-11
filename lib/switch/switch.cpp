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
    <input type="hidden" name="D4" id="D4" value="toggle">
    <input type="submit" class="tb" [value]="D4">
    </form>
    <form action="/switch" method="POST">
    <label class="tb">D5, GPIO12</label>
    <input type="hidden" name="D5" id="D5" value="toggle">
    <input type="submit" class="tb" [value]="D5">
    </form>            
    <form action="/switch" method="POST">
    <label class="tb">D6, GPIO12</label>
    <input type="hidden" name="D6" id="D6" value="toggle">
    <input type="submit" class="tb" [value]="D6">
    </form>
    <form action="/switch" method="POST">
    <label class="tb">D7, GPIO13</label>
    <input type="hidden" name="D7" id="D7" value="toggle">
    <input type="submit" class="tb" [value]="D7">
    </form>
    )htmlform";


Switch::Switch() {
    pinMode(D4, OUTPUT); 
    pinMode(D5, OUTPUT); 
    pinMode(D6, OUTPUT); 
    pinMode(D7, OUTPUT); 
}

void togglePin(uint8_t pin, String name, const std::map<String, String>& config) {
    if (config.count(name) == 1 && config.at(name) == "toggle") {
        uint8_t newValue = digitalRead(pin) == LOW ? HIGH : LOW;
        PRINT_IF_DEBUG(pin)
        PRINT_IF_DEBUG(" = ")
        PRINTLN_IF_DEBUG(newValue)
        digitalWrite(pin, newValue);
    }
}

void Switch::set(std::map<String, String> config) {
    togglePin(D4, "D4", config);
    togglePin(D5, "D5", config);
    togglePin(D6, "D6", config);
    togglePin(D7, "D7", config);
    togglePin(D8, "D8", config);
    togglePin(D9, "D9", config);
    togglePin(D10, "D10", config);
}

std::map<String, String> Switch::get() {
    std::map<String, String> result;
    result["D4"] = digitalRead(D4) == HIGH ? "on" : "off";
    result["D5"] = digitalRead(D5) == HIGH ? "on" : "off";
    result["D6"] = digitalRead(D6) == HIGH ? "on" : "off";
    result["D7"] = digitalRead(D7) == HIGH ? "on" : "off";
    return result;
}


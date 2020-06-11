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
    <form action="/SwitchD6" method="POST">
    <label for "D6">D6, GPIO12</label>
    <div class="sw">
    <input type="submit" name="D6" class="sw-checkbox" id="D6" tabindex="0" [checked]="D6">
    <label class="sw-label" for="D6">
        <span class="sw-inner"></span>
        <span class="sw-switch"></span>
    </label>
    </div>
    </form>
    <form action="/SwitchD7" method="POST">
    <label for "D7">D7, GPIO12</label>
    <div class="sw">
    <input type="submit" name="D7" class="sw-checkbox" id="D7" tabindex="0" [checked]="D7">
    <label class="sw-label" for="D7">
        <span class="sw-inner"></span>
        <span class="sw-switch"></span>
    </label>
    </div>
    </form>
    )htmlform";


Switch::Switch() {

}
 
/**
 * Gets the configuration as key/value map
 */
std::map<String, String> Switch::get() {

}

void setPin(uint8_t pin, String name, const std::map<String, String>& config) {
    if (config.count(name) == 1) {
        digitalWrite(pin, config.at(name) == "on" ? HIGH : LOW);
    }
}

void Switch::set(std::map<String, String> config) {
    setPin(D4, "D4", config);
    setPin(D5, "D5", config);
    setPin(D6, "D6", config);
    setPin(D7, "D7", config);
    setPin(D8, "D8", config);
    setPin(D9, "D9", config);
    setPin(D10, "D10", config);
}

std::map<String, String> Switch::get() {
    std::map<String, String> result;
    result["D4"] = digitalRead(D4) == HIGH ? "on" : "off";
    result["D5"] = digitalRead(D5) == HIGH ? "on" : "off";
    result["D6"] = digitalRead(D6) == HIGH ? "on" : "off";
}


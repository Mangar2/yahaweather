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
#include "motion.h"

bool Motion::motion1;
bool Motion::motion2;
bool Motion::motion3;

void ICACHE_RAM_ATTR motionD5() { Motion::motion1 = true; }
void ICACHE_RAM_ATTR motionD6() { Motion::motion2 = true; }
void ICACHE_RAM_ATTR motionD7() { Motion::motion3 = true; }

Motion::Motion(){
    pinMode(D5, INPUT); 
    pinMode(D6, INPUT); 
    pinMode(D7, INPUT); 
    attachInterrupt(digitalPinToInterrupt(D5), motionD5, RISING);
    attachInterrupt(digitalPinToInterrupt(D6), motionD6, RISING);
    attachInterrupt(digitalPinToInterrupt(D7), motionD7, RISING);
    motion1 = digitalRead(D5) == HIGH;
    motion2 = digitalRead(D6) == HIGH;
    motion3 = digitalRead(D7) == HIGH;
}

Messages_t Motion::getMessages(const String& baseTopic) {
    std::vector<Message> result;
    bool motion = motion1 || motion2 || motion3;
    result.push_back(Message(baseTopic + "/motion sensor/detection state", motion ? "1" : "0", "motion detected by yaha ESP8266 module"));
    result.push_back(Message(baseTopic + "/motion sensor/sensor1", motion1 ? "1" : "0", "motion detected by yaha ESP8266 module"));
    result.push_back(Message(baseTopic + "/motion sensor/sensor2", motion2 ? "1" : "0", "motion detected by yaha ESP8266 module"));
    result.push_back(Message(baseTopic + "/motion sensor/sensor3", motion3 ? "1" : "0", "motion detected by yaha ESP8266 module"));
    motion1 = false;
    motion2 = false;
    motion3 = false;

    return result;
}
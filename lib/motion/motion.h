/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Provides a class to detect motion
 */
#pragma once

#include <Arduino.h>
#include <message.h>
#include <map>
#include <idevice.h>

class Motion : public IDevice
{
public:
    Motion();
    /**
     * Gets a yaha messages to send the battery voltage
     */
    virtual Messages_t getMessages(const String &baseTopic);


    /**
     * Check for battery mode. If not in battery mode we will add interrupt functions to 
     * register motion changes. In battery mode motion information is only collected directly after start
     * @param key message identifier
     * @param value message value
     */
    virtual void handleMessage(const String& key, const String& value);

    static bool motion1;
    static bool motion2;
    static bool motion3;

private:


};
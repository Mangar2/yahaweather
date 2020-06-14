/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Interface for devices of the yaha station
 */
#pragma once
#include <Arduino.h>

class IMessageBroker {
public:
    
    /**
     * Sends a message to all devices
     * @param key message identifier
     * @param value message value
     */
    virtual void sendMessageToDevices(const String& key, const String& value) = 0;

};

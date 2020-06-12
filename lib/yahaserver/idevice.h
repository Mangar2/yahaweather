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

#include <json.h>
#include <message.h>

class IDevice {
public:
    
    /**
     * Sets configuration from a key/value map
     * @param config map containing the configuration
     */
    virtual void setConfig(jsonObject_t config) = 0;
    
    /**
     * Gets configuration as key/value map
     * @returns configuration 
     */
    virtual jsonObject_t getConfig() = 0;
    
    /**
     * Gets messages to send
     * @param baseTopic start topic to be used to create the message topic
     * @returns a list of messages to send with topic, value and reason
     */
    virtual Messages_t getMessages(const String& baseTopic) = 0;

    /**
     * @returns true, if the device is initialized and working
     */
    virtual bool isValid() const = 0;

};

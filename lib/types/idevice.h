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

#include <imessagebroker.h>
#include <htmlpageinfo.h>
#include <message.h>

class IDevice {
public:
    
    /**
     * Sets configuration from a key/value map
     * @param config map containing the configuration
     */
    virtual void setConfig(jsonObject_t& config) {};
    
    /**
     * Gets configuration as key/value map
     * @returns configuration 
     */
    virtual jsonObject_t getConfig() { return jsonObject_t(); };

    /**
     * Writes the configuration to EEPROM
     * @param EEPROMAddress EEPROM address to write to
     * @returns EEPROM address for the next device
     */
    virtual uint16_t writeConfigToEEPROM(uint16_t EEPROMAddress) { return EEPROMAddress; }

    /**
     * Reads configuration from EEPROM
     * @param EEPROMAddress EEPROM address to read from
     * @returns EEPROM address for the next device
     */
    virtual uint16_t readConfigFromEEPROM(uint16_t EEPROMAddress) { return EEPROMAddress; }
    
    /**
     * Gets messages to send
     * @param baseTopic start topic to be used to create the message topic
     * @returns a list of messages to send with topic, value and reason
     */
    virtual Messages_t getMessages(const String& baseTopic) { return Messages_t(); }

    /**
     * Handles a message send to devices
     * @param key message identifier
     * @param value message value
     */
    virtual void handleMessage(const String& key, const String& value) {}

    /**
     * @returns true, if the device is initialized and working
     */
    virtual bool isValid() const { return true; };

    /**
     * Called on setup
     */
    virtual void setup() {}

    /**
     * Called on closedown
     */
    virtual void closeDown() {}

    /**
     * Called on loop
     */
    virtual void run() {}

    /**
     * Gets an info about the matching html page
     */
    virtual HtmlPageInfo getHtmlPage() { return HtmlPageInfo(); }

    void setMessageBroker(IMessageBroker* messageBroker) {
        _messageBroker = messageBroker;
    }

    /**
     * Sends a message to all devices
     * @param key message identifier
     * @param value message value
     */
    virtual void sendMessageToDevices(const String& key, const String& value) {
        _messageBroker->sendMessageToDevices(key, value);
    }
private:
    IMessageBroker* _messageBroker;    

};

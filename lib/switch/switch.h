/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Provides a class to steer Switch based on humidity measurement
 */
#pragma once

#include <Arduino.h>
#include <message.h>
#include <map>
#include <idevice.h>

class Switch : public IDevice
{
public:

    Switch(); 

    /**
     * Gets the configuration as key/value map
     */
    virtual jsonObject_t getConfig();

    /**
     * Sets the configuration from a key/value map
     * @param config configuration settings in a map
     */
    virtual void setConfig(jsonObject_t config);

    /**
     * Gets messages to send
     * @param baseTopic start topic to be used to create the message topic
     * @returns a list of messages to send with topic, value and reason
     */
    virtual Messages_t getMessages(const String& baseTopic) {
        return Messages_t();
    }

    /**
     * Gets an info about the matching html page
     */
    virtual HtmlPageInfo getHtmlPage() { return HtmlPageInfo(htmlForm, "/switch", "Switch"); }


private:

    /**
     * Toggels the state of a digital output
     * @param pin number of the digital output
     * @param name string representation of the pin number
     * @param config new digital output settings
     */
    void togglePin(uint8_t pin, String name, const jsonObject_t& config);

    static const char* htmlForm;
};
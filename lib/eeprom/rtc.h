/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Provides a class to connect to WLAN
 */

#pragma once

#include <json.h>
#include <message.h>
#include <idevice.h>

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif

class RTC : public IDevice {

public:

    RTC(uint16_t startWakeupCounter = 0) { 
        initWakeupCounter(startWakeupCounter); 
    }

    /**
     * Called on setup
     */
    virtual void setup();
    
    /**
     * Gets messages to send
     * @param baseTopic start topic to be used to create the message topic
     * @returns a list of messages to send with topic, value and reason
     */
    virtual Messages_t getMessages(const String& baseTopic);

    /**
     * Handles a message send to devices
     * @param key message identifier
     * @param value message value
     */
    virtual void handleMessage(const String& key, const String& value) {
        if (key == "rtc/wakeupAmount") {
            setWakeupAmount(value.toInt());
        }
    }

    /**
     * Gets the reason for a wakuep event
     */
    uint16_t getResetReason();

    /**
     * Returns true, if the module waked up from deep sleep
     */
    bool isDeepSleepAwake();

    /**
     * Returns true, if the module waked up from power up
     */
    bool isPowerUpAwake();

    /**
     * Gets the time stored in the rtc module
     */
    uint32_t getRtcTime();

    /**
     * Gets the amount of wakeups encountered
     */
    uint16_t getWakeupAmount();

    /**
     * Gets the fast reset amount read from RTC-Mem at the start
     */
    bool isFastReset();

private:

    /**
     * Sets the amount of wakeup encountered
     */
    void setWakeupAmount(uint16_t wakeupAmount);

    /**
     * Initializes the RTC memory entries and places a wakeup counter at the beginning
     * @param startWakeupCounter start value for the wakeup counter
     */
    void initWakeupCounter(uint16_t startWakeupCounter);

    /**
     * Increase the amount of wakups encountered by one
     */
    void incWakeupAmount(); 

    /**
     * Type of start, either "NORMAL_REST" or "FAST_RESET"
     */
    uint16_t _startType;

    /**
     * Started by RTC wakeup (false) or power on (true)
     */
    bool _isPowerOn;

};

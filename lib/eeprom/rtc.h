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

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif

namespace RTC {

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
     * Initializes the RTC memory entries and places a wakeup counter at the beginning
     * @param startWakeupCounter start value for the wakeup counter
     */
    void initWakeupCounter(uint16_t startWakeupCounter = 0);

    /**
     * Gets the amount of continuous resets
     */
    uint16_t getFastResetAmount();

    /**
     * Sets the amount of continuous resets
     */
    void setFastResetAmount(uint16_t fastResetAmount);

    /**
     * Gets the amount of wakeups encountered
     */
    uint16_t getWakeupAmount();

    /**
     * Sets the amount of wakeup encountered
     */
    void setWakeupAmount(uint16_t wakeupAmount);

    /**
     * Increase the amount of wakups encountered by one
     */
    void incWakeupAmount(); 
}

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

    template <class T>
    class RTCMem {
    public:
    static T read(uint16_t addr) {
        T result;
        system_rtc_mem_read(RTC_USER_DATA_ADDR + addr, &result, sizeof(result));
        return result;  
    }

    static void write(uint16_t addr, T data) {
        system_rtc_mem_write(RTC_USER_DATA_ADDR + addr, &data, sizeof(data));
    }
    
    static const uint16_t RTC_USER_DATA_ADDR = 65;
    };

    /**
     * Gets the reason for a wakuep event
     */
    uint16_t getResetReason();

    /**
     * Returns true, if the module waked up from deep sleep
     */
    bool isDeepSleepAwake() {
        return getResetReason() == REASON_DEEP_SLEEP_AWAKE;
    }

    /**
     * Returns true, if the module waked up from power up
     */
    bool isPowerUp() {
        return getResetReason() == REASON_DEFAULT_RST;
    }

    /**
     * Gets the time stored in the rtc module
     */
    uint32_t getRtcTime() {
        return system_get_rtc_time();
    }

    /**
     * Initializes the RTC memory entries and places a wakeup counter at the beginning
     * @param startWakeupCounter start value for the wakeup counter
     */
    void initWakeupCounter(uint16_t startWakeupCounter = 0);

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


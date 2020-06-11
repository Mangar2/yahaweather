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
#define __DEBUG
#include "debug.h"
#include "rtc.h"

using namespace RTC;


const uint32_t MAGIC_NUMBER = 0xAABBCCDD;
const int16_t MAGIC_NUMBER_ADDR = 0;
const int16_t WAKEUP_COUNTER_ADDR = 1;
const int16_t START_TYPE_ADDR = 2;
const int16_t NORMAL_RESET = 0;
const int16_t FAST_RESET = 1;

/**
 * Type of start, either "NORMAL_REST" or "FAST_RESET"
 */
uint16_t _startType;

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

bool RTC::isFastReset() {
    return _startType == FAST_RESET;
}

uint16_t RTC::getResetReason() {
    rst_info *resetInfo;
    resetInfo = ESP.getResetInfoPtr();
    PRINT_IF_DEBUG("Reset reason : ");
    PRINTLN_IF_DEBUG((*resetInfo).reason);
    return (*resetInfo).reason;
}

bool RTC::isDeepSleepAwake() {
    return getResetReason() == REASON_DEEP_SLEEP_AWAKE;
}

bool RTC::isPowerUpAwake() {
    return getResetReason() == REASON_DEFAULT_RST;
}

uint32_t RTC::getRtcTime() {
    return system_get_rtc_time();
}

void RTC::initWakeupCounter(uint16_t startWakeupCounter) {
    uint32_t magicNumber = RTCMem<uint32_t>::read(MAGIC_NUMBER_ADDR);
    PRINTLN_VARIABLE_IF_DEBUG(magicNumber)
    if (magicNumber != MAGIC_NUMBER) {
        PRINTLN_IF_DEBUG("Magic number does not match initializing RTC")
        RTCMem<uint32_t>::write(MAGIC_NUMBER_ADDR, MAGIC_NUMBER);
        uint32_t check = RTCMem<uint32_t>::read(MAGIC_NUMBER_ADDR);
        PRINTLN_IF_DEBUG("Checking to read RTC mem")
        PRINTLN_VARIABLE_IF_DEBUG(check)
        if (check != MAGIC_NUMBER) {
            PRINTLN_IF_DEBUG("Fatal cannot write to RTCMem")
        } else {
            setWakeupAmount(startWakeupCounter);
        }
    }
    _startType = RTCMem<uint32_t>::read(START_TYPE_ADDR);
    PRINTLN_VARIABLE_IF_DEBUG(_startType)
    RTCMem<uint32_t>::write(START_TYPE_ADDR, FAST_RESET);
    PRINTLN_IF_DEBUG("Permanent active on next start, press reset to keep this mode")
}


uint16_t RTC::getWakeupAmount() {
    uint16_t wakeupAmount = RTCMem<uint16_t>::read(WAKEUP_COUNTER_ADDR);
    return wakeupAmount;
}

void RTC::setWakeupAmount(uint16_t wakeupAmount) {
    RTCMem<uint16_t>::write(WAKEUP_COUNTER_ADDR, wakeupAmount);  
    RTCMem<uint32_t>::write(START_TYPE_ADDR, NORMAL_RESET);
    PRINTLN_IF_DEBUG("Normal reset")
}

void RTC::incWakeupAmount() {
    setWakeupAmount(getWakeupAmount() + 1);
}


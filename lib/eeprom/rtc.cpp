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
const int16_t FAST_RESET_ADDR = 3;
const int16_t NORMAL_RESET = 0;
const int16_t FAST_RESET = 1;

template <class T>
class RTCMem {
public:
    static T read(uint16_t addr) {
        T result;
        system_rtc_mem_read(RTC_USER_DATA_ADDR, &result, sizeof(result));
        return result;  
    }

    static void write(uint16_t addr, T data) {
        system_rtc_mem_write(RTC_USER_DATA_ADDR, &data, sizeof(data));
    }

    static const uint16_t RTC_USER_DATA_ADDR = 65;
};

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
    uint32_t startValue = RTCMem<uint32_t>::read(MAGIC_NUMBER_ADDR);
    if (startValue != MAGIC_NUMBER) {
        RTCMem<uint32_t>::write(MAGIC_NUMBER_ADDR, MAGIC_NUMBER);
        setWakeupAmount(startWakeupCounter);
        setFastResetAmount(0);
    }
    uint32_t startType = RTCMem<uint32_t>::read(START_TYPE_ADDR);
    if (startType == FAST_RESET) {
        setFastResetAmount(getFastResetAmount() + 1);
    }
    RTCMem<uint32_t>::write(START_TYPE_ADDR, FAST_RESET);
}

uint16_t RTC::getFastResetAmount() {
    return RTCMem<uint16_t>::read(FAST_RESET_ADDR);
}

void RTC::setFastResetAmount(uint16_t fastResetAmount) {
    return RTCMem<uint16_t>::write(FAST_RESET_ADDR, fastResetAmount);
}

uint16_t RTC::getWakeupAmount() {
    uint16_t wakeupAmount = RTCMem<uint16_t>::read(WAKEUP_COUNTER_ADDR);
    return wakeupAmount;
}

void RTC::setWakeupAmount(uint16_t wakeupAmount) {
    RTCMem<uint16_t>::write(WAKEUP_COUNTER_ADDR, wakeupAmount);  
    RTCMem<uint32_t>::write(START_TYPE_ADDR, NORMAL_RESET);
}

void RTC::incWakeupAmount() {
    setWakeupAmount(getWakeupAmount() + 1);
}


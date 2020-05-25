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
#include <Arduino.h>
#include <EEPROM.h>
#include "debug.h"
#include "eeprommanage.h"

/**
 * Provided function to store and read a configuration from EEPROM
 */
namespace EEPROMAccess {

    void init() {
      EEPROM.begin(EEPROM_SIZE);
    }

    /**
     * dumps the EEPROM entry for debugging
     */
    void dump() {
    IF_DEBUG(
        uint8_t data;
        char buf[16];
        char chData[17];
        chData[0] = 0;
        chData[16] = 0;
        for (uint16_t i = 0; i < EEPROM_SIZE; i++) {
            if (i % 16 == 0) {
                PRINTLN_IF_DEBUG(chData);
                sprintf(buf, "%04x ", i);
                PRINT_IF_DEBUG(buf);
            }
            data = EEPROM.read(i);
            chData[i% 16] = (data < 32 || data > 128) ? ' ' : char(data);
            sprintf(buf, "%02x ", data);
            PRINT_IF_DEBUG(buf);
            delay(1);
        }
        PRINTLN_IF_DEBUG(chData);
    )
    }

    /**
     * Reads a byte array from EEPROM
     */
    void read(uint16_t baseAddress, uint8_t* data, uint16_t size) {
        if (baseAddress + size < EEPROM_SIZE) {
            for (uint16_t i = 0; i < size; i++)
            {
                data[i] = (uint8_t) EEPROM.read(i + baseAddress);
            }
        }
    }

    /**
     * Write a byte array to EEPROM
     */
    void write(uint16_t baseAddress, const uint8_t* data, uint16_t size) {
        if (baseAddress + size < EEPROM_SIZE) {
            for (uint16_t i = 0; i < size; i++)
            {
                EEPROM.write(i + baseAddress, data[i]);
            }
        }
    }

}

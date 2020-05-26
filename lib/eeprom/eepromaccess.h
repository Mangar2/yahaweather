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
#include <Arduino.h>

/**
 * Provided function to store and read a configuration from EEPROM
 */
namespace EEPROMAccess {
    const uint16_t EEPROM_SIZE = 512;

    /**
     * Initializes the EEPROM
     */
    void init();

    /**
     * dumps the EEPROM entry for debugging
     */
    void dump();

    /**
     * Reads a byte array from EEPROM
     * @param baseAddress start address in the eeprom
     * @param data data to write
     * @param size size of the data structure
     */
    void read(uint16_t baseAddress, uint8_t* data, uint16_t size);
    
    /**
     * Write a byte array to EEPROM
     * @param baseAddress start address in the eeprom
     * @param data data to write
     * @param size size of the data structure
     */
    void write(uint16_t baseAddress, const uint8_t* data, uint16_t size);

    /**
     * Commits all write accesses
     */
    void commit();

}

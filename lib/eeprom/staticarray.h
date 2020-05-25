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
 * Provides an array with a constant size. Used to store configuration in the EPROM
 */
template<class T, uint16_t size>
class StaticArray {
public:
    uint16_t getSize() { return size; }
    T data[size];
};
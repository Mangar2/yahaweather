/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @documentation
 * Provided debugging makros, define __DEBUG in the file for tracing
 */

#pragma once

#ifdef __DEBUG
#include <Arduino.h>
#define IF_DEBUG(param) param
#else
#define IF_DEBUG(param)
#endif

#define PRINTLN_IF_DEBUG(string) IF_DEBUG(Serial.println(string);)
#define PRINT_IF_DEBUG(string) IF_DEBUG(Serial.print(string);)
#define PRINT_IF_DEBUG2(string, param) IF_DEBUG(Serial.print(string, param);)
#define INIT_SERIAL_IF_DEBUG(SERIAL_SPEED) IF_DEBUG(Serial.begin(SERIAL_SPEED); delay(10);)
#define PRINT_VARIABLE_IF_DEBUG(v) IF_DEBUG(Serial.print(F(" "#v": ")); Serial.print(v); Serial.print(" ");)
#define PRINTLN_VARIABLE_IF_DEBUG(v) IF_DEBUG(Serial.print(F(" "#v": ")); Serial.println(v);)

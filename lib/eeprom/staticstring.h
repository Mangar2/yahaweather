/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Provides a string class using a fixed amount of memory used for flash based configuration
 */
#pragma once
#include <Arduino.h>

/**
 * Provides a string with a constant memory length. Used to store configuration in the EPROM
 */
template<uint16_t size>
class StaticString {
public:
    StaticString() {
      operator=("");
    }
    StaticString(const char* str) {
        operator=(str);
    }
    StaticString(const String str) {
        operator=(str.c_str());
    }
    StaticString(const StaticString<size>& str) {
        operator=(str.data);
    }
    StaticString<size>& operator=(const char* str) {
        if (str != 0 && str != data) {
            strncpy(data, str, size);
            data[size - 1] = 0;
        }
        return *this;
    }
    StaticString<size>& operator=(const StaticString<size>& str) {
        operator=(str.data);
        return *this;
    }
    operator String() const {
        return String(getBuffer());
    }
    // Access to the buffer
    const char* getBuffer() const 
    { 
        return data; 
    }
    
	// Compares two strings
	friend bool operator==(const StaticString<size>& aStr1, const StaticString<size>& aStr2)
	{
		return strncmp(aStr1.getBuffer(), aStr2.getBuffer(), size) == 0;
	}

	// Compares two strings
	friend bool operator==(const StaticString<size>& aStr1, const char* apChar)
	{
		return strncmp(aStr1.getBuffer(), apChar, size) == 0;
	}

	// Compares two strings
	friend bool operator==(const char* apChar, const StaticString<size>& aStr1)
	{
		return strncmp(apChar, aStr1.getBuffer(), size) == 0;
	}

	// Compares two strings, not equal
	friend bool operator!=(const StaticString<size>& aStr1, const StaticString<size>& aStr2)
	{
		return strncmp(aStr1.getBuffer(), aStr2.getBuffer(), size) != 0;
	}

    // Compares two strings, not equal
	friend bool operator!=(const StaticString<size>& aStr1, const char* apChar)
	{
		return strncmp(aStr1.getBuffer(), apChar, size) != 0;
	}

	// Compares two strings, not equal
	friend bool operator!=(const char* apChar, const StaticString<size>& aStr1)
	{
		return strncmp(apChar, aStr1.getBuffer(), size) != 0;
	}
private:
    char data[size];
};

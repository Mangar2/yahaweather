/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Provides a tokenizer for JSON formatted strings
 */

#pragma once

#include <Arduino.h>

// This class implements a tokenizer used to tokenize JSON strings
class JSONTokenizer
{
public:
	// CTor
	JSONTokenizer(const String &data) : _data(data), _pos(0), _error("") {}
	// DTor
	~JSONTokenizer(void)
	{
	}

	/**
	* Gets the next token
	*/
	String getNextToken();

	/**
	* Skips an expected token or throws an error
	* @param expectedToken token that is expected
	*/
	String skipExpectedToken(const String& expectedToken) {
		String result = getNextToken();
		if (result != expectedToken) {
			setError(expectedToken);
		}
		return result;
	}

	/**
	 * Sets an error condition
	 * @param text expected value/situation
	 */
	void setError(const String& text) {
		_error = String("Error at pos ") + String(_pos) + String(" expected: ") + text;
	}

private:
	/**
	* Skips all spaces and line breaks
	*/
	void skipSpaces();

	/**
	* Search a string until a special char is found or until line end (if multiLine is false)
	* @param searchCh char to search for
	* @param multiLine true, if the search should pass line ends
	*/
	void findChar(char searchCh, bool multiLine);

	/**
	* Skips all Digits
	*/
	void skipDigits();

	/**
	* Skips all characters allowed in symbols
	*/
	void skipSymbolContinuation();

	String _data;
	uint16_t _pos;
	String _error;
};


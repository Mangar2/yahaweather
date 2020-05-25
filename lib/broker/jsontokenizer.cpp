/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 */

#include "jsontokenizer.h"

void JSONTokenizer::skipSpaces()
{
	while ((_data.charAt(_pos) == ' ') || (_data.charAt(_pos) == '\n'))
		_pos++;
}

void JSONTokenizer::findChar(char searchCh, bool multiLine)
{
	while ((_data.charAt(_pos) != searchCh) && (_data.charAt(_pos) != 0))
	{
		if (!multiLine && (_data.charAt(_pos) == '\n'))
			break;
		_pos++;
	}
}

void JSONTokenizer::skipDigits()
{
	while ((_data.charAt(_pos) >= '0') && (_data.charAt(_pos) <= '9'))
		_pos++;
}

void JSONTokenizer::skipSymbolContinuation()
{
	for (;; _pos++)
	{
		const char curCh = _data.charAt(_pos);
		if (!(((curCh >= 'a') && (curCh <= 'z')) ||
			((curCh >= '0') && (curCh <= '9')) ||
			((curCh >= 'A') && (curCh <= 'Z')) ||
			(curCh == '_') || (curCh == '+') ||
			(curCh == '#') || (curCh == '=') ||
			(curCh == ':') || (curCh == '-') || (curCh == '/')))
			break;
	}
}

String JSONTokenizer::getNextToken()
{
	if (_error != "") {
		return "";
	}
	skipSpaces();
	uint16_t tokenStartPos = _pos;
	char curCh = _data.charAt(_pos);

	switch (curCh)
	{
	case 0:
		break;
	case '[':
	case ']':
	case '{':
	case '}':
	case '(':
	case ')':
	case '<':
	case '>':
	case '.':
	case '*':
	case '!':
	case '?':
		// Special single char tokens
		_pos++;
		break;
	case '"':
		// Scan a string
		_pos++;
		findChar('"', false);
		_pos++;
		break;
	default:
		if (((curCh >= 'a') && (curCh <= 'z')) ||
			((curCh >= '0') && (curCh <= '9')) ||
			((curCh >= 'A') && (curCh <= 'Z')))
		{
			skipSymbolContinuation();
		}
		else
			_pos++; // Unknown char
	}
	return _data.substring(tokenStartPos, _pos);
}


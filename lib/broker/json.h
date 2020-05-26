/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Provides a class to access a JSON formatted string by a JSON path
 */

#pragma once
#include <Arduino.h>

/**
 * Creates a json property string
 * @param name property name
 * @param value property string value
 */
String jsonStringProperty(String name, String value);

/**
 * Creates a json object property string
 * @param name property name
 * @param object json definition of an object or array or non-string value as string
 */
String jsonObjectProperty(String name, String object);

class JSONTokenizer;

class JSON {
public:
	JSON(const String& jsonString) :_jsonString(jsonString) {}

	/**
	* Gets an element from the JSON fromatted string based on a json path
	* @param jsonPath path of the for a.b[x] (as in javaScript)
	*/
	String getElement(String jsonPath) const;

private:
	String _jsonString;

	/**
	* Gets the content of an array
	* @param json string tokenizer
	* @returns Array content
	*/
	String getArrayContent(JSONTokenizer& json) const;

	/**
	* Gets the content of an object
	* @param json string tokenizer
	* @returns Object content
	*/
	String getObjectContent(JSONTokenizer& json) const;

	/**
	* Gets the next object of a json
	* @param json string tokenizer
	*/
	String getNextObject(JSONTokenizer& json) const;


	/**
	* Searches the start position of a property of a json object
	* @param json string tokenizer
	* @param property property name
	* @returns true, if found
	*/
	bool searchPropertyInObject(JSONTokenizer& json, const String& property) const;

	/**
	* Searches the start position of an array element
	* @param json string tokenizer
	* @param index index of the array element
	* @returns true, if found
	*/
	bool searchArrayElement(JSONTokenizer& json, uint16_t index) const;

	/**
	* Recursively extracts a substring from a json string by following the path
	* @param json string in JSON format
	* @param path string in json path format
	*/
	String getElementRec(JSONTokenizer& json, JSONTokenizer& path) const;

};

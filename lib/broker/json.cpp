/**
* This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
* "as is", without any support, and with no warranty, express or implied, as to its usefulness for
* any purpose.
*
* @author Volker B�hm
* @copyright Copyright (c) 2020 Volker B�hm
* @brief
* Provides a class to connect to WLAN
*/

#include "json.h"
#include "jsontokenizer.h"

/**
* Creates a json property string
* @param name property name
* @param value property string value
*/
String jsonStringProperty(String name, String value) {
	return String("\"" + name + "\": \"" + value + "\"");
}

/**
* Creates a json object property string
* @param name property name
* @param object json definition of an object or array or non-string value as string
*/
String jsonObjectProperty(String name, String object) {
	return String("\"" + name + "\": " + object);
}

String unquote(const String& quotedString) {
    String result;
    if (quotedString.charAt(0) == '"' && quotedString.charAt(quotedString.length() - 1) == '"') {
		result = quotedString.substring(1, quotedString.length() - 1);
	} else {
        result = quotedString;
    }
    return result;
}

/**
  * Gets the content of an array
  * @param json string tokenizer
  * @returns Array content
  */
String JSON::getArrayContent(JSONTokenizer& json) const {
	String object = getNextObject(json);
	String result = "";
	String tk;
	while (object != "" && object != "]") {
		result += object;
		tk = json.getNextToken();
		if (tk == "") {
			break;
		}
		else if (tk == ",") {
			result += tk;
			object = getNextObject(json);
		}
		else if (tk == "]") {
			break;
		} else {
			json.setError(", or ]");
		}
	}
	result += tk;
	return result;
}

/**
* Gets the content of an array
* @param json string tokenizer
* @returns Array content
*/
String JSON::getObjectContent(JSONTokenizer& json) const {
	String propertyName = json.getNextToken();
	String result = propertyName;
	while (propertyName != "}" && propertyName != "") {
		result += json.skipExpectedToken(":");
		result += getNextObject(json);
		String tk = json.getNextToken();
		if (tk == "}") {
			result += tk;
			break;
		}
		else if (tk == ",") {
			result += tk;
			propertyName = json.getNextToken();
			result += propertyName;
		}
		else {
			json.setError(", or  }");
		}
	}
	return result;
}

/**
 * Gets the next object of a json
 * @param json string tokenizer
 */
String JSON::getNextObject(JSONTokenizer& json) const {
	String tk = json.getNextToken();
	String result = tk;
	if (tk == "{") {
		result += getObjectContent(json);
	}
	else if (tk == "[") {
		result += getArrayContent(json);
	}
	else if (tk == "]" || tk == "}") {
		return tk;
	} 
	return result;
}


/**
* Gets a property from a json object
* @param json string tokenizer
* @param property property name
* @returns true, if found
*/
bool JSON::searchPropertyInObject(JSONTokenizer& json, const String& property) const {
	String tk;
	bool result = false;
	json.skipExpectedToken("{");
	tk = json.getNextToken();
	while (tk != "}") {
		json.skipExpectedToken(":");
		if (tk == "\"" + property + "\"") {
			result = true;
			break;
		}
		getNextObject(json);
		tk = json.getNextToken();
		if (tk == ",") {
			tk = json.getNextToken();
		} else if (tk != "}") {
			json.setError("} or ,");
			break;
		}
	}
	return result;
}

/**
* Gets a property from a json object
* @param json string tokenizer
* @param property property name
* @returns true, if found
*/
bool JSON::searchArrayElement(JSONTokenizer& json, uint16_t index) const {
	json.skipExpectedToken("[");
	String tk;
	while (index > 0 && tk != "]") {
		getNextObject(json);
		tk = json.getNextToken();
		if (tk == ",") {
			index--;
		} else if (tk != "]") {
			json.setError("] or ,");
			break;
		} 
	}
	return index == 0;
}


/**
* Recursively extracts a substring from a json string by following the path
* @param json string in JSON format
* @param path string in json path format
*/
String JSON::getElementRec(JSONTokenizer& json, JSONTokenizer& path) const {
	String tk = path.getNextToken();
	String result = "";
	if (tk == ".") {
		tk = path.getNextToken();
	}
	if (tk == "") {
		result = getNextObject(json);
	}
	else if (tk == "[") {
		String indexString = path.getNextToken();
		uint16_t index = indexString.toInt();
		path.skipExpectedToken("]");
		if (searchArrayElement(json, index)) {
			result = getElementRec(json, path);
		}
	}
	else {
		if (searchPropertyInObject(json, tk)) {
			result = getElementRec(json, path);
		}
	}
	return result;
}

String JSON::getElement(String jsonPath) const {
	JSONTokenizer json(_jsonString);
	JSONTokenizer path(jsonPath);
	String result = unquote(getElementRec(json, path));
	return result;
}

jsonObject_t JSON::parseObject(String jsonPath) const {
    jsonObject_t result;
    String subObject = getElement(jsonPath);
    JSONTokenizer json(subObject);
	json.skipExpectedToken("{");
	String tk = json.getNextToken();
	while (tk != "}") {
		json.skipExpectedToken(":");
		result[unquote(tk)] = unquote(getNextObject(json));
		tk = json.getNextToken();
		if (tk == ",") {
		    tk = json.getNextToken();
		} else if (tk != "}") {
			json.setError("} or ,");
			break;
		}
	}
    return result;
}
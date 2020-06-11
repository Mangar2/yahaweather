/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Provides a class to steer Switch based on humidity measurement
 */
#pragma once

#include <Arduino.h>
#include <message.h>
#include <map>

class Switch
{
public:

    Switch(); 

    /**
     * Gets the configuration as key/value map
     */
    std::map<String, String> get();

    /**
     * Sets the configuration from a key/value map
     * @param config configuration settings in a map
     */
    void set(std::map<String, String> config);

    static const char* htmlForm;

private:
};
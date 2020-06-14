/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Describes a html page
 */
#pragma once

#include <Arduino.h>

struct HtmlPageInfo {
    HtmlPageInfo() : hasForm(false) {}
    HtmlPageInfo(String form, String uri, String menuName) 
        : hasForm(true), _form(form), _uri(uri), _menuName(menuName) 
    {}
    bool hasForm;
    String _form;
    String _uri;
    String _menuName;
};

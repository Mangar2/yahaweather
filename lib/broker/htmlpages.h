/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Provides a web page with a configuration form
 */
#pragma once

#include <Arduino.h>
#include "htmlstructure.h"
#include "htmlstyle.h"
#include "css.h"
#include "htmltopnav.h"

String htmlPage = 
    String(htmlStart) + 
    String(htmlStyle) + 
    "</head><body>";

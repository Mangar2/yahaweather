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

const char* htmlClientForm = R"htmlform(
<form action="/client" method="POST">
<label for="clientname">Client name</label>
<input type="text" id="clientname" name="clientName" [value]="clientName">
<label for="basetopic">Base topic</label>
<input type="text" id="basetopic" name="baseTopic" [value]="baseTopic">
<label for="subscribeto">Subscribe topic</label>
<input type="text" id="subscribeto" name="subscribeTo" [value]="subscribeTo">
<input type="submit" value="Submit">
</form>
)htmlform";
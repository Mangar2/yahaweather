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

const char* htmlWeatherForm = R"htmlweather(
<form>
<label for="temperature">Temperature</label>
<input type="text" id="temperature" readonly [value]="temperature">
<label for="humidity">Humidity</label>
<input type="text" id="humidity" readonly [value]="humidity">
<label for="pressure">Barometric pressure</label>
<input type="text" id="pressure" readonly [value]="pressure">
<label for="battery">Battery voltage</label>
<input type="text" id="battery" readonly [value]="battery">
</form>
)htmlweather";
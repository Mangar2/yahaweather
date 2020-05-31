/**
 * This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 3. It is furnished
 * "as is", without any support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2020 Volker Böhm
 * @brief
 * Provides a configuration form for battery management
 */

#pragma once

const char* htmlBatteryForm = R"htmlform(
<form action="/battery" method="POST">
<label for="voltage">Current voltage</label>
<input type="text" id="voltage" name="voltage" [value]="voltage">
<label for="calibration">Voltage calibration factor</label>
<input type="text" id="calibration" name="voltageCalibrationFactor" [value]="voltageCalibrationFactor">
<label for="highVoltage">High voltage</label>
<input type="text" id="highVoltage" name="highVoltage" [value]="highVoltage">
<label for="lowVoltage">Low voltage</label>
<input type="text" id="lowVoltage" name="lowVoltage" [value]="lowVoltage">
<label for="highTime">High voltage sleep time in seconds</label>
<input type="text" id="highTime" name="highVoltageSleepTimeInSeconds" [value]="highVoltageSleepTimeInSeconds">
<label for="normalTime">Normal voltage sleep time in seconds</label>
<input type="text" id="normalTime" name="normalVoltagesleepTimeInSeconds" [value]="normalVoltagesleepTimeInSeconds">
<label for="normalTime">Low voltage sleep time in seconds</label>
<input type="text" id="lowTime" name="lowVoltageSleepTimeInSeconds" [value]="lowVoltageSleepTimeInSeconds">
<input type="submit" value="Submit">
</form>
)htmlform";
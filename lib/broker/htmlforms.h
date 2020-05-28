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

const char* htmlMainForm = R"htmlform(
<div class="container">
  <form action="/" method="POST">
    <p>
        <label for="ssid">Wlan name</label>
        <input type="text" id="ssid" name="wlanssid" placeholder="ssid...">

        <label for="passwd">Wlan Password</label>
        <input type="password" id="passwd" name="wlanpassword" placeholder="password...">
    </p>
    <label for="brokerhost">Broker host</label>
    <input type="text" id="brokerhost" name="brokerhost" placeholder="Broker host...">

    <label for="brokerport">Broker port</label>
    <input type="number" id="brokerport" name="brokerport" value="8183">

    <label for="clientname">Client name</label>
    <input type="text" id="clientname" name="clientname" value="ESP8266/weather/station">

    <label for="basetopic">Base topic</label>
    <input type="text" id="basetopic" name="basetopic" value="outdoor/garden/esp8266/weather">

    <input type="submit" value="Submit">
  </form>
</div>
)htmlform";
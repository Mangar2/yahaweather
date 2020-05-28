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

const char* htmlWLANForm = R"htmlwlan(
<div class="container">
  <form action="/wlan" method="POST">
    <label for="ssid">Wlan name</label>
    <input type="text" id="ssid" name="wlanssid" placeholder="ssid...">

    <label for="passwd">Wlan Password</label>
    <input type="password" id="passwd" name="wlanpassword" placeholder="password...">

    <input type="submit" value="Submit">
  </form>
</div></body></html>
)htmlwlan";
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

const char* htmlBrokerForm = R"htmlform(
<div class="container">
  <form action="/broker" method="POST">
    <label for="brokerhost">Broker host</label>
    <input type="text" id="brokerhost" name="brokerHost" placeholder="Broker host...", [value]="brokerHost">

    <label for="brokerport">Broker port</label>
    <input type="number" id="brokerport" name="brokerPort" [value]="brokerPort">

    <input type="submit" value="Submit">
  </form>
</div></body></html>
)htmlform";
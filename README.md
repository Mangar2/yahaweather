# Yaha ESP8266 Weather Statioin

## Abstract

Version 0.3 of the yaha weather station. It is currently under construction. It is already partially usable. Use PIO to compile and install it.

## Features

The yaha weather station provides the following abilities:

- Control a BME280, measuring temperature, humidity and barometric pressure.
- Sends temperature, humidity, barometric pressure, wakeup-time and battery voltage to the yaha broker
- Sends a binary "rain" information from a rain sensor
- Minimizes power consumption
- Provides a web interface for configuraiton and an automatic soft-ap mode for setting up the WLAN credentials

### AP mode/WLAN mode

The device can operate in an existing WLAN (station mode) or in Access Point (AP) mode. It selects the mode automatically. If the WLAN is not yet configured, it will automatically switch to AP mode. If the WLAN is configured, it will try to connect to the WLAN. 

- **SSID**: YAHA_ESP_AP
- **Passwort**: yahaadmin
- **Link**: <http://192.168.4.1> (Port 80)

### On mode/Battery mode

Normally, the device operates in "battery mode". It will wake up, send data to the broker and go to sleep. By pressing reset twice in a row (with a delay between 0,5s and 1s) it will switch to always on mode.

In Battery mode, it will run about 5s-10s, depending on the WLAN signal. It will then go to sleep for some time. Sleep time can be configured and can be dependend on the voltage of the battery.

## Customizing the program

The yaha station is flexible and designed to support multiple functionalities. Functionalities are changed by uncommenting the features in the main.cpp file.

```Code
// #define __DEBUG
// #define __BME
// #define __IRRIGATION
// #define __SWITCH
// #define __MOTION
// #define __BATTERY
// #define __RTC
// #define __RAIN
```Code

Adding a feature by uncommenting it will:

- Include the neccessary headers
- Call the initialization in "setup"
- Add a page to the web interface
- Send/Receive corresponding data to/from the yaha mqtt broker

### Debug

Uncomment the debug definition, only if you are debugging the source code. The Yaha Station has several trace points traicing information to the serial interface, if __DEBUG is defined. The following calls are debug traces:
- PRINT_IF_DEBUG prints an information
- PRINTLN_IF_DEBUG prints an information and adds a carrage return
- PRINT_VARIABLE_IF_DEBUG prints the name and the content of a variable
- PRINTLN_VARIABLE_IF_DEBUG prints the name and the content of a variable and adds a carrage return

### BME

Uncomment this, if you added a BME 280 sensor measuring temperature, humidity and barometric pressure. 

## Yaha Broker

It needs the Yaha Broker to be integrated in a home automation system. See Mangar2/yaha to install the broker.

## Installation

Upload it with platformio or any other tool handling sources with "true" .cpp file (no .ino file)

## Configuration

- Upload the software, than change the WLAN Access Point of your device to **YahaWeather**
- Open the Webpage in your favorite browser, <http://192.168.4.1,> no password needed
- Go to the "WLAN" submenu and enter the credentials (ssid, password) of your wlan, press submit (ignore that it looks disabled on mobile devices)
- Reset the ESP8266 twice with with a delay of about 0.5 seconds. The device will switch to "permanent" mode, whenever two resets with small delay are detected.
- Connect your device again with your standard WLAN. The device should be connected to this WLAN now. Open the same Webpage <http://192.168.4.1> again.
- Change any configuration you like.
- Reset the ESP once, it is now in working mode.

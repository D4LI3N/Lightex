# Lightex
Lightex is a device that can automatically control lights by detecting the proximity of your chosen slave device, using BT's iBeacon technology.

iBeacon is a small, cheap and long-lasting device that sends signals periodically to the master device(Lightex), also in theory, any device can be used as an iBeacon device as long as it supports Bluetooth 4.0+. (such as your smartphone most conveniently)

The project is primarily intended to be used with RV light systems, but can potentially be used to control any 10Amp AC/DC device, not just RV lights.

# Project Features
- binary control of light installation (ON/OFF)
- proximity detection of the iBeacon device (about 8m of spherical range)
- light detection using photoresistor extension (day/night)
- dual power input 5V/12V (can be powered by unregulated 5V or 12V DC power source)
- timeout when not in range (1min for lights to go in OFF/STANDBY mode)
- when using a smartphone, a Bluetooth switch can also be used to turn the lights ON/OFF
- supports multiple iBeacon devices (you can use an unlimited number of iBeacon devices with the same ID)
- USB debugging (serial debug output)

# Operation
When the photoresistor extension is connected to the Lightex, it can detect day or night, and only turns ON by NIGHT.

If the photoresistor extension is not connected, it detects that as NIGHT, and in that case, it's always ON.
If Lightex is in NIGHT mode, as long as the user(iBeacon) is in range, the lights are turned ON, when the user gets out of range, the device waits for adjustable delay(default 1min), then the lights turn OFF, and Lightex goes into STANDBY mode.
In STANDBY mode, the device checks if the user is in range.
When the user again gets in range, the lights turn ON instantly without delay.

‍

# Project Description
## The main unit:
Lightex custom-made board
## Arbitrary unit:
Photoresistor extension
‍


# Technology Overview
## Software:
C/C++.

## Hardware:
### Lightex board:
- ESP32 chip (NodeMCU, ESP-WROOM-32, 38-pin board)
- 5VDC/10A relay board (with optocoupler circuit isolation)
- 24V DC-DC Adjustable step-down buck power converter
- photoresistor extension

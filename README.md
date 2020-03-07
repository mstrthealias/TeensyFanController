# Teensy Fan Controller

*Work in progress... An initial release is not yet available.*

A PC fan controller with temperature sensor input, for **Teensy 3.x** USB Development Board(s).

Supports up to 6 fans (4pin/PWM), and up to 5 temperature sensors (thermistors). Each fan may individually operate in PID, temperature-percent table, or fixed-percent control modes.

Designed to control radiator fans on a PC water cooling loop (to maintain supply water temperature), but may be configured for other purposes (fe., see [Router Cooling Demo][6]).


## Documentation

The project is **documented using Gitbook**, and accessible at **[https://mstrthealias.gitbook.io/teensy-fan-controller/][7]**.


## Hardware

Hardware is described in the [Hardware][5] folder:

* [Prototype - Hand-made (Schmart Board based)][3]: uses over-the-shelf components (OK for single fan/single sensor builds)
* [Prototype - Simple PCB][4]: requires PCB fabrication (gerber files provided) and through-hole soldering
* [Prototype - Breadboard][2]: breadboard example with a single fan/single sensor


## Software

### Qt UI

[Teensy Fan Controller UI (Qt5)](https://github.com/mstrthealias/TFC-ui): provides a user interface to configure and monitor the fan controller.

### Linux Kernel module (hwmon)

[Fork of liquidtux](https://github.com/mstrthealias/liquidtux): Linux kernel module for reading temperatures and fan RPMs using *lm-sensors* (or *psensor*, as used below).  Tested on Ubuntu 18.04.


## Compatibility

- Teensy 3.5
- Teensy 3.2


## Notes

Releases will be published for supported devices. Teensy 3.x should be used for development.


## Credits

Icons made by [Freepik](https://www.flaticon.com/authors/freepik) from [www.flaticon.com](https://www.flaticon.com/):

* ui_qt5/images/log.svg
* ui_qt5/images/worldwide.svg


### Code
- https://github.com/br3ttb/Arduino-PID-Library/ (License MIT)
- https://learn.adafruit.com/thermistor/using-a-thermistor (License Apache?)
- https://stackoverflow.com/a/10990893/1345237 (License ?)
- https://github.com/apmorton/teensy-template (License ?)



[2]: Hardware/Breadboard/
[3]: Hardware/Schmart%20Board/
[4]: Hardware/Simple/
[5]: Hardware/
[6]: https://mstrthealias.gitbook.io/teensy-fan-controller/demo/router-cooling-demo
[7]: https://mstrthealias.gitbook.io/teensy-fan-controller/

# Teensy Fan Controller

*Documentation: [https://mstrthealias.gitbook.io/teensy-fan-controller/][7]*

A PC fan controller with temperature sensor input, for **[Teensy](https://www.pjrc.com/teensy/index.html) 3.x** USB Development Board(s).

Supports up to 6 fans (4pin/PWM), and up to 5 temperature sensors. Each fan may individually operate in PID, temperature-percent table, or fixed-percent control modes. Configuration is saved onto the device, and software is not required other than to configure the controller.


## Documentation

The project is **documented using Gitbook**, and accessible at **[https://mstrthealias.gitbook.io/teensy-fan-controller/][7]**.


## Releases

- v0.1.0
  - [Firmware (Teensy 3.2, 3.5)](https://github.com/mstrthealias/TeensyFanController/releases/tag/v0.1.0)
  - [Configuration UI (Windows 10)](https://github.com/mstrthealias/TFC-ui/releases/tag/v0.1.0)


## Hardware

* [Prototype - Simple PCB][4]: requires PCB fabrication (gerber files provided) and through-hole soldering
* [Prototype - Breadboard][2]: breadboard example with a single fan/single sensor
* [Prototype - Hand-made (Schmart Board based)][3]: uses over-the-shelf components (OK for single fan/single sensor builds)


## Software

### Qt UI

[Teensy Fan Controller UI (Qt5)](https://github.com/mstrthealias/TFC-ui): provides a user interface to configure or monitor the fan controller (only needed for initial configuration).

### Linux Kernel module (hwmon)

[Fork of liquidtux](https://github.com/mstrthealias/liquidtux): Linux kernel module for reading temperatures and fan RPMs using *lm-sensors* or *psensor*.  Tested on Ubuntu 18.04.


## Credits

Icons made by [Freepik](https://www.flaticon.com/authors/freepik) from [www.flaticon.com](https://www.flaticon.com/):

* ui_qt5/images/log.svg
* ui_qt5/images/worldwide.svg


### Code
- https://github.com/br3ttb/Arduino-PID-Library/ (License MIT)
- https://learn.adafruit.com/thermistor/using-a-thermistor (License Apache?)
- https://stackoverflow.com/a/10990893/1345237 (License ?)
- https://github.com/apmorton/teensy-template (License ?)


## Simple 001 PCB

![Simple 001 PCB](doc/images/simple001.photo.1.png)



[2]: https://mstrthealias.gitbook.io/teensy-fan-controller/hardware/breadboard
[3]: https://mstrthealias.gitbook.io/teensy-fan-controller/hardware/prototype
[4]: https://mstrthealias.gitbook.io/teensy-fan-controller/hardware/simple-pcb
[5]: Hardware/
[6]: https://mstrthealias.gitbook.io/teensy-fan-controller/demo/router-cooling-demo
[7]: https://mstrthealias.gitbook.io/teensy-fan-controller/

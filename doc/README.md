# Teensy Fan Controller

A PC fan controller with temperature sensor input, for **Teensy 3.x** USB Development Board(s).

Supports up to 6 fans (4pin/PWM), and up to 5 temperature sensors (thermistors). Each fan may individually operate in PID, temperature-percent table, or fixed-percent control modes.

Designed to control radiator fans on a PC water cooling loop (to maintain supply water temperature), but may be configured for other purposes (for example, see [Hardware/Router cooling demo][1]).

[1]: demo/router-cooling-demo


## Hardware

The following hardware has been used with Teensy Fan Controller:

- [Breadboard][3]: breadboard example with a single fan/single sensor
- [Prototype - Hand-made (Schmart Board based)][4]: uses over-the-shelf components
- [Prototype - Simple PCB][5]: requires PCB fabrication (gerber files provided) and through-hole soldering

Related files are available in the [TeensyFanController/Hardware][2] folder.

[2]: https://github.com/mstrthealias/TeensyFanController/tree/master/Hardware
[3]: breadboard
[4]: prototype
[5]: simple-pcb

### Hardware Interface

The Teensy Micro USB port is used to configure and monitor the fan controller.

{% hint style="info" %}
It is possible to connect the Micro USB port to an internal motherboard USB 1.1/2.0 header, by making a custom cable, or purchasing a *Micro USB 5pin Male to 1x 5Pin Female 0.1" USB Header PCB Motherboard Cable*.
{% endhint %}


## Software

### Qt UI

[Teensy Fan Controller UI (Qt5)](https://github.com/mstrthealias/TFC-ui): provides a user interface to configure and monitor the fan controller.

### Linux Kernel module (hwmon)

[Fork of liquidtux](https://github.com/mstrthealias/liquidtux): Linux kernel module for reading temperatures and fan RPMs using *lm-sensors* (or *psensor*, as used below).  Tested on Ubuntu 18.04.





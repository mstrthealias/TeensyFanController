# Teensy Fan Controller

*Work in progress... An initial release is not yet available. However, you're welcome to build and flash using Arduino Studio (with Teensyduino add-on).*

A PC fan controller using temperature sensors/thermistors, for Teensy 3.x USB Development Board(s).

Supports up to 6 PWM (4pin) fans, and up to 5 temperature sensors (supply water temp, return water temp, case temp, aux1 temp, aux2 temp). Each fan may individually operate in PID, percent-table, or fixed-speed (percent) control modes.

Primarily designed to control radiator fans (4pin/PWM) for a PC water cooling loop, using feedback from a water supply temperature sensor. Configurable to prefer quiet-but-safe operation in all cases.

Prototype hardware (supporting 6 fans and 4 sensors) is described in the *Hardware* folder.  Schematic and PCB (Gerber files) will be provided later.


## Interface

The Teensy Micro USB port is used to configure and monitor the fan controller.

Note: it is possible to connect the Micro USB port to an internal motherboard USB 1.1/2.0 header, by making a custom cable, or purchasing a *Micro USB 5pin Male to 1x 5Pin Female 0.1" USB Header PCB Motherboard Cable*.


## Control Modes

### PID Control Mode

The PID controllers satisfy a temperature setpoint, by changing fan PWM duty cycle(s).  By default, all fans operate in PID mode for supply water temperature control.

The controller may operate up to 4 PID controllers, one per the following temperature sensors: supply water temp, case temp, aux1 temp, and aux2 temp.


#### Supply Water Temperature Control

A supply water temperature setpoint reset schedule is enabled by default, to adjust for room and/or case temperature changes. By default, the setpoint adjusts between 28 and 31 degC, at 0.5 degC increments. If a case temperature sensor is connected, the reset schedule can be limited by a difference between water supply and case temperature (allowing fan spin-up when case temperature does not change).


### Percent-Table Mode

Each fan may be individually configured to operate using its own *Temperature to Fan Percent table*.  This is like BIOS custom fan configurations.

Any one of the five temperature sensors, or DeltaT (return temp - supply temp), may be used as the temperature source.


### Fixed Mode

Each fan may be individually configured to operate using a fixed speed (percent).


## Software

### Qt UI

[Teensy Fan Controller UI (Qt5)](https://github.com/mstrthealias/TFC-ui): provides a user interface to configure and monitor the fan controller.


### Linux Kernel modules (hwmon)

To be published (liquidtux).


## Configuration
TODO

### Fan(s)
TODO

#### Hardware Setup
TODO
- PWM Pin
- RPM Pin

### Sensor(s)
TODO

#### Hardware Setup
TODO
- Pin
- Thermistor Beta Coefficient
- Thermistor Nominal Resistance (fe. 10k, 50k)
- Thermistor Series Resistance (value of series resistor, fe. 10000 for 10k 1% resistor)

### PID Setup
TODO




## Hardware

Mostly tested with a Teensy 3.5, however the prototype board is also pin-compatible with the Teensy LC.  The prototype board was built using a Schmart Board; details are provided in the *Hardware* folder.

If used on a Teensy LC, the config will not be savable on the device (and will always load compiled defaults on start).


### Compatibility

- Teensy 3.5
- Partial: Teensy LC (unable to save config in EEPROM)


### Code
- https://github.com/br3ttb/Arduino-PID-Library/ (License MIT)
- https://learn.adafruit.com/thermistor/using-a-thermistor (License Apache?)
- https://stackoverflow.com/a/10990893/1345237 (License ?)


# Teensy Fan Controller

A PC fan controller using temperature sensors/thermistors, based on Teensy USB Development Board(s).

Designed to control radiator fans (4pin/PWM) for a PC water cooling loop using feedback from a water supply temperature sensor.  Supports PID and percent-table control modes (per fan).  Easily configurable to prefer quiet-but-safe operation in all cases.

Prototype hardware is described in the *Hardware* folder.  Schematic and PCB (Gerber files) will be provided later.


## Interface

The Teensy Micro USB port is used to configure and monitor the fan controller.

Note: it is possible to connect the Micro USB port to an internal motherboard USB 1.0/2.0 header, by making a custom cable, or purchasing a *Micro USB 5pin Male to 1x 5Pin Female 0.1" USB Header PCB Motherboard Cable*.


### PID Control Mode

The PID controller satisfies a supply water temperature setpoint, by changing fan PWM duty cycle(s).  By default, all fans operate in PID mode.

A supply water temperature setpoint reset schedule is enabled by default, to adjust for room and/or case temperature changes. By default, the setpoint adjusts between 28 and 31 degC, at 0.5 degC increments. If a case temperature sensor is connected, the reset schedule can be limited by a difference between water supply and case temperature (allowing fan spin-up when case temperature does not change).


### Percent-Table Mode

A single *Temperature to Fan Percent table* is configurable.  This is similar to BIOS custom fan configurations.  Each fan may individually be set to operate in this mode.

Any one of the four temperature sensors, or DeltaT (return temp - supply temp), may be used as the temperature source.


## Software

### Qt UI

Provides a user interface to configure and monitor the fan controller.  To be published.


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


### Compat.

- Teensy 3.5
- Teensy LC


### Code
- https://github.com/br3ttb/Arduino-PID-Library/ (License MIT)
- https://learn.adafruit.com/thermistor/using-a-thermistor (License Apache?)
- https://stackoverflow.com/a/10990893/1345237 (License ?)


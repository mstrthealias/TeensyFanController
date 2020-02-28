# Teensy Fan Controller

*Work in progress... An initial release is not yet available. However, you're welcome to build and flash using Arduino Studio (with Teensyduino add-on).*

A PC fan controller with temperature sensor input, for **Teensy 3.x** USB Development Board(s).

Supports up to 6 fans (4pin/PWM), and up to 5 temperature sensors (thermistors). Each fan may individually operate in PID, temperature-percent table, or fixed-percent control modes.

Designed to control radiator fans on a PC water cooling loop (to maintain supply water temperature), but may be configured for other purposes (for example, see [Hardware/Router cooling demo][2]).

## Hardware

Hardware is described in the [Hardware][5] folder:

* [Prototype - Hand-made (Schmart Board based)][3]: uses over-the-shelf components (OK for single fan/single sensor builds)
* [Prototype - Simple PCB][4]: requires PCB fabrication (gerber files provided) and through-hole soldering
* [Router Cooling Demo][2]: breadboard based experiment using a single fan/single sensor to cool a router to a specific temperature

## Interface

The Teensy Micro USB port is used to configure and monitor the fan controller.

Note: it is possible to connect the Micro USB port to an internal motherboard USB 1.1/2.0 header, by making a custom cable, or purchasing a *Micro USB 5pin Male to 1x 5Pin Female 0.1" USB Header PCB Motherboard Cable*.


## Control Modes

### PID Control Mode

The PID controllers satisfy a temperature setpoint, by changing fan PWM duty cycle(s).  By default, all fans operate in PID mode for supply water temperature control.

The fan controller may operate up to 4 PID controllers, one per the following temperature sensors: supply water temp, case temp, aux1 temp, and aux2 temp.


#### Supply Water Temperature Control / Setpoint Adjustment

TODO

A supply water temperature setpoint reset schedule is enabled by default, to adjust for room and/or case temperature changes. By default, the setpoint adjusts between 28°C and 31.5°C, at 0.25°C increments. If a case temperature sensor is connected, the reset schedule is limited by a difference between supply water and case temperature (allowing fan spin-up when case temperature does not change).

### Percent-Table Mode

Each fan may be individually configured to operate using its own *temperature to fan %-table*.  This is like BIOS custom fan configurations.

Any one of the five temperature sensors, or DeltaT (return temp - supply temp), may be used as a temperature source.


### Fixed Mode

Each fan may be individually configured to operate using a fixed speed (percent).


## Software

### Qt UI

[Teensy Fan Controller UI (Qt5)](https://github.com/mstrthealias/TFC-ui): provides a user interface to configure and monitor the fan controller.

### Linux Kernel module (hwmon)

To be published (liquidtux).


## PID Usage and Example

The default configuration works well on a PC with a water-cooled CPU and an air-cooled GPU:

- Uses a supply water temperature sensor and a case temperature sensor
- Controls supply water temperature to 2-3°C above case temperature (initial setpoint is 28°C)
- Fan output percent is generally between 45%-60%, unless CPU is loaded
- If case temperature is within 2°C of supply water temperature, and fan output percent is greater than 60% for 20 seconds, the setpoint increases by 0.25°C


For example, the fans are controlled to keep supply water temperature at setpoint:

![Example 1.1](doc/images/ex1.1.jpg?raw=true 'Example 1.1')


In the same dataset as *Example 1.1*, and only plotting fan RPMs and DeltaT (return temp - supply temp), observe the fan speeds trailing DeltaT (which represents CPU load in the following example):

![Example 1.2](doc/images/ex1.2.jpg?raw=true 'Example 1.2')


Next, an air-cooled GPU is used for about 30 minutes, showing *setpoint adjustment* keeping fan speeds within the configured range:

![Example 2](doc/images/ex2.jpg?raw=true 'Example 2')


The same behavior is observed, except caused by room temperature changes, in this example:

![Example 3](doc/images/ex3.jpg?raw=true 'Example 3')


When only the CPU is loaded, the setpoint temperature does not change, and the fans speed up as needed:

![Example 4](doc/images/ex4.jpg?raw=true 'Example 4')


### Considerations

A case temperature sensor should be used if using *setpoint adjustment* with PID supply water temperature control:

- When under load and without a case temperature sensor, the setpoint will increase to the maximum before allowing fan speedup
- Without a case temperature sensor, *setpoint adjustment* may achieve the same result as using a supply water *temperature to fan %-table*


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

Note: sensors are labeled as supply water temp, return water temp, case temp, aux1 temp, and aux2 temp.

#### Hardware Setup
TODO
- Pin
- Thermistor Beta Coefficient
- Thermistor Nominal Resistance (fe. 10k, 50k)
- Thermistor Series Resistance (value of series resistor, fe. 10000 for 10k 1% resistor)

### PID Setup
TODO




## Notes

Releases will be published for supported devices. For now, Teensy 3.x should be used (although prototype hardware is pin-compatible with Teensy LC, 3.x, and 4.0).

If used on a Teensy LC, the config will not be savable on the device (and will always load compiled defaults on start).


### Compatibility

- Teensy 3.5
- Partial: Teensy LC (unable to save config in EEPROM)


### Code
- https://github.com/br3ttb/Arduino-PID-Library/ (License MIT)
- https://learn.adafruit.com/thermistor/using-a-thermistor (License Apache?)
- https://stackoverflow.com/a/10990893/1345237 (License ?)




[2]: Hardware/Router%20cooling%20demo/
[3]: Hardware/Schmart%20Board/
[4]: Hardware/Simple/
[5]: Hardware/

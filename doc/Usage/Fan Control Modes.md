# Fan Control Modes

TODO

## PID Control Mode

The PID controllers satisfy a temperature setpoint, by changing fan PWM duty cycle(s).  By default, all fans operate in PID mode for supply water temperature control.

The fan controller may operate up to 4 PID controllers, one per the following temperature sensors: supply water temp, case temp, aux1 temp, and aux2 temp.


### *Setpoint Adjustment* for Supply Water Temperature Control

Supply water temperature *setpoint adjustment* is enabled by default, to handle room and/or case temperature changes. By default, the setpoint adjusts between 28°C and 31.5°C, at 0.25°C increments.

If a case temperature sensor is connected, *setpoint adjustment* is limited by a difference between supply water and case temperature (allowing fan spin-up when case temperature does not change).


## Percent-Table Mode

Each fan may be individually configured to operate using its own *temperature to fan %-table*.  This is like BIOS custom fan configurations.

Any one of the five temperature sensors, or DeltaT (return temp - supply temp), may be used as a temperature source.


## Fixed Mode

Each fan may be individually configured to operate using a fixed speed (percent).

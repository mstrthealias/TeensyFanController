# Fan Control

Each fan individually configures a _control mode_.

The following _control modes_ are supported:

* PID
* Temperature-Percent Table \(%-table\)
* Fixed Percent
* Off

PID and %-table _control modes_ configure a _control source_, which represents what temperature sensor is used.

The following _control sources_ are supported:

* Water Supply Temperature
* Water Return Temperature
* Case Temperature
* Aux1 Temperature
* Aux2 Temperature

{% hint style="info" %}
By default, all fans operate in PID mode for supply water temperature control.
{% endhint %}

## Percent-Table Mode

Each fan may be individually configured to operate using its own _temperature to fan %-table_. This is like BIOS custom fan configurations.

Any one of the five temperature sensors, or DeltaT \(return temp - supply temp\), may be used as a temperature source.

## Fixed Mode

Each fan may be individually configured to operate using a fixed speed \(percent\).

## PID Control Mode

The PID controllers satisfy a temperature setpoint, by changing fan PWM duty cycle\(s\).

The fan controller may operate up to 4 PID controllers, one per the following temperature sensors: supply water temp, case temp, aux1 temp, and aux2 temp.

### _Setpoint Adjustment_ for Supply Water Temperature Control

Supply water temperature _setpoint adjustment_ is enabled by default, to handle room and/or case temperature changes. By default, the setpoint adjusts between 28°C and 31.5°C, at 0.25°C increments.

If a case temperature sensor is connected, _setpoint adjustment_ is limited by a difference between supply water and case temperature \(allowing fan spin-up when case temperature does not change\).

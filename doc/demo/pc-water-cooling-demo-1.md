# Water-cooled CPU and air-cooled GPU

TODO

## PID Usage and Example

TODO

The default configuration works well on a PC with a water-cooled CPU and an air-cooled GPU:

* Uses a supply water temperature sensor and a case temperature sensor
* Controls supply water temperature to 2-3°C above case temperature \(initial setpoint is 28°C\)
* Fan output percent is generally between 45%-60%, unless CPU is loaded
* If case temperature is within 2°C of supply water temperature, and fan output percent is greater than 60% for 20 seconds, the setpoint increases by 0.25°C

For example, the fans are controlled to keep supply water temperature at setpoint:

![Example 1.1](https://github.com/mstrthealias/TeensyFanController/tree/9169de2e0e3d817ff4fb50c9669d37643c585822/doc/images/ex1.1.jpg?raw=true)

In the same dataset as _Example 1.1_, and only plotting fan RPMs and DeltaT \(return temp - supply temp\), observe the fan speeds trailing DeltaT \(which represents CPU load in the following example\):

![Example 1.2](https://github.com/mstrthealias/TeensyFanController/tree/9169de2e0e3d817ff4fb50c9669d37643c585822/doc/images/ex1.2.jpg?raw=true)

Next, an air-cooled GPU is used for about 30 minutes, showing _setpoint adjustment_ keeping fan speeds within the configured range:

![Example 2](https://github.com/mstrthealias/TeensyFanController/tree/9169de2e0e3d817ff4fb50c9669d37643c585822/doc/images/ex2.jpg?raw=true)

The same behavior is observed, except caused by room temperature changes, in this example:

![Example 3](https://github.com/mstrthealias/TeensyFanController/tree/9169de2e0e3d817ff4fb50c9669d37643c585822/doc/images/ex3.jpg?raw=true)

When only the CPU is loaded, the setpoint temperature does not change, and the fans speed up as needed:

![Example 4](https://github.com/mstrthealias/TeensyFanController/tree/9169de2e0e3d817ff4fb50c9669d37643c585822/doc/images/ex4.jpg?raw=true)


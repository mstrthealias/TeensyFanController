# PC Water Cooling Demo

TODO


## PID Usage and Example

TODO

The default configuration works well on a PC with a water-cooled CPU and an air-cooled GPU:

- Uses a supply water temperature sensor and a case temperature sensor
- Controls supply water temperature to 2-3°C above case temperature (initial setpoint is 28°C)
- Fan output percent is generally between 45%-60%, unless CPU is loaded
- If case temperature is within 2°C of supply water temperature, and fan output percent is greater than 60% for 20 seconds, the setpoint increases by 0.25°C


For example, the fans are controlled to keep supply water temperature at setpoint:

![Example 1.1](../images/ex1.1.jpg?raw=true 'Example 1.1')


In the same dataset as *Example 1.1*, and only plotting fan RPMs and DeltaT (return temp - supply temp), observe the fan speeds trailing DeltaT (which represents CPU load in the following example):

![Example 1.2](../images/ex1.2.jpg?raw=true 'Example 1.2')


Next, an air-cooled GPU is used for about 30 minutes, showing *setpoint adjustment* keeping fan speeds within the configured range:

![Example 2](../images/ex2.jpg?raw=true 'Example 2')


The same behavior is observed, except caused by room temperature changes, in this example:

![Example 3](../images/ex3.jpg?raw=true 'Example 3')


When only the CPU is loaded, the setpoint temperature does not change, and the fans speed up as needed:

![Example 4](../images/ex4.jpg?raw=true 'Example 4')



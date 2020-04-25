# Fan Setup

Fan speed configuration \(fe. control mode, temperature sensor source, etc.\).

Each available fan is individually configured:

![Fan Setup - Control modes](../images/ui/fans.1.png)

## Fan Operation

Fan operation / control mode (PID, Temp-% Table, Fixed, Off):

![Fan Setup - Control modes](../images/ui/fans.fan.modes.png)

If a fan's control mode is set to **PID** or **Temp-% Table**, a sensor is configured:

![Fan Setup - Temperature sources](../images/ui/fans.fan.sources.png)

### Temp-% Table

TODO

If a fan's control mode is set to **Temp-% Table**, click **edit table** to configure the fan speed points:

![Fan Setup - Temp-% Table](../images/ui/fans.table.1.png)

### PID Setup

TODO

If a fan's control mode is set to **PID**, click **edit PID settings** to configure PID control:

![Fan Setup - PID](../images/ui/fans.pid.1.png)

Setpoint adjustment is enabled by checking **Automatically Adjust**:

![Fan Setup - PID setpoint adjustment](../images/ui/fans.pid.2.png)

Setpoint adjustment may be restricted by case temperature, by checking **Use Case Temp**:

![Fan Setup - PID setpoint adjustment](../images/ui/fans.pid.3.png)


## Saving Changes

Once configured, click **save** to upload the configuration onto the hardware:

![Fan Setup](../images/ui/fans.2.png)

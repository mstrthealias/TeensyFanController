# Overview

A Qt 5 based application to configure or monitor a Teensy Fan Controller.

{% hint style="info" %}
After clicking Save, the configuration is saved onto the hardware, and is restored after power-loss \(without using the _Management UI_\).
{% endhint %}

## Getting Started

The application attempts to connect when launched:

![Management UI - Home screen (connecting)](../images/ui/main.connecting.png)

Once connected, the active configuration is downloaded:

![Management UI - Home screen (downloading configuration)](../images/ui/main.downloading.png)

Once downloaded, the present values are displayed:

![Management UI - Home screen (connected)](../images/ui/main.connected.png)

## Usage

Clicking the **menu button** provides access to 3 pages:

![Management UI - Main menu](../images/ui/main.menu.png)

The **log window** may also be opened by clicking the **LOG button** on the top right:

![Management UI - Log screen](../images/ui/main.log.png)

If the application fails to connect, or losses connection, the red globe icon in the toolbar may be clicked to reconnect:

![Management UI - Home screen - Offline](../images/ui/main.offline.png)

### Pages

| Page | Description |
| :--- | :--- |
| Controller Status (Home) | Display current fan RPMs and temperature readings \(updates every 500 ms\) |
| Hardware Setup | Sensor and fan hardware configuration \(fe. pin mapping, thermistor resistance, etc.\) |
| Fan Setup | Fan speed configuration \(fe. control mode, temperature sensor source, etc.\) |
| Controller Log | Debug logs sent from controller \(values printed every 5 seconds\) |


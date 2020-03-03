# Simple PCB

The initial Teensy Fan Controller PCB, designed to fit in a [Raspberry Pi 3 case][1]:

![Simple 001 PCB in a Raspberry Pi 3 case](../images/simple001.photo.1.png)

[1]: https://www.microcenter.com/product/614861/micro-connectors-plastic-raspberry-pi-3-model-b-case-kit


## Build Instructions

- Order PCB fabrication using the provided [Gerber files][2] (fe. 4pcb 33each)
- Order parts using provided [bill-of-materials][3]
- Solder parts onto PCB, guided by the provided [parts list][4]

{% hint style="info" %}

*Simple 001* board mount holes do not exactly fit the enclosure. Perform these steps to fit the PCB in the enclosure:

- Increase board mount hole size using a _7/64"_ drill bit
- Remove the in-board portion of the enclosure stand-offs (keeping the majority of each stand-off)

{% endhint %}

[2]: https://github.com/mstrthealias/TeensyFanController/blob/master/Hardware/Simple/simple001/gerbers.zip
[3]: https://github.com/mstrthealias/TeensyFanController/blob/master/Hardware/Simple/simple001/parts.csv
[4]: https://github.com/mstrthealias/TeensyFanController/blob/master/Hardware/Simple/simple001/partslist.txt


## Features

* Supports Teensy 3.x
* Standard 4pin Molex PC power connector
* Fits in a [Raspberry Pi 3 case][1]
* 6 fans
* 5 temperature sensors

{% hint style="warning" %}
*Simple 001* **fan 6** control is not functional when used with a Teensy 3.2.
{% endhint %}


## Notes

- The jumper in the middle (JP1) ties v3.3 to ADC reference voltage
- The AGND-GND bridge (R6) is left open

{% hint style="info" %}
A 2nd revision--Simple002--will be published, removing the AGND-GND bridge and the V3.3-AREF jumper.
{% endhint %}


## Paths

| Path | Description |
| :--- | :--- |
| [datasheets/](https://github.com/mstrthealias/TeensyFanController/tree/master/Hardware/Simple/datasheets) | Part datasheets |
| [simple001/EAGLE/](https://github.com/mstrthealias/TeensyFanController/tree/master/Hardware/Simple/simple001/EAGLE) | EAGLE library, schematic, and board \(*Simple 001*\) |
| [simple001/gerbers.zip](../../Hardware/Simple/simple001/gerbers.zip) | Gerber files for PCB fabrication \(*Simple 001*\) |
| [simple001/parts.csv](../../Hardware/Simple/simple001/parts.csv) | Part order list \(*Simple 001*\) |
| [simple001/partslist.txt](../../Hardware/Simple/simple001/partslist.txt) | Modified EAGLE partlist.txt export \(*Simple 001*\) |
| [simple001/schematic.pdf](../../Hardware/Simple/simple001/schematic.pdf) | Schematic print-out PDF \(*Simple 001*\) |
| [simple001/freeDFM.pdf](../../Hardware/Simple/simple001/freeDFM.pdf) | freeDFM multi-layer PDF output results \(*Simple 001*\) |


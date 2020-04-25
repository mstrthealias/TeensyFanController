# Simple PCB

The initial Teensy Fan Controller PCB, designed to fit in a [Raspberry Pi 3 case](https://www.microcenter.com/product/614861/micro-connectors-plastic-raspberry-pi-3-model-b-case-kit):

![Simple 001 PCB in a Raspberry Pi 3 case](../.gitbook/assets/simple001.photo.1.png)

{% hint style="info" %}
[Simple002 design files](https://github.com/mstrthealias/TeensyFanController/blob/master/Hardware/Simple/simple002) are available, but untested.
{% endhint %}

## Build Instructions

* Order PCB fabrication using the provided [Gerber files](https://github.com/mstrthealias/TeensyFanController/blob/master/Hardware/Simple/simple001/gerbers.zip) \(fe. 4pcb 33each\)
* Order parts using provided [bill-of-materials](https://github.com/mstrthealias/TeensyFanController/blob/master/Hardware/Simple/simple001/parts.csv)
* Solder parts onto PCB, guided by the provided [parts list](https://github.com/mstrthealias/TeensyFanController/blob/master/Hardware/Simple/simple001/partslist.txt)

{% hint style="info" %}
_Simple 001_ board mount holes do not exactly fit the enclosure. Perform these steps to fit the PCB in the enclosure:

* Increase board mount hole size using a _7/64"_ drill bit
* Remove the in-board portion of the enclosure stand-offs \(keeping the majority of each stand-off\)
{% endhint %}

## Features

* Supports Teensy 3.x
* Standard 4pin Molex PC power connector
* Fits in a [Raspberry Pi 3 case](https://www.microcenter.com/product/614861/micro-connectors-plastic-raspberry-pi-3-model-b-case-kit)
* 6 fans
* 5 temperature sensors

{% hint style="warning" %}
_Simple 001_ **fan 6** control is not functional when used with a Teensy 3.2.
{% endhint %}

## Notes

* The jumper in the middle \(JP1\) ties v3.3 to ADC reference voltage
* The AGND-GND bridge \(R6\) is left open

{% hint style="info" %}
A 2nd revision--Simple002--will be published, removing the AGND-GND bridge and the V3.3-AREF jumper.
{% endhint %}

## Paths

| Path | Description |
| :--- | :--- |
| [datasheets/](https://github.com/mstrthealias/TeensyFanController/tree/master/Hardware/Simple/datasheets) | Part datasheets |
| [simple001/EAGLE/](https://github.com/mstrthealias/TeensyFanController/tree/master/Hardware/Simple/simple001/EAGLE) | EAGLE library, schematic, and board \(_Simple 001_\) |
| [simple001/gerbers.zip](https://github.com/mstrthealias/TeensyFanController/tree/0ceefdbffbd5efc627939be4be3313a873193821/Hardware/Simple/simple001/gerbers.zip) | Gerber files for PCB fabrication \(_Simple 001_\) |
| [simple001/parts.csv](https://github.com/mstrthealias/TeensyFanController/tree/0ceefdbffbd5efc627939be4be3313a873193821/Hardware/Simple/simple001/parts.csv) | Part order list \(_Simple 001_\) |
| [simple001/partslist.txt](https://github.com/mstrthealias/TeensyFanController/tree/0ceefdbffbd5efc627939be4be3313a873193821/Hardware/Simple/simple001/partslist.txt) | Modified EAGLE partlist.txt export \(_Simple 001_\) |
| [simple001/schematic.pdf](https://github.com/mstrthealias/TeensyFanController/tree/0ceefdbffbd5efc627939be4be3313a873193821/Hardware/Simple/simple001/schematic.pdf) | Schematic print-out PDF \(_Simple 001_\) |
| [simple001/freeDFM.pdf](https://github.com/mstrthealias/TeensyFanController/tree/0ceefdbffbd5efc627939be4be3313a873193821/Hardware/Simple/simple001/freeDFM.pdf) | freeDFM multi-layer PDF output results \(_Simple 001_\) |


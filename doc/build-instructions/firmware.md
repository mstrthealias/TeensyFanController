# Firmware \(Teensy\)

TODO

## [Teensy 3.X Project Template](https://github.com/apmorton/teensy-template)

### Linux setup

Install the Teensy udev rule: `sudo cp tools/49-teensy.rules /etc/udev/rules.d/`

Then unplug your Teensy and plug it back in.

### Usage

1. Build your code `make`
2. Upload your code `make upload`

### Make Targets

* `make` alias for `make hex`
* `make build` compiles everything and produces a .elf
* `make hex` converts the elf to an intel hex file
* `make post_compile` opens the launcher with the correct file
* `make upload` uploads the hex file to a teensy board
* `make reboot` reboots the teensy

### Where everything came from

* The `teensy3` sub-folder is taken from [The Teensy 3 Cores](https://github.com/PaulStoffregen/cores/tree/master/teensy3)
* The `tools` sub-folder is taken from [Teensyduino](http://www.pjrc.com/teensy/td_download.html)
* The `src/main.cpp` file is moved, unmodified from `teensy3/main.cpp`
* The `Makefile` file is moved, modified from `teensy3/Makefile`
* The `49-teensy.rules` file is taken from [PJRC's udev rules](http://www.pjrc.com/teensy/49-teensy.rules)


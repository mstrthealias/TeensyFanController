# Firmware \(Teensy\)

A Makefile and Arduino project for building the Teensy Fan Controller firmware. Sources are contained in the [teensy\_fan\_controller sub-folder](https://github.com/mstrthealias/TeensyFanController/tree/master/teensy_fan_controller).

{% hint style="info" %}
The Makefile and project structure is based on [Teensy 3.X Project Template](https://github.com/apmorton/teensy-template).
{% endhint %}

## Usage \(Makefile\)

1. Prepare the project \(once\), as described in [Project Setup](setup.md)
2. Enter the `teensy_fan_controller` directory:

   ```text
    cd teensy_fan_controller
   ```

3. Compile from source:

   ```text
    TEENSY=35 make
   ```

{% hint style="info" %}
Makefile generates a Teensy 3.2 build by default, you may use a different Teensy by setting the TEENSY variable \(either 30, 31, 32, 35, or 36\).
{% endhint %}

1. Upload to Teensy 3.x:

   ```text
     make upload
   ```

{% hint style="info" %}
Firmware Make targets:

* `make` alias for `make hex`
* `make build` compiles everything and produces a .elf
* `make hex` converts the elf to an intel hex file
* `make post_compile` opens the launcher with the correct file
* `make upload` uploads the hex file to a teensy board
* `make reboot` reboots the teensy
{% endhint %}

## Usage \(Arduino Studio\)

1. Prepare the project \(once\), as described in [Project Setup](setup.md)
2. Copy `teensy_fan_controller/libraries/nanopb` into your user's Arduino libraries folder \(fe. `~/Documents/Arduino/libraries/`\)
3. Copy `teensy_fan_controller/libraries/PID` into your user's Arduino libraries folder \(fe. `~/Documents/Arduino/libraries/`\)
4. Launch **Arduino Studio**, and open project **teensy\_fan\_controller/src/src.ino**
5. Set **Tools &gt; Board** to your Teensy board using the top menu
6. Set **Tools &gt; USB Type** to **Raw HID** using the top menu
7. Build and upload firmware using **Sketch &gt; Upload**, or by clicking the **Upload** button in the toolbar

### Folder Structure

After the project is prepared, the `teensy_fan_controller/` folder has the following structure:

* `teensy3` sub-folder is taken from [The Teensy 3 Cores](https://github.com/PaulStoffregen/cores/tree/master/teensy3)
* `tools` sub-folder is taken from [Teensyduino](http://www.pjrc.com/teensy/td_download.html)
* `src/src.ino` contains the `loop()` and `setup()` functions, called by `teensy3/main.cpp`
* `src/*.{ino,c,h,cpp}` Teensy Fan Controller sources
* `Makefile` file, copied and modified from `teensy3/Makefile`
* `49-teensy.rules` file is taken from [PJRC's udev rules](http://www.pjrc.com/teensy/49-teensy.rules)


# Project Setup

## Software Requirements

The following software is required to build the project:

### Windows

* [Arduino](https://www.arduino.cc/en/Main/Software) installation with [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html) add-on
* Git for Windows \(Bash\)
* MinGW \(make\)
* Visual Studio 2017 \(for Management UI builds\)
* Qt 5 \(for Management UI builds\)

{% hint style="info" %}
This project uses Makefile, however build tools and board files are copied from an [Arduino](https://www.arduino.cc/en/Main/Software)/[Teensyduino](https://www.pjrc.com/teensy/teensyduino.html) installation.
{% endhint %}

{% hint style="warning" %}
If needed, copy `MinGW/bin/mingw32-make.exe` to `MinGW/bin/make.exe`, as this documentation refers to the command as `make`.
{% endhint %}

Requirements to regenerate protobuf \([nanopb](https://github.com/nanopb/nanopb)\) sources:

1. Download protoc binaries, fe.: [https://github.com/protocolbuffers/protobuf/releases/download/v3.11.4/protoc-3.11.4-win64.zip](https://github.com/protocolbuffers/protobuf/releases/download/v3.11.4/protoc-3.11.4-win64.zip)
2. Extract zip, fe. c:\share\protoc-3.11.4
3. Add protoc \(fe., c:\share\protoc-3.11.4\bin\) to PATH

### Ubuntu 18.04

* [Arduino](https://www.arduino.cc/en/Main/Software) installation with [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html) add-on
* `apt install build-essential`
* Qt 5 \(for Management UI builds\)

{% hint style="info" %}
This project uses Makefile, however build tools and board files are copied from an [Arduino](https://www.arduino.cc/en/Main/Software)/[Teensyduino](https://www.pjrc.com/teensy/teensyduino.html) installation.
{% endhint %}

Requirements to regenerate protobuf \([nanopb](https://github.com/nanopb/nanopb)\) sources:

* `apt install protobuf libprotobuf-dev python-protobuf`

{% hint style="info" %}
To communicate with the Teensy hardware as a **non-root user**, install the [Teensy udev rule][1]:
```shell script
sudo cp teensy_fan_controller/tools/49-teensy.rules /etc/udev/rules.d/
```
{% endhint %}

[1]: https://www.pjrc.com/teensy/49-teensy.rules


## Fetching sources

Use `git` to clone the root project \([TeensyFanController](https://github.com/mstrthealias/TeensyFanController)\).

You must initialize and pull the git sub-modules/sub-repositories:

```text
git clone https://github.com/mstrthealias/TeensyFanController.git
git submodule init
git submodule update
```

{% hint style="info" %}
Use the `ui_qt5` sub-directory underneath the root project directory for Management UI builds.
{% endhint %}

## Setup project build environment

Change directory to the project root, and use Makefile:

```text
cd TeensyFanController
ARDUINO_DIR=/opt/Arduino make
```

{% hint style="info" %}
Root Make targets:

* `make` alias for `make setup`
* `make setup` alias for `make clean setup-toolchain setup-libs setup-nanopb protos`
* `make setup-toolchain` copy toolchain and Teensy3 cores from [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html) into project
* `make setup-libs` copy [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html) libraries into project
* `make setup-nanopb` copy [nanopb](https://github.com/nanopb/nanopb) into project libraries
* `make protos` alias for `make runtime_config_v1.pb.c`
* `make runtime_config_v1.pb.c` generate RuntimeConfigV1 protobuf sources
* `make clean` alias for `make clean-build clean-setup`
* `make clean-build` delete builds produced by `teensy_fan_controller/Makefile`
* `make clean-setup` delete files produced by this Makefile
{% endhint %}


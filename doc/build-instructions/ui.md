# Management UI

A \(Qt5\) user interface to configure and monitor the [Teensy Fan Controller](https://github.com/mstrthealias/TeensyFanController/).

## Build Instructions

### Windows

Prerequisites:

1. Qt Installer
2. MSVC 2017 64-bit
3. Recent Qt release \(Qt 5.13.2 at time of writing\) for MSVC 2017 64-bit
4. Qt Charts component

Note: the application is NOT compatible with MinGW builds due to the use of [hidapi](https://github.com/libusb/hidapi). HIDAPI will compile with mingw32 \(NOT mingw64-32bit\), however Qt must be built from source to use this stack.

First, build the static [HIDAPI](https://github.com/mstrthealias/HIDAPI-Qt5) library:

Note: HIDAPI should be cloned into this project's parent directory \(or clone the [parent project](https://github.com/mstrthealias/TeensyFanController/) with all submodules\).

1. Launch **x64 Native Tools Command Prompt for VS 2017**
2. Add Qt5 to PATH, fe.:

```text
    SET PATH=%PATH%;c:\share\Qt\5.13.2\msvc2017_64\bin
```

1. Change directory to HIDAPI
2. Run qmake:

```text
    qmake
```

1. Run nmake:

```text
    nmake
```

1. Verify HIDAPI.lib was created:

```text
    dir windows\HIDAPI.lib

    ...
    12/30/2019  11:03 AM            27,960 HIDAPI.lib
```

Building the Fan Controller UI:

Until the Fan Controller UI's installer is published, it is easiest to run the application using Qt Creator. You may build using the CLI with qmake and nmake, but will have to copy all Qt runtime dependencies \(qml, plugins, dlls\) into the release directory to start the application.

### Linux \(Ubuntu 18.04\)

Prerequisites:

1. Qt Installer
2. Recent Qt release \(Qt 5.13.2 at time of writing\) for GCC 64-bit
3. Qt Charts component
4. `apt install build-essential libxkbcommon-x11-0 libhidapi-dev libhidapi-hidraw0 libgl1 libgl1-mesa-dev libx11-xcb1`

Note: I experienced issues launching the application using X11 forwarding and ended up installing and launching xfce \(`apt install xfce4`\) to run Qt Creator.

First, build the static [HIDAPI](https://github.com/mstrthealias/HIDAPI-Qt5) library:

Note: HIDAPI should be cloned into this project's parent directory \(or clone the [parent project](https://github.com/mstrthealias/TeensyFanController/) with all submodules\).

In non-root Shell, add Qt5 to PATH, fe.:

```text
    PATH=$PATH:~/Qt/5.13.2/gcc_64/bin
    QT_QPA_PLATFORM_PLUGIN_PATH=~/Qt/5.13.2/gcc_64/plugins
```

Change directory to HIDAPI, fe.:

```text
    cd ~/TeensyFanController/HIDAPI
```

Run qmake:

```text
    qmake
```

Run make:

```text
    make
```

Verify libHIDAPI.a was created:

```text
    ls linux/libHIDAPI.a
```

Building the Fan Controller UI:

Change directory to HIDAPI, fe.:

```text
    cd ~/TeensyFanController/teensy_fan_controller
```

Run qmake:

```text
    qmake
```

Run make:

```text
    make
```

Launch using `sudo` \(note: replace /home/username/Qt/5.13.2 with path to Qt release\):

```text
sudo bash -c 'QT_BASE=/home/username/Qt/5.13.2 QML2_IMPORT_PATH="$QT_BASE/gcc_64/qml" QML_IMPORT_PATH="$QT_BASE/gcc_64/qml" QT_QPA_PLATFORM_PLUGIN_PATH="$QT_BASE/gcc_64/plugins"  ./tfctrl'
```


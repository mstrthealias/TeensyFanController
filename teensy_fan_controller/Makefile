# The name of your project (used to name the compiled .hex file)
TARGET = $(notdir $(CURDIR))

# The teensy version to use, 30, 31, 32, 35, 36, or LC
TEENSY ?= 32

# define to override Teensy default frequency (underclocking most chips for this project)
TEENSY_CORE_SPEED = 48000000

# Arduino version
ARDUINO = 10810

# configurable options
OPTIONS = -DUSB_RAWHID -DLAYOUT_US_ENGLISH -DARDUINO=$(ARDUINO) -DTEENSYDUINO=144

# directory to build in
BUILDDIR = $(abspath $(CURDIR)/build)

#************************************************************************
# Location of Teensyduino utilities, Toolchain, and Arduino Libraries.
# To use this makefile without Arduino, copy the resources from these
# locations and edit the pathnames.  The rest of Arduino is not needed.
#************************************************************************

# path location for Teensy Loader, teensy_post_compile and teensy_reboot
TOOLSPATH = $(CURDIR)/tools

# used to append .exe to compiler programs if Windows_NT is detected
PROG_SUFFIX = ""

ifeq ($(OS),Windows_NT)
    PROG_SUFFIX = ".exe"
#else
#    UNAME_S := $(shell uname -s)
#    ifeq ($(UNAME_S),Darwin)
#        TOOLSPATH = /Applications/Arduino.app/Contents/Java/hardware/tools/
#    endif
endif

# path location for Teensy 3 core
COREPATH = teensy3

# path location for Arduino libraries
LIBRARYPATH = libraries

# path location for the arm-none-eabi compiler
COMPILERPATH = $(TOOLSPATH)/arm/bin

#************************************************************************
# Settings below this point usually do not need to be edited
#************************************************************************

# CPPFLAGS = compiler options for C and C++
CPPFLAGS = -Wall -g -Os -mthumb -ffunction-sections -fdata-sections -nostdlib -MMD $(OPTIONS) -Isrc -I$(COREPATH)

# compiler options for C++ only
CXXFLAGS = -std=gnu++14 -felide-constructors -fno-exceptions -fno-rtti

# compiler options for C only
CFLAGS =

# linker options
LDFLAGS = -Os -Wl,--gc-sections,--defsym=__rtc_localtime=0 --specs=nano.specs -mthumb

# additional libraries to link
LIBS = -lm

# compiler options specific to teensy version
ifeq ($(TEENSY), 30)
    TEENSY_CORE_SPEED ?= 48000000
    CPPFLAGS += -D__MK20DX128__ -mcpu=cortex-m4 -DF_CPU=$(TEENSY_CORE_SPEED) -DARDUINO_TEENSY30
    LDSCRIPT = $(COREPATH)/mk20dx128.ld
    LDFLAGS += -mcpu=cortex-m4 -T$(LDSCRIPT)
else ifeq ($(TEENSY), 31)
    TEENSY_CORE_SPEED ?= 72000000
    CPPFLAGS += -D__MK20DX256__ -mcpu=cortex-m4 -DF_CPU=$(TEENSY_CORE_SPEED) -DARDUINO_TEENSY31
    LDSCRIPT = $(COREPATH)/mk20dx256.ld
    LDFLAGS += -mcpu=cortex-m4 -T$(LDSCRIPT)
else ifeq ($(TEENSY), 32)
    TEENSY_CORE_SPEED ?= 72000000
    CPPFLAGS += -D__MK20DX256__ -mcpu=cortex-m4 -DF_CPU=$(TEENSY_CORE_SPEED) -DARDUINO_TEENSY32
    LDSCRIPT = $(COREPATH)/mk20dx256.ld
    LDFLAGS += -mcpu=cortex-m4 -T$(LDSCRIPT)
else ifeq ($(TEENSY), LC)
    TEENSY_CORE_SPEED ?= 48000000
    CPPFLAGS += -D__MKL26Z64__ -mcpu=cortex-m0plus -DF_CPU=$(TEENSY_CORE_SPEED) -DARDUINO_TEENSYLC
    LDSCRIPT = $(COREPATH)/mkl26z64.ld
    LDFLAGS += -mcpu=cortex-m0plus -T$(LDSCRIPT)
    LIBS += -larm_cortexM0l_math
else ifeq ($(TEENSY), 35)
    TEENSY_CORE_SPEED ?= 120000000
    CPPFLAGS += -D__MK64FX512__ -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DF_CPU=$(TEENSY_CORE_SPEED) -DARDUINO_TEENSY35
    LDSCRIPT = $(COREPATH)/mk64fx512.ld
    LDFLAGS += -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -T$(LDSCRIPT)
    LIBS += -larm_cortexM4lf_math
else ifeq ($(TEENSY), 36)
    TEENSY_CORE_SPEED ?= 180000000
    CPPFLAGS += -D__MK66FX1M0__ -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DF_CPU=$(TEENSY_CORE_SPEED) -DARDUINO_TEENSY36
    LDSCRIPT = $(COREPATH)/mk66fx1m0.ld
    LDFLAGS += -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -T$(LDSCRIPT)
    LIBS += -larm_cortexM4lf_math
else
    $(error Invalid setting for TEENSY)
endif

# names for the compiler programs
CC = $(abspath $(COMPILERPATH))/arm-none-eabi-gcc$(PROG_SUFFIX)
CXX = $(abspath $(COMPILERPATH))/arm-none-eabi-g++$(PROG_SUFFIX)
OBJCOPY = $(abspath $(COMPILERPATH))/arm-none-eabi-objcopy$(PROG_SUFFIX)
SIZE = $(abspath $(COMPILERPATH))/arm-none-eabi-size$(PROG_SUFFIX)

# automatically create lists of the sources and objects
LC_FILES := $(wildcard $(LIBRARYPATH)/*/*.c)
LCPP_FILES := $(wildcard $(LIBRARYPATH)/*/*.cpp)
TC_FILES := $(wildcard $(COREPATH)/*.c)
TCPP_FILES := $(wildcard $(COREPATH)/*.cpp)
C_FILES := $(wildcard src/*.c)
CPP_FILES := $(wildcard src/*.cpp)
INO_FILES := $(wildcard src/*.ino)

# include paths for libraries
L_INC := $(foreach lib,$(filter %/, $(wildcard $(LIBRARYPATH)/*/)), -I$(lib))

SOURCES := $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o) $(INO_FILES:.ino=.o) $(TC_FILES:.c=.o) $(TCPP_FILES:.cpp=.o) $(LC_FILES:.c=.o) $(LCPP_FILES:.cpp=.o)
OBJS := $(foreach src,$(SOURCES), $(BUILDDIR)/$(src))

all: hex

build: $(BUILDDIR)/$(TARGET).elf

hex: $(BUILDDIR)/$(TARGET).hex

post_compile: $(BUILDDIR)/$(TARGET).hex
	$(abspath $(TOOLSPATH))/teensy_post_compile -file="$(TARGET)" -path=$(BUILDDIR) -tools="$(abspath $(TOOLSPATH))"

reboot:
	@-$(abspath $(TOOLSPATH))/teensy_reboot

upload: post_compile reboot

$(BUILDDIR)/%.o: %.c
	@echo -e "[CC]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.cpp
	@echo -e "[CXX]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.ino
	@echo -e "[CXX]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -o "$@" -x c++ -include Arduino.h -c "$<"

$(BUILDDIR)/$(TARGET).elf: $(OBJS) $(LDSCRIPT)
	@echo -e "[LD]\t$@"
	@$(CC) $(LDFLAGS) -o "$@" $(OBJS) $(LIBS)

$(BUILDDIR)/%.hex: $(BUILDDIR)/%.elf
	@echo -e "[HEX]\t$@"
	@$(SIZE) "$<"
	@$(OBJCOPY) -O ihex -R .eeprom "$<" "$@"

# compiler generated dependency info
-include $(OBJS:.o=.d)

clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)"
	@rm -f "$(BUILDDIR)/$(TARGET).elf" "$(BUILDDIR)/$(TARGET).hex"

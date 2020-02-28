# ARDUINO_DIR must be set to the Arduino path on your system:
ARDUINO_DIR = /c/Program\ Files\ \(x86\)/Arduino


# Begin logic:

PROJECTDIR = $(CURDIR)/teensy_fan_controller
NANOPBDIR = $(CURDIR)/nanopb


ifneq ($(shell test -d $(ARDUINO_DIR)/drivers && echo -n yes),yes)
    $(error Invalid ARDUINO_DIR)
endif
ifneq ($(shell test -d $(ARDUINO_DIR)/hardware/teensy/avr/cores/teensy3 && echo -n yes),yes)
    $(error Arduino must have Teensyduino addon installed)
endif


# Include the nanopb provided Makefile rules
include nanopb/extra/nanopb.mk



all: setup


clean-build:
	@echo
	@echo Cleaning build...
	rm -rf $(PROJECTDIR)/build/*
	rm -f $(PROJECTDIR)/src/nanopb.proto
	rm -f $(PROJECTDIR)/src/runtime_config_v1.pb.c
	rm -f $(PROJECTDIR)/src/runtime_config_v1.pb.h


clean-setup:
	@echo
	@echo Cleaning project...
	rm -rf $(PROJECTDIR)/libraries/nanopb
	rm -rf $(PROJECTDIR)/libraries/EEPROM
	rm -rf $(PROJECTDIR)/tools/*
	rm -rf $(PROJECTDIR)/teensy3/*


clean: clean-build clean-setup


setup: clean protos
	@echo
	@echo Copy Teensy tools into project...
	cp $(ARDUINO_DIR)/hardware/tools/teensy* $(PROJECTDIR)/tools/
	cp $(ARDUINO_DIR)/hardware/tools/precompile_helper* $(PROJECTDIR)/tools/
	cp $(ARDUINO_DIR)/hardware/tools/stdout_redirect* $(PROJECTDIR)/tools/
	cp $(ARDUINO_DIR)/hardware/tools/mktinyfat* $(PROJECTDIR)/tools/
	cp -a $(ARDUINO_DIR)/hardware/tools/arm $(PROJECTDIR)/tools/
	touch $(PROJECTDIR)/tools/.gitkeep
	
	@echo
	@echo Copy Teensy 3.x cores into project...
	cp -a $(ARDUINO_DIR)/hardware/teensy/avr/cores/teensy3/* $(PROJECTDIR)/teensy3/
	touch $(PROJECTDIR)/teensy3/.gitkeep
	
	@echo
	@echo Copy libraries into project...
	cp -a $(ARDUINO_DIR)/hardware/teensy/avr/libraries/EEPROM $(PROJECTDIR)/libraries/
	touch $(PROJECTDIR)/libraries/EEPROM/.gitkeep
	
	mkdir $(PROJECTDIR)/libraries/nanopb
	cp $(NANOPBDIR)/*.{h,c} $(PROJECTDIR)/libraries/nanopb/
	touch $(PROJECTDIR)/libraries/nanopb/.gitkeep
	


# Protobuf build steps:

protos: runtime_config_v1.pb.c

runtime_config_v1.pb.c: $(PROJECTDIR)/src/runtime_config_v1.proto
	@echo
	@echo Building protobufs
	cp $(NANOPBDIR)/generator/proto/nanopb.proto $(PROJECTDIR)/src/
	$(PROTOC) $(PROTOC_OPTS) --proto_path=$(PROJECTDIR)/src/ --nanopb_out=$(PROJECTDIR)/src/ $(PROJECTDIR)/src/runtime_config_v1.proto


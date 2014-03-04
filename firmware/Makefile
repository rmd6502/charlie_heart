###############################################################################
# Makefile for the project charlie_bracelet
###############################################################################

## General Flags
PROJECT = charlie_heart2
#PROJECT = charlie_heart3
MCU = attiny841
TARGET = $(PROJECT).elf
CC = avr-gcc
F_CPU = 8000000

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -DF_CPU=$(F_CPU) -Os -fsigned-char -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d --std=c99
#CFLAGS += -DADC_MODE

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=$(PROJECT).map

## Programmer settings
AVRDUDE = avrdude 
PROGRAMMER = dragon_isp
PORT=usb
ifeq ($(PART),)
	ifeq ($(MCU),attiny85)
		PART=t85
        LFUSE=0xe1
        HFUSE=0xdf
        EFUSE=0xff
	else ifeq ($(MCU),attiny841)
		PART=t841
        LFUSE=0xe1
        HFUSE=0xdf
        EFUSE=0xff
	else ifeq ($(MCU),attiny45)
		PART=t45
        LFUSE=0xe2
        HFUSE=0xdf
        EFUSE=0xff
	else ifeq ($(MCU),attiny841)
        PART=t841
        LFUSE=0xe2
        HFUSE=0xdf
        EFUSE=0xff
	endif
endif

## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = 

## Objects that must be built in order to link
OBJECTS = $(PROJECT).o charlie.o pins.o

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) $(PROJECT).hex $(PROJECT).eep $(PROJECT).lss size

## Compile
$(PROJECT).o: $(PROJECT).c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size ${TARGET}

flash: $(PROJECT).hex
	$(AVRDUDE) -P usb -p $(PART) -c $(PROGRAMMER) -U flash:w:$(PROJECT).hex

fuse:
	$(AVRDUDE) -P $(PORT) -p $(PART) -c $(PROGRAMMER) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

.PHONY: program
program: fuse flash
	@echo All done!

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(PROJECT).elf dep/* $(PROJECT).hex $(PROJECT).eep $(PROJECT).lss $(PROJECT).map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)


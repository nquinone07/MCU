## COMPILER ## 
AVR_GCC := avr-gcc
AVR_CP  := avr-objcopy
AVR_SZ  := avr-size
AVR_DMP := avr-objdump

## FLAGS ## 
DEVICE    := -D__AVR_ATmega168__
MCU       := -mmcu=atmega168 #-B /usr/lib/gcc/avr/5.4.0/device-specs/
RAM_START := -Tdata 0x800100 # referencing: /usr/lib/avr/include/avr/iom168.h
OPTMIZ    := -Os
OPTS      := -nostartfiles 

## OUTPUT ##
BUILD_DIR := build
BIN_DIR   := bin
ADD_DIR   := supplementary

## SOURCE FILES ##
BOOT      := $(subst ./,,$(shell find . -maxdepth 1 -name \*.S))
PROGRAM   := $(subst ./,,$(shell find . -maxdepth 1 -name \*.c))
SRC_FILES := $(BOOT) $(PROGRAM)
INCLUDES  := $(subst ./,,$(shell find . -maxdepth 1 -name \*.h))
OBJS      := $(addprefix $(BUILD_DIR)/, $(patsubst %.c,%.o,$(PROGRAM)))
OBJS      += $(addprefix $(BUILD_DIR)/, $(patsubst %.S,%.o,$(BOOT)))

## START FILE ##
START := /usr/lib/avr/lib/avr5/crtatmega168.o

## TARGET ##
TARGET  := $(BIN_DIR)/target.elf
HEX_OUT := $(BIN_DIR)/target.hex

## PHONY ##
.PHONY: all setup clean build

## RECIPES ##
all: setup build

printenv:
	@echo 
	@echo "OBJECTS  : $(OBJS)"
	@echo "PROGRAM  : $(PROGRAM)"
	@echo "BOOT     : $(BOOT)"
	@echo "INCLUDES : $(INCLUDES)"
	@echo 

setup:
	@echo 
	@echo "Setting up output directories [$(BIN_DIR), $(BUILD_DIR)]..."
	@mkdir -p $(BIN_DIR) $(BUILD_DIR) $(ADD_DIR)

clean:
	@echo 
	@echo "Cleaning output directories..."
	@rm -rf $(BIN_DIR) $(BUILD_DIR) $(ADD_DIR)

build: $(TARGET)
	@echo 
	@echo "Binary created @ $(TARGET)! Creating Intel Hex File $(HEX_OUT)..."
	$(AVR_DMP) -S -h $(TARGET) > "$(ADD_DIR)/target.lss"
	$(AVR_CP) -j .text -j .data -j .bss -O ihex $(TARGET) $(HEX_OUT)
	$(AVR_SZ) --format=avr --mcu=atmega168 $(TARGET)

$(TARGET): $(OBJS)
	@echo 
	@echo "Linking object files. Creating .elf..."
	$(AVR_GCC) $(MCU) $(RAM_START) $(OPTS) $^ $(START) -o $@

$(BUILD_DIR)/%.o : %.c
	@echo
	@echo "Compiling object file [$@]..."
	$(AVR_GCC) -g $(MCU) $(DEVICE) $(OPTMIZ) -c $< -o $@

$(BUILD_DIR)/%.o : %.S
	@echo
	@echo "Compiling object file [$@]..."
	$(AVR_GCC) -g $(MCU) $(DEVICE) $(OPTMIZ) -c $< -o $@

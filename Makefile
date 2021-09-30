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
BARE_DIR  := bare
COMMON_DIR:= common
RT_DIR    := rtos
BIN_DIR   := bin
OUT_DIRS  := $(BUILD_DIR)/$(BARE_DIR) $(BUILD_DIR)/$(RT_DIR) $(BUILD_DIR)/$(COMMON_DIR) $(BIN_DIR)
ADD_DIR   := supplementary
SRC_DIR   := src

## RTOS SPECIFIC ## 
RTOS_INC  :=  -Iinclude -I../FreeRTOSv202107.00/FreeRTOS/Source/include 

## SOURCE FILES ##
SRCS        := $(shell find $(SRC_DIR) -mindepth 1 -name \*.c -o -name \*.S)
OBJS        := $(subst $(SRC_DIR)/,,$(SRCS:%.c=$(BUILD_DIR)/%.o))
RT_OBJS     := $(filter $(BUILD_DIR)/$(RT_DIR)/%.o,$(OBJS))
BARE_OBJS   := $(filter $(BUILD_DIR)/$(BARE_DIR)/%.o,$(OBJS))
COMMON_OBJS := $(filter $(BUILD_DIR)/$(COMMON_DIR)/%.o,$(OBJS))

## START FILE ##
START := /usr/lib/avr/lib/avr5/crtatmega168.o

## TARGETS ##
BARE_BIN           := $(BIN_DIR)/bare-target.elf
BARE_HEX_OUT       := $(BIN_DIR)/bare-target.hex
RTOS_BIN           := $(BIN_DIR)/rt-target.elf
RTOS_HEX_OUT       := $(BIN_DIR)/rt-target.hex

## PHONY ##
.PHONY: all setup clean build

## RECIPES ##
all: build-bare build-rtos
	@echo
	@echo "Finished! All binaries were created... Use the programmer in tools/ to ship any binary over to your atmel!"

printenv:
	@echo 
	@echo "OBJECTS  : $(OBJS)"
	@echo "SOURCES  : $(SRCS)"
	@echo "BOOT     : $(BOOT)"
	@echo "INCLUDES : $(INCLUDES)"
	@echo 

setup:
	@echo 
	@echo "Setting up output directories [$(OUT_DIRS)]..."
	@mkdir -p $(OUT_DIRS) $(ADD_DIR)

clean:
	@echo 
	@echo "Cleaning output directories..."
	@rm -rf $(BIN_DIR) $(BUILD_DIR) $(ADD_DIR)

build-rtos: setup $(RTOS_BIN) $(RTOS_HEX_OUT)

build-bare: setup $(BARE_BIN) 
	@echo 
	@echo "Binary created: $(BARE_BIN)! Creating Intel Hex File..."
	$(AVR_DMP) -S -h $(BARE_BIN) > "$(ADD_DIR)/$(notdir $(basename $<)).lss"
	$(AVR_CP) -j .text -j .data -j .bss -O ihex $(BARE_BIN) $(BARE_HEX_OUT)
	$(AVR_SZ) --format=avr --mcu=atmega168 $(BARE_BIN)

$(BARE_BIN): $(BARE_OBJS) $(COMMON_OBJS) 
	@echo 
	@echo "Linking object files. Creating .elf..."
	$(AVR_GCC) $(MCU) $(RAM_START) $(OPTS) $^ $(START) -o $@

$(BUILD_DIR)/$(RT_DIR)/%.o : $(SRC_DIR)/$(RT_DIR)/%.c
	@echo
	@echo "Compiling object file [$@]..."
	$(AVR_GCC) -g $(MCU) $(DEVICE) $(OPTMIZ) -c $< -o $@

$(BUILD_DIR)/$(BARE_DIR)/%.o : $(SRC_DIR)/$(BARE_DIR)/%.c
	@echo
	@echo "Compiling object file [$@]..."
	$(AVR_GCC) -I$(SRC_DIR) -I$(SRC_DIR)/$(COMMON_DIR) -g $(MCU) $(DEVICE) $(OPTMIZ) -c $< -o $@

$(BUILD_DIR)/$(COMMON_DIR)/%.o : $(SRC_DIR)/$(COMMON_DIR)/%.c
	@echo
	@echo "Compiling object file [$@]..."
	$(AVR_GCC) -std=c11 -I$(SRC_DIR)/$(COMMON_DIR) -g $(MCU) $(DEVICE) $(OPTMIZ) -c $< -o $@

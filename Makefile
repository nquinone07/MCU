## COMPILER ## 
AVR_GCC := avr-gcc

## FLAGS ## 
DEVICE    := -D__AVR_ATmega168__
MCU       := -mmcu=atmega168
RAM_START := -Tdata 0x100 # referencing: /usr/lib/avr/include/avr/iom168.h

## OUTPUT ##
BUILD_DIR := build
BIN_DIR   := bin

## SOURCE FILES ##
BOOT     := $(subst ./,,$(shell find . -name \*.S))
PROGRAM  := $(subst ./,,$(shell find . -name \*.c))
INCLUDES := $(subst ./,,$(shell find . -name \*.h))
OBJS     := $(addprefix $(BUILD_DIR)/, $(patsubst %.c,%.o,$(PROGRAM)))
OBJS     += $(addprefix $(BUILD_DIR)/, $(patsubst %.S,%.o,$(BOOT)))

## TARGET ##
TARGET := $(BIN_DIR)/target.elf

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
	@mkdir -p $(BIN_DIR) $(BUILD_DIR)

clean:
	@echo 
	@echo "Cleaning output directories..."
	@rm -rf $(BIN_DIR) $(BUILD_DIR)


build: $(TARGET)
	@echo 
	@echo "Binary created @ $(TARGET)..."

$(TARGET): $(OBJS)
	@echo 
	@echo "Linking..."
	$(AVR_GCC) $(MCU) $(RAM_START) -nostartfiles $^ -o $@

$(OBJS): $(BOOT) $(PROGRAM)
	@echo
	@echo "Compiling object file [$@]..."
	$(AVR_GCC) -g $(MCU) $(DEVICE) -c $< -o $@

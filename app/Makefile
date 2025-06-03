TARGET = app

DEBUG = 0
OPT = -Os
# -Os == size optimalization, -Og for debugging

DIR_ROOT  ?= .

DIR_HW     := ../hw
DIR_SDK    := ../sdk
DIR_USB    := ../usb

include  $(DIR_SDK)/sdk_stm32u535.mk

BUILD_DIR = build

MAKEFLAGS += "-j $(shell nproc)"

C_SOURCES +=  \
  $(DIR_ROOT)/main.c \
  $(DIR_ROOT)/cmd.c \
  \
  $(DIR_HAL)/tty.c \
  $(DIR_HAL)/led.c \
  \
  $(DIR_DRV)/dma.c \
  $(DIR_DRV)/gpio.c \
  $(DIR_DRV)/irq.c \
  $(DIR_DRV)/reset.c \
  $(DIR_DRV)/time.c \
  $(DIR_DRV)/uart.c \
  $(DIR_DRV)/sys.c \
  $(DIR_DRV)/wd.c \
  $(DIR_DRV)/spi.c \
  \
  $(DIR_COMMON)/util.c \
  \
  $(DIR_USB)/ux_device_cdc_acm.c \
  $(DIR_USB)/ux_device_descriptors.c \
  $(DIR_USB)/usb_device.c \


C_DEFS +=  \
-DMAIN_DEBUG=$(DEBUG)

# C includes
C_INCLUDES +=  \
-I$(DIR_ROOT) \
-I$(DIR_USB) \
-I$(DIR_HW)

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################

# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################

.PHONY: clean
clean:
	-rm -fR $(BUILD_DIR)

.PHONY: flash
flash:
	#-st-flash --reset write $(BUILD_DIR)/$(TARGET).bin 0x8000000
	-st-flash --format ihex --reset write $(BUILD_DIR)/$(TARGET).hex


.PHONY: erase
erase:
	-st-flash erase
  
#######################################
# -include $(wildcard $(BUILD_DIR)/*.d)


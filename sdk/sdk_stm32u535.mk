# to be included in Makefile

ifndef DIR_SDK
  $(error DIR_SDK not set)
endif

DIR_STM := $(DIR_SDK)/stm32
DIR_DRV := $(DIR_SDK)/drv_u5
DIR_HAL := $(DIR_SDK)/hal
DIR_SYS := $(DIR_STM)/CMSIS
DIR_COMMON := $(DIR_SDK)/common

PREFIX = @echo $@;arm-none-eabi-

ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif

HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

CPU = -mcpu=cortex-m33
FPU = -mfpu=fpv4-sp-d16
FLOAT-ABI = -mfloat-abi=hard
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

STM32_HAL = $(DIR_STM)/STM32U5xx_HAL_Driver
STM32_USB = $(DIR_STM)/STM32_USBX_Library

C_SOURCES =  \
  $(DIR_SYS)/src/system_stm32u5xx.c \
  $(STM32_HAL)/Src/stm32u5xx_ll_adc.c \
  $(STM32_HAL)/Src/stm32u5xx_ll_dma.c \
  $(STM32_HAL)/Src/stm32u5xx_ll_exti.c \
  $(STM32_HAL)/Src/stm32u5xx_ll_gpio.c \
  $(STM32_HAL)/Src/stm32u5xx_ll_pwr.c \
  $(STM32_HAL)/Src/stm32u5xx_ll_rcc.c \
  $(STM32_HAL)/Src/stm32u5xx_ll_rtc.c \
  $(STM32_HAL)/Src/stm32u5xx_ll_spi.c \
  $(STM32_HAL)/Src/stm32u5xx_ll_tim.c \
  $(STM32_HAL)/Src/stm32u5xx_ll_lpuart.c \
  $(STM32_HAL)/Src/stm32u5xx_ll_usart.c \
  $(STM32_HAL)/Src/stm32u5xx_ll_usb.c \
  $(STM32_HAL)/Src/stm32u5xx_ll_utils.c \
  $(STM32_HAL)/Src/stm32u5xx_hal_cortex.c \
  $(STM32_HAL)/Src/stm32u5xx_hal_dma.c \
  $(STM32_HAL)/Src/stm32u5xx_hal_dma_ex.c \
  $(STM32_HAL)/Src/stm32u5xx_hal_pcd.c \
  $(STM32_HAL)/Src/stm32u5xx_hal_pcd_ex.c \
  $(STM32_HAL)/Src/stm32u5xx_hal_spi.c \
  $(STM32_HAL)/Src/stm32u5xx_hal_spi_ex.c \
  \
  $(STM32_USB)/common/usbx_stm32_device_controllers/ux_dcd_stm32_callback.c \
  $(STM32_USB)/common/usbx_stm32_device_controllers/ux_dcd_stm32_endpoint_create.c \
  $(STM32_USB)/common/usbx_stm32_device_controllers/ux_dcd_stm32_endpoint_destroy.c \
  $(STM32_USB)/common/usbx_stm32_device_controllers/ux_dcd_stm32_endpoint_reset.c \
  $(STM32_USB)/common/usbx_stm32_device_controllers/ux_dcd_stm32_endpoint_stall.c \
  $(STM32_USB)/common/usbx_stm32_device_controllers/ux_dcd_stm32_endpoint_status.c \
  $(STM32_USB)/common/usbx_stm32_device_controllers/ux_dcd_stm32_frame_number_get.c \
  $(STM32_USB)/common/usbx_stm32_device_controllers/ux_dcd_stm32_function.c \
  $(STM32_USB)/common/usbx_stm32_device_controllers/ux_dcd_stm32_initialize.c \
  $(STM32_USB)/common/usbx_stm32_device_controllers/ux_dcd_stm32_initialize_complete.c \
  $(STM32_USB)/common/usbx_stm32_device_controllers/ux_dcd_stm32_interrupt_handler.c \
  $(STM32_USB)/common/usbx_stm32_device_controllers/ux_dcd_stm32_transfer_run.c \
  $(STM32_USB)/common/usbx_stm32_device_controllers/ux_dcd_stm32_uninitialize.c \
  $(STM32_USB)/common/usbx_stm32_device_controllers/ux_dcd_stm32_transfer_abort.c \
  $(STM32_USB)/common/core/src/ux_system_tasks_run.c \
  $(STM32_USB)/common/core/src/ux_utility_debug_callback_register.c \
  $(STM32_USB)/common/core/src/ux_utility_debug_log.c \
  $(STM32_USB)/common/core/src/ux_utility_delay_ms.c \
  $(STM32_USB)/common/core/src/ux_utility_descriptor_pack.c \
  $(STM32_USB)/common/core/src/ux_utility_descriptor_parse.c \
  $(STM32_USB)/common/core/src/ux_utility_error_callback_register.c \
  $(STM32_USB)/common/core/src/ux_utility_long_get.c \
  $(STM32_USB)/common/core/src/ux_utility_long_get_big_endian.c \
  $(STM32_USB)/common/core/src/ux_utility_long_put.c \
  $(STM32_USB)/common/core/src/ux_utility_long_put_big_endian.c \
  $(STM32_USB)/common/core/src/ux_utility_memory_allocate.c \
  $(STM32_USB)/common/core/src/ux_utility_memory_allocate_add_safe.c \
  $(STM32_USB)/common/core/src/ux_utility_memory_allocate_mulc_safe.c \
  $(STM32_USB)/common/core/src/ux_utility_memory_allocate_mulv_safe.c \
  $(STM32_USB)/common/core/src/ux_utility_memory_compare.c \
  $(STM32_USB)/common/core/src/ux_utility_memory_copy.c \
  $(STM32_USB)/common/core/src/ux_utility_memory_free.c \
  $(STM32_USB)/common/core/src/ux_utility_memory_byte_pool_create.c \
  $(STM32_USB)/common/core/src/ux_utility_memory_byte_pool_search.c \
  $(STM32_USB)/common/core/src/ux_utility_memory_set.c \
  $(STM32_USB)/common/core/src/ux_utility_physical_address.c \
  $(STM32_USB)/common/core/src/ux_utility_set_interrupt_handler.c \
  $(STM32_USB)/common/core/src/ux_utility_short_get.c \
  $(STM32_USB)/common/core/src/ux_utility_short_get_big_endian.c \
  $(STM32_USB)/common/core/src/ux_utility_short_put.c \
  $(STM32_USB)/common/core/src/ux_utility_short_put_big_endian.c \
  $(STM32_USB)/common/core/src/ux_utility_string_length_check.c \
  $(STM32_USB)/common/core/src/ux_utility_string_length_get.c \
  $(STM32_USB)/common/core/src/ux_utility_string_to_unicode.c \
  $(STM32_USB)/common/core/src/ux_utility_unicode_to_string.c \
  $(STM32_USB)/common/core/src/ux_utility_virtual_address.c \
  $(STM32_USB)/common/core/src/ux_system_error_handler.c \
  $(STM32_USB)/common/core/src/ux_system_initialize.c \
  $(STM32_USB)/common/core/src/ux_system_uninitialize.c \
  $(STM32_USB)/common/core/src/ux_device_stack_alternate_setting_get.c \
  $(STM32_USB)/common/core/src/ux_device_stack_alternate_setting_set.c \
  $(STM32_USB)/common/core/src/ux_device_stack_class_register.c \
  $(STM32_USB)/common/core/src/ux_device_stack_class_unregister.c \
  $(STM32_USB)/common/core/src/ux_device_stack_clear_feature.c \
  $(STM32_USB)/common/core/src/ux_device_stack_configuration_get.c \
  $(STM32_USB)/common/core/src/ux_device_stack_configuration_set.c \
  $(STM32_USB)/common/core/src/ux_device_stack_control_request_process.c \
  $(STM32_USB)/common/core/src/ux_device_stack_descriptor_send.c \
  $(STM32_USB)/common/core/src/ux_device_stack_disconnect.c \
  $(STM32_USB)/common/core/src/ux_device_stack_endpoint_stall.c \
  $(STM32_USB)/common/core/src/ux_device_stack_get_status.c \
  $(STM32_USB)/common/core/src/ux_device_stack_host_wakeup.c \
  $(STM32_USB)/common/core/src/ux_device_stack_initialize.c \
  $(STM32_USB)/common/core/src/ux_device_stack_interface_delete.c \
  $(STM32_USB)/common/core/src/ux_device_stack_interface_get.c \
  $(STM32_USB)/common/core/src/ux_device_stack_interface_set.c \
  $(STM32_USB)/common/core/src/ux_device_stack_interface_start.c \
  $(STM32_USB)/common/core/src/ux_device_stack_microsoft_extension_register.c \
  $(STM32_USB)/common/core/src/ux_device_stack_set_feature.c \
  $(STM32_USB)/common/core/src/ux_device_stack_transfer_abort.c \
  $(STM32_USB)/common/core/src/ux_device_stack_transfer_all_request_abort.c \
  $(STM32_USB)/common/core/src/ux_device_stack_transfer_request.c \
  $(STM32_USB)/common/core/src/ux_device_stack_uninitialize.c \
  $(STM32_USB)/common/core/src/ux_device_stack_tasks_run.c \
  $(STM32_USB)/common/core/src/ux_device_stack_transfer_run.c \
  $(STM32_USB)/common/usbx_device_classes/src/ux_device_class_cdc_acm_activate.c \
  $(STM32_USB)/common/usbx_device_classes/src/ux_device_class_cdc_acm_control_request.c \
  $(STM32_USB)/common/usbx_device_classes/src/ux_device_class_cdc_acm_deactivate.c \
  $(STM32_USB)/common/usbx_device_classes/src/ux_device_class_cdc_acm_entry.c \
  $(STM32_USB)/common/usbx_device_classes/src/ux_device_class_cdc_acm_initialize.c \
  $(STM32_USB)/common/usbx_device_classes/src/ux_device_class_cdc_acm_ioctl.c \
  $(STM32_USB)/common/usbx_device_classes/src/ux_device_class_cdc_acm_unitialize.c \
  $(STM32_USB)/common/usbx_device_classes/src/ux_device_class_cdc_acm_write_with_callback.c \
  $(STM32_USB)/common/usbx_device_classes/src/ux_device_class_cdc_acm_read_run.c \
  $(STM32_USB)/common/usbx_device_classes/src/ux_device_class_cdc_acm_tasks_run.c \
  $(STM32_USB)/common/usbx_device_classes/src/ux_device_class_cdc_acm_write_run.c

ASM_SOURCES =  \
  $(DIR_SYS)/src/startup_stm32u535xx.s

AS_DEFS = 

C_DEFS =  \
-DUSE_FULL_LL_DRIVER \
-DUX_INCLUDE_USER_DEFINE_FILE \
-DHSE_VALUE=8000000 \
-DHSI_VALUE=16000000 \
-DLSI_VALUE=32000 \
-DVDD_VALUE=3300 \
-DSTM32U535xx


C_INCLUDES =  \
-I$(DIR_COMMON) \
-I$(DIR_DRV) \
-I$(DIR_HAL) \
-I$(DIR_SDK) \
-I$(DIR_SYS) \
-I$(DIR_STM) \
-I$(STM32_HAL)/Inc \
-I$(STM32_USB)/common/core/inc \
-I$(STM32_USB)/ports/generic/inc \
-I$(STM32_USB)/common/usbx_stm32_device_controllers \
-I$(STM32_USB)/common/usbx_device_classes/inc \
-I$(DIR_SYS)/device/STM32U5xx/inc \
-I$(DIR_SYS)/inc

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

LDSCRIPT = $(DIR_SYS)/linker/STM32U535xx.ld

LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections,--print-memory-usage 


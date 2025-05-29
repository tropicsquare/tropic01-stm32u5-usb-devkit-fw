#include "usb_device.h"

#include "main.h"
#include "sys.h"
#include "log.h"
#include "irq.h"

#include "ux_api.h"
#include "ux_dcd_stm32.h"

#include "ux_device_descriptors.h"
#include "ux_device_cdc_acm.h"

LOG_DEF("USB");

PCD_HandleTypeDef hpcd_usb_drd_fs;

#define USB_MEM_POOL_SIZE      (8*1024)

static u32 usb_mem_pool_buffer[USB_MEM_POOL_SIZE/sizeof(u32)];

static ULONG cdc_acm_interface_number;
static ULONG cdc_acm_configuration_number;
static UX_SLAVE_CLASS_CDC_ACM_PARAMETER cdc_acm_parameter;
static UINT usbd_change_function(ULONG Device_State);

void HAL_PCD_MspInit(PCD_HandleTypeDef* hpcd)
{
    if (hpcd->Instance == USB_DRD_FS)
    {
        __HAL_RCC_USB_FS_CLK_ENABLE();
        irq_enable(USB_IRQn, USB_ISR_PRIO);
    }
}

static void _usb_drd_fs_pcd_init(void)
{
    hpcd_usb_drd_fs.Instance = USB_DRD_FS;
    hpcd_usb_drd_fs.Init.dev_endpoints = 8;
    hpcd_usb_drd_fs.Init.speed = PCD_SPEED_FULL;
    hpcd_usb_drd_fs.Init.phy_itface = PCD_PHY_EMBEDDED;
    hpcd_usb_drd_fs.Init.Sof_enable = DISABLE;
    hpcd_usb_drd_fs.Init.low_power_enable = DISABLE;
    hpcd_usb_drd_fs.Init.lpm_enable = DISABLE;
    hpcd_usb_drd_fs.Init.battery_charging_enable = DISABLE;
    hpcd_usb_drd_fs.Init.vbus_sensing_enable = DISABLE;
    hpcd_usb_drd_fs.Init.bulk_doublebuffer_enable = DISABLE;
    hpcd_usb_drd_fs.Init.iso_singlebuffer_enable = DISABLE;

    if (HAL_PCD_Init(&hpcd_usb_drd_fs) != HAL_OK)
    {
        Error_Handler();
    }
}

static bool _usb_cdc_init(void)
{
    u8 *device_framework_high_speed;
    u8 *device_framework_full_speed;
    ULONG device_framework_hs_length;
    ULONG device_framework_fs_length;
    ULONG string_framework_length;
    ULONG language_id_framework_length;
    u8 *string_framework;
    u8 *language_id_framework;

    // Initialize USBX Memory
    if (ux_system_initialize((u8 *)usb_mem_pool_buffer, sizeof(usb_mem_pool_buffer), UX_NULL, 0) != UX_SUCCESS)
    {
        return false;
    }

    // Get Device Framework High Speed and get the length
    device_framework_high_speed = USBD_Get_Device_Framework_Speed(USBD_HIGH_SPEED, &device_framework_hs_length);

    // Get Device Framework Full Speed and get the length
    device_framework_full_speed = USBD_Get_Device_Framework_Speed(USBD_FULL_SPEED, &device_framework_fs_length);

    // Get String Framework and get the length 
    string_framework = USBD_Get_String_Framework(&string_framework_length);

    // Get Language Id Framework and get the length
    language_id_framework = USBD_Get_Language_Id_Framework(&language_id_framework_length);

    // Install the device portion of USBX
    if (ux_device_stack_initialize(device_framework_high_speed,
                                   device_framework_hs_length,
                                   device_framework_full_speed,
                                   device_framework_fs_length,
                                   string_framework,
                                   string_framework_length,
                                   language_id_framework,
                                   language_id_framework_length,
                                   usbd_change_function) != UX_SUCCESS)
    {
        return false;
    }

    // Initialize the cdc acm class parameters for the device
    cdc_acm_parameter.ux_slave_class_cdc_acm_instance_activate   = ux_device_cdc_acm_activate;
    cdc_acm_parameter.ux_slave_class_cdc_acm_instance_deactivate = ux_device_cdc_acm_deactivate;
    cdc_acm_parameter.ux_slave_class_cdc_acm_parameter_change    = ux_device_cdc_acm_parameterchange;

    // Get cdc acm configuration number
    cdc_acm_configuration_number = USBD_Get_Configuration_Number(CLASS_TYPE_CDC_ACM, 0);

    // Find cdc acm interface number 
    cdc_acm_interface_number = USBD_Get_Interface_Number(CLASS_TYPE_CDC_ACM, 0);

    // Initialize the device cdc acm class
    if (ux_device_stack_class_register(_ux_system_slave_class_cdc_acm_name,
                                     ux_device_class_cdc_acm_entry,
                                     cdc_acm_configuration_number,
                                     cdc_acm_interface_number,
                                     &cdc_acm_parameter) != UX_SUCCESS)
    {
        return false;
    }

    return true;
}


void usb_device_init(void)
{
    _usb_drd_fs_pcd_init();

    if (! _usb_cdc_init())
    {
        LOG_ERROR("CDC init failed");
    }
    HAL_PCDEx_PMAConfig(&hpcd_usb_drd_fs, 0x00, PCD_SNG_BUF, 0x14);  // 0x00 = EP0 OUT
    HAL_PCDEx_PMAConfig(&hpcd_usb_drd_fs, 0x80, PCD_SNG_BUF, 0x54);  // 0x80 = EP0 IN
    HAL_PCDEx_PMAConfig(&hpcd_usb_drd_fs, 0x81, PCD_SNG_BUF, 0x94);  // 0x81 = EP1 IN
    HAL_PCDEx_PMAConfig(&hpcd_usb_drd_fs, 0x01, PCD_SNG_BUF, 0xD4);  // 0x01 = EP1 OUT
    HAL_PCDEx_PMAConfig(&hpcd_usb_drd_fs, 0x82, PCD_SNG_BUF, 0x114); // 0x82 = EP2 IN

    // Initialize and link controller HAL driver
    if (ux_dcd_stm32_initialize((ULONG)USB_DRD_FS, (ULONG)&hpcd_usb_drd_fs) != UX_SUCCESS)
    {
        LOG_ERROR("ux_dcd init failed");
    }

    HAL_PCD_Start(&hpcd_usb_drd_fs);
}

void usb_device_task(void)
{
    ux_device_stack_tasks_run();
    ux_device_cdc_acm_task();
}


bool usb_cdc_rx_init(usb_cdc_rx_pfunc_t rx_handler)
{
	ux_device_cdc_acm_rx_init(rx_handler);
	return (true);
}

bool usb_device_connected(void)
{
    return (ux_device_cdc_acm_connected());
}

usb_result_e usb_cdc_tx(u8* data, u16 len)
{
      if (ux_device_cdc_acm_tx(data, len))
            return (USB_RESULT_OK);
    
      return (USB_RESULT_BUSY);
}

bool usb_cdc_tx_busy(void)
{
    return (false);
}

/**
  * @brief  usbd_change_function
  *         This function is called when the device state changes.
  * @param  device_state: USB Device State
  * @retval status
  */
static UINT usbd_change_function(ULONG device_state)
{
    UINT status = UX_SUCCESS;

    switch (device_state)
    {
    case UX_DEVICE_ATTACHED:
        break;

    case UX_DEVICE_REMOVED:
        break;

    case UX_DCD_STM32_DEVICE_CONNECTED:
        break;

    case UX_DCD_STM32_DEVICE_DISCONNECTED:
        break;

    case UX_DCD_STM32_DEVICE_SUSPENDED:
        break;

    case UX_DCD_STM32_DEVICE_RESUMED:
        break;

    case UX_DCD_STM32_SOF_RECEIVED:
        break;

    default:
        break;

    }
    return status;
}

ALIGN_TYPE _ux_utility_interrupt_disable(VOID)
{
  UINT interrupt_save;
  interrupt_save = __get_PRIMASK();
  __disable_irq();
  return interrupt_save;
}

void _ux_utility_interrupt_restore(ALIGN_TYPE flags)
{
  __set_PRIMASK(flags);
}

void USB_IRQHandler(void)
{
    // printf("I\n");
    HAL_PCD_IRQHandler(&hpcd_usb_drd_fs);
}




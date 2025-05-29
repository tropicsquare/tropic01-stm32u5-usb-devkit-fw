/**
  ******************************************************************************
  * @file    ux_device_cdc_acm.c
  * @author  MCD Application Team
  * @brief   USBX Device applicative file
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "type.h"
#include "usb_device.h"
#include "ux_device_cdc_acm.h"
#include "log.h"

LOG_DEF("CDC");

UX_SLAVE_CLASS_CDC_ACM  *cdc_acm;

#define USB_HS_READ_LENGTH (512)
#define ACM_RX_BUFFER_SIZE USB_HS_READ_LENGTH
static u8 acm_rx_buffer[ACM_RX_BUFFER_SIZE];

UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_PARAMETER CDC_VCP_LineCoding;

static usb_cdc_rx_pfunc_t _rx_handler = NULL;

void ux_device_cdc_acm_rx_init(usb_cdc_rx_pfunc_t rx_handler)
{
	_rx_handler = rx_handler;
}

/**
  * @brief  ux_device_cdc_acm_activate
  *         This function is called when insertion of a CDC ACM device.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
void ux_device_cdc_acm_activate(void *cdc_acm_instance)
{
    cdc_acm = (UX_SLAVE_CLASS_CDC_ACM*)cdc_acm_instance;
    
    LOG_DEBUG("ux_device_cdc_acm_activate()");
    
    CDC_VCP_LineCoding.ux_slave_class_cdc_acm_parameter_baudrate = 115200;
    CDC_VCP_LineCoding.ux_slave_class_cdc_acm_parameter_data_bit = 8;
    CDC_VCP_LineCoding.ux_slave_class_cdc_acm_parameter_parity   = 0;
    CDC_VCP_LineCoding.ux_slave_class_cdc_acm_parameter_stop_bit = 0; // stop bits-1 

    // Set device class_cdc_acm with default parameters
    if (ux_device_class_cdc_acm_ioctl(cdc_acm, UX_SLAVE_CLASS_CDC_ACM_IOCTL_SET_LINE_CODING, &CDC_VCP_LineCoding) != UX_SUCCESS)
    {
        LOG_ERROR("IOCTL failed");
    }
}

/**
  * @brief  ux_device_cdc_acm_deactivate
  *         This function is called when extraction of a CDC ACM device.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
void ux_device_cdc_acm_deactivate(void *cdc_acm_instance)
{
    UX_PARAMETER_NOT_USED(cdc_acm_instance);
    cdc_acm = UX_NULL;
}

/**
  * @brief  ux_device_cdc_acm_parameterchange
  *         This function is invoked to manage the CDC ACM class requests.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
void ux_device_cdc_acm_parameterchange(void *cdc_acm_instance)
{
    UX_PARAMETER_NOT_USED(cdc_acm_instance);
}

bool ux_device_cdc_acm_connected(void)
{
    if (cdc_acm == UX_NULL)
        return (false);

   if (_ux_system_slave->ux_system_slave_device.ux_slave_device_state == UX_DEVICE_CONFIGURED)
       return (true);

   return (false);
}

void tty_debug (const ascii *buf, size_t count);

bool ux_device_cdc_acm_tx(u8* data, u16 len)
{
    UX_SLAVE_CLASS_CDC_ACM *ctx = cdc_acm;
    ULONG actual_length;

    if (ctx == UX_NULL)
        return (false);

    if (ux_device_class_cdc_acm_write_run(ctx, (UCHAR *)data, len, &actual_length) != UX_STATE_NEXT)
        return (false);
    
    if (actual_length != len)
        return (false);

    return (true);
}

void ux_device_cdc_acm_task(void)
{
    ULONG actual_length;
    UX_SLAVE_DEVICE *device;
    ULONG read_length;
    UX_SLAVE_CLASS_CDC_ACM *ctx = cdc_acm;

    device = &_ux_system_slave->ux_system_slave_device;

    if (ctx == UX_NULL)
        return;

    if (device->ux_slave_device_state != UX_DEVICE_CONFIGURED)
        return;

    read_length = (_ux_system_slave->ux_system_slave_speed == UX_HIGH_SPEED_DEVICE) ? USB_HS_READ_LENGTH : 64;

    UINT status = ux_device_class_cdc_acm_read_run(ctx, (UCHAR *)acm_rx_buffer, read_length, &actual_length);
        
    if (status <= UX_STATE_ERROR)
        return;
    
    if (status == UX_STATE_NEXT)
    {
        if (actual_length != 0)
        {
          	if (_rx_handler != NULL)
	        {
		        _rx_handler(acm_rx_buffer, actual_length);
	        }
        }
    }
}




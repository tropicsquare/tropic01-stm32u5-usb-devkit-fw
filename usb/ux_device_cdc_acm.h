/**
  ******************************************************************************
  * @file    ux_device_cdc_acm.h
  * @author  MCD Application Team
  * @brief   USBX Device CDC ACM interface header file
  ******************************************************************************
  */

#ifndef USBD_CDC_ACM_H
#define USBD_CDC_ACM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ux_api.h"
#include "ux_device_class_cdc_acm.h"

void ux_device_cdc_acm_rx_init(usb_cdc_rx_pfunc_t rx_handler);
void ux_device_cdc_acm_activate(void *cdc_acm_instance);
void ux_device_cdc_acm_deactivate(void *cdc_acm_instance);
void ux_device_cdc_acm_parameterchange(void *cdc_acm_instance);
bool ux_device_cdc_acm_connected(void);
bool ux_device_cdc_acm_tx(u8* data, u16 len);

void ux_device_cdc_acm_task(void);

#ifdef __cplusplus
}
#endif

#endif  // ! USBD_CDC_ACM_H

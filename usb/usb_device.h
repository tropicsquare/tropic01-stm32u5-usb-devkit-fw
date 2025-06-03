/**
  ******************************************************************************
  * @file    usb_device.h
  * @author  Tropicsquare
  * @brief   USB Device main API header
  ******************************************************************************
  */

#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "type.h"

typedef void (*usb_cdc_rx_pfunc_t)(uint8_t* pbuf, u32 len);

typedef enum {
    USB_RESULT_OK = 0,
    USB_RESULT_BUSY = 0xFF,
} usb_result_e;

void usb_device_init(void);
void usb_device_task(void);
bool usb_device_connected(void);

bool         usb_cdc_rx_init(usb_cdc_rx_pfunc_t rx_handler);
usb_result_e usb_cdc_tx(u8 *data, u16 len);
bool         usb_cdc_tx_busy(void);

#ifdef __cplusplus
}
#endif

#endif // ! USB_DEVICE_H


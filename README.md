# About

This is the minimal USB firmware for (todo the Tropic Square TROPIC01 development kit), based on the **STM32U535** microcontroller.
Its primary function is to act as a USB-to-SPI master converter, master then talks to TROPIC01's slave SPI interface.

For hardware details, see the (todo ts13-dev-kit) hardware GIT repository.

> [!WARNING]
> This firmware only facilitates raw data transfer between the TROPIC01 and a host-side application.
> Key handling and security must be managed by the host application.

> [!TIP]
> Although this firmware is minimalistic by design, the STM32U535 MCU offers significantly more capabilities (e.g., secure key storage).
> Users are encouraged to develop custom firmware to leverage these features for enhanced functionality.


# Usage

For basic low-level test open **any serial portapplication**, then follow instruction for particular API version in [`API/`](API/README.md) folder.

> [!TIP]
> To see the ful potential of TROPIC01, we recommend to explore and evaluate examples using [libtropic-linux](https://github.com/tropicsquare/libtropic-linux/todo) repository.
> For bash users we provide also command line application [libtropic-util](https://github.com/tropicsquare/libtropic-util/todo).


# API

Various firmware tags might have various API, please follow description in [`API/`](API/README.md) folder.


# Updating devkit's firmware

Minimal requirements for compilation:

 * linux compatible os
 * arm-none-eabi-gcc toolchain
 * dfu-util tool

Install toolchain:
```
apt install gcc-arm-none-eabi 
```

Install `dfu-util`, should be available in most Debian-based distributions:
```
$ apt install dfu-util
```

> [!IMPORTANT]
> Do not forget to update submodules with `git submodule update --init --recursive`

## Build firmware

Once tools are installed, build firmware with following commands:
```
$ cd ./app
$ make clean
$ make
```

## Flash firmware

1) Build FW (see chapter [Build firmware](#build-firmware))
2) Press button and (while pressed) connect to USB, then release.
   Devkit will be connected as USB device "STMicroelectronics STM Device in DFU Mode"
3) run following command `dfu-util`:
   ```bash
   dfu-util -a 0 -s 0x08000000:leave -D build/app.bin
   ```
4) after successful update the device starts FW automatically


# Project structure

Description of all folders:

```
├── app                 # application main directory
│   └── build           # application build directory
├── doc                 # device documentation
├── hw                  # hardware definitions
├── usb                 # usb interface to usb library
└── sdk                 # sdk root (stm32), may be submodule
    ├── common          # universal libraries
    ├── drv_*           # stm32 drivers
    ├── hal             # HW abstraction layer (universal)
    └── stm32
        ├── CMSIS
        │   ├── device  # MCU definitions 
        │   ├── inc     # stm32 core definitions
        │   ├── linker  # linker files
        │   └── src     # stm32 core sources
        ├── STM32_USBX_Library   # MS Azure USBX library provided by STM
        └── STM32U5xx_HAL_Driver # submodule (STMicroelectronics)
```

Project uses nested git submodules. Use `git submodule update --init --recursive`.


# Manufacturing notes

USB identification (see `./usb/ux_device_descriptors.h`):
```
USBD_VID                 =  1155 == 0x0483 == "STMicroelectronics"
USBD_PID_FS              = 22336 == 0x5740 == "Virtual COM Port"
USBD_MANUFACTURER_STRING = "TropicSquare"
USBD_PRODUCT_STRING_FS   = "SPI interface"
```

For manufacturing purposes there is `flash_tool.sh` script, which does 
automatic flashing and communication testing.

It requires some tools installed : 
```
apt install coreutils usbutils dfu-util
```

Prior to use, you have to build the FW ant then simply execute the script and follow instructions.

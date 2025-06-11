# About

Tropic Square TROPIC01 minimal USB dev-kit firmware for MCU STM32U535.
Main function is USB to SPI master converter.
Designed for PCB TS13xx (first compatible version is TS1302), see HW repo ts13-dev-kit.git
Connected device work as USB-CDC class (in Linux and Android it appears as `/dev/ttyACM*`).

# Usage

For basic low-level test you may use any terminal application, see  chapter [Function](#Function).
For real usage of TROPIC01 chip use [Libtropic library](https://github.com/tropicsquare/libtropic-util)

# STM32 based firmware

Minimal requirements for compilation:

 * linux compatible os
 * arm-none-eabi-gcc

Tools installation i.e.:

```
apt install gcc-arm-none-eabi
```

Debugging tools install (optional):

```
apt install stlink-tools
```

# Project structure

Current setup for CPU STM32U535.

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

## FW build

```
$ cd ./app
$ make clean
$ make
```

Optional flashing using ST-Link
```
$ make flash
```

How to flash the new FW without ST-link see chapter [FW update](#FW update).

# Function

In Linux compatible OS it works as USB character device /dev/ttyACMx. 

USB identification (see `./usb/ux_device_descriptors.h`):
```
USBD_VID                 =  1155 == 0x0483 == "STMicroelectronics"
USBD_PID_FS              = 22336 == 0x5740 == "Virtual COM Port"
USBD_MANUFACTURER_STRING = "TropicSquare"
USBD_PRODUCT_STRING_FS   = "SPI interface"
```

Communication uses ASCII characters lines ended by `\r` or `\n` (0x0D or 0x0A).

To send SPI data just put HEX values string (i.e. "0A1B2C3D") and new line character. 
Any such line will issue CS low signal and line end will issue CS high signal.
Character `x` or `\` at the end of line will cause leaving CS low (continuous data reading).

Received data will be printed the same way (same number bytes as send).

Example SPI communication with TROPIC01 :
```
> 010202002b98
< 01FFFFFFFFFF
> aaffffffffffffff
< 010400000300E073
```

## Commands

* `HELP` : Print quick help
* `AUTO` : Show automatic response reading status.
* `AUTO=<mode>[,<get_resp>,<no_resp>]` : Automatic response reading set \
    `<mode>` : 1 = enable, 0 = disable (default 0) \
    `<get_resp>` : HEX value of byte used for reading \
    `<no_resp>` : HEX value of byte which mean no response available
* `CLKDIV` : Show SCK clock divisor current value.
* `CLKDIV=<n>` : SCK clock divisor set \
    `<n>` : 2,4,8,16,32,64,128 or 256 to select SCK frequency as `48MHz / <n>`
* `CS` : Show SPI CS state (1 == active == LOW) 
* `CS=<n>` : Set SPI CS state (0 == idle, 1 == active == LOW) 
* `GPO` : Show GPO state 
* `ID` : Request product id
* `PWR` : Show power status.
* `PWR=<mode>` : Get/set target power \
    `<mode>` : 1 = power ON, 0 = power OFF
* `RESET` : Instant reset
* `SN`: Request product serial number, same as `iSerial` identification on USB.
* `VER` : Request version information

Execution of any command is finished with message "OK" or "ERROR" (see below).

## Error results

In case of some error, response "`ERROR: <reason>`" will be send.

Possible error results `<reason>`:

* "illegal parameter"
* "invalid parameter"
* "missing parameter"
* "unknown command"
* "USB RX overflow !"

## FW update

This device may be updated without any special HW tool, just using USB. \
You need utility `dfu-util` which can be installed on Debian-based distros:
```
$ apt install dfu-util
```

1) Build FW (see chapter [FW build](#fw-build))
2) Press button and while pressed connect to USB
   you can see connected USB device "STMicroelectronics STM Device in DFU Mode"
3) run command `dfu-util` like that :
   `$ dfu-util -a 0 -s 0x08000000:leave -D build/app.bin`
4) after successful update the device starts FW automatically

# Manufacturing 

For manufacturing purposes there is `flash_tool.sh` script, which does 
automatic flashing and communication testing.

It requires some tools installed : 
```
apt install coreutils usbutils dfu-util
```

Prior to use, you have to build the FW ant then simply execute the script and follow instructions.

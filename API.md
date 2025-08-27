# API

This file describes USB devkit's communication interface version 1.0.0 exposed over serial port.

# Communication

When connected to Linux compatible OS, the device appears as a USB CDC (Communications Device Class) interface (e.g., **/dev/ttyACM\*** on Linux and Android).

Communication uses ASCII characters lines ended by `\r` or `\n` (0x0D or 0x0A).

> [!IMPORTANT]
> To send SPI data just put HEX values string (i.e. "0A1B2C3D") and new line character.
> Any such line will issue CS low signal and line end will issue CS high signal.
> Character `x` or `\` at the end of line will cause leaving CS low (continuous data reading).

Received data will be printed the same way (same number of bytes as were sent).

Example SPI communication with TROPIC01 :
```
> 010202002b98
< 01FFFFFFFFFF
> aaffffffffffffff
< 010400000300E073
```

### All Commands

Beside just transferring data between host and TROPIC01, this usb device also has own set of commands:

* `HELP` : Print quick help
* `AUTO` : Show automatic response reading status.
* `AUTO=<mode>[,<get_resp>,<no_resp>]` : Automatic response reading set \
    `<mode>` : 1 = enable, 0 = disable (default 0) \
    `<get_resp>` : HEX value of byte used for reading \
    `<no_resp>` : HEX value of byte which mean no response available
* `BUTTON` : Get button state.
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

Execution of any command is finished with message "OK" or "`ERROR: <reason>`".

Possible error results `<reason>`:

* "illegal parameter"
* "invalid parameter"
* "missing parameter"
* "unknown command"
* "USB RX overflow !"

### LED signalization

 * LED OFF == no power
 * LED ON == power OK and USB connection established correctly (ready to use)
 * Blinking regularly == power OK but USB not working properly
 * Short flashes during SPI transaction

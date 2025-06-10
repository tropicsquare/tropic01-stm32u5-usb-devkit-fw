#!/bin/bash
#
# Script Name:    flash_tool.sh
# Description:    Does automatic flashing and communication testing of TS13xx board.
# Author:         Tropic Square
# Date:           2025-06-10
# Version:        1.0.0
#
# Usage:          Run script ./flash_tool.sh and then connect TS13xx device with pressed button and follow instructions.
#
# Notes:          Make sure no other /dev/ttyACM* devices connected to this computer.
#
# Dependencies:   coreutils usbutils dfu-util
#
# License:        BSD, see LICENSE.txt

FIRMWARE="app/build/app.bin"
ACM_DEVICE="/dev/ttyACM0"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' 

err() {
    echo -e "${RED}ERROR: $1 ${NC}"
}

clean() {
    echo -n "$1" | tr -d '\r\n'
}

parse_version() {
    local hex="$1"

    # Convert hex string to array of bytes
    local -a s
    for ((i=0; i<${#hex}; i+=2)); do
        byte="0x${hex:$i:2}"
        s+=($byte)
    done

	# get version from TS_L2_GET_INFO_REQ response
    local major=$((s[6]))
    local minor=$((s[5]))
    local patch=$((s[4]))

    printf "v%d.%d.%d\n" "$major" "$minor" "$patch"
}

require_command() {
    if ! command -v "$1" >/dev/null 2>&1; then
        err "Required command '$1' not found." >&2
        exit 1
    fi
}

echo "STM32 DFU Auto-Flasher"

require_command stty
require_command lsusb
require_command dfu-util

if [ ! -e $FIRMWARE ]; then
    err "Firmware $FIRMWARE not fonud, you need to compile it"
    exit 1
fi

if [ -e  $ACM_DEVICE ]; then
    err "Device $ACM_DEVICE already connected, cant continue. \n       Disconnect the device or update this script."
    exit 2
fi

while true; do

	echo ""
	echo "Waiting for DFU device (connect TS13xx board with pressed button) ..."
    
	# Wait for DFU device to appear
    while ! lsusb | grep -i "STM.*DFU" >/dev/null; do
        sleep 1
    done

    echo "DFU device detected, flashing ... "

    # Flash firmware using dfu-util
    FLASH_OUTPUT=$(dfu-util -a 0  -s 0x08000000:leave -D "$FIRMWARE" 2>&1)

    if echo "$FLASH_OUTPUT" | grep -q "File downloaded successfully"; then
        echo "Flash successful."
    else
        echo "$FLASH_OUTPUT"
        err "Flashing failed !"
        sleep 5
        echo "continue ... "
        continue
    fi

    echo "Waiting for device to reset and enumerate as $ACM_DEVICE ..."

    # Wait for serial device to appear
    for i in {1..5}; do
        if [ -e  $ACM_DEVICE ]; then
            break
        fi
        sleep 1
    done

    if [ ! -e $ACM_DEVICE ]; then
        err "$ACM_DEVICE not found after flashing."
		continue
	fi

	echo "Deviece found, testing ... "
	stty -F $ACM_DEVICE 115200 time 5 cs8 -hupcl -icrnl -ixon -ixoff -isig -icanon -iexten -echo
	
	echo "SN" > $ACM_DEVICE &
	read -t 0.1 -r RSP < $ACM_DEVICE
	SN=$(echo -n "$RSP" | grep 'SN')
	SN=$(clean "$SN")
	echo -e "${YELLOW}Board $SN ${NC}"

	# send TS_L2_GET_INFO_REQ (see tropic01_l2_api.h)
	echo "010202002b98" > $ACM_DEVICE &
	read -t 0.1 -r RSP < $ACM_DEVICE
	RSP=$(clean "$RSP")
	
	if [[ "$RSP" == "01FFFFFFFFFF" ]]; then
		echo -e "${GREEN}Request send OK ${NC}"
		sleep 0.1
		echo "aaffffffffffffffff" > $ACM_DEVICE &
		read -t 0.1 -r RSP < $ACM_DEVICE
		VER=$(clean "$RSP")
		HVER=$(parse_version "$VER")
		echo -e "${GREEN}Response: $VER == Chip version: ${YELLOW} $HVER ${NC}"
	else
		err "Unexpected chip response $RSP"
	fi
done


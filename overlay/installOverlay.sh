#!/bin/bash

export SLOTS=/sys/devices/bone_capemgr.9/slots
export PINS=/sys/kernel/debug/pinctrl/44e10800.pinmux/pins

CURRENT_DIR=$(pwd)
	
cd /lib/firmware
echo Install Overlay...
echo BB-OLED > $SLOTS

#cd $CURRENT_DIR

#!/bin/sh

# Green LED lights up when MTP firmware update is complete
# Switch 'OFF' and 'ON' the board after the update is done

../../../util/app_switch usb_dfu_bl
dfu-util -v -d 0x108c:0xab3d -a RAM -D usb_mtp.pkg -R

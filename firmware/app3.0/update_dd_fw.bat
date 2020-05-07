:: This script updates APP3.0 board with Development Desktop firmware
:: Switching to bootloader mode is automatic !

@echo off
..\..\util\app_switch usb_dfu_bl
..\..\util\usb-dfu\dfu-util -d 0x108c:0xab3d -a FLASH -D DevelopmentDesktop_2.0_Firmware_v0.8.bin -R
pause
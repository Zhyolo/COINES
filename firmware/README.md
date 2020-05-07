# Firmware

## **Application Board 2.0**
### app2.0/DevelopmentDesktop_2.0_Firmware_V3.4.fwu2
- The default firmware preloaded in APP2.0
- Required to use [Development Desktop 2.0](https://www.bosch-sensortec.com/software-tools/tools/development-desktop-software/), COINES and [coinespy](https://pypi.org/project/coinespy/)

### app2.0/coines_bootloader/coines_usb_dfu_bl.pkg
- Required to run COINES examples directly on APP2.0 board's microcontroller
- Comes along with `DevelopmentDesktop_2.0_Firmware_V3.4.fwu2` firmware
- People using `DevelopmentDesktop_2.0_Firmware_V3.1.fwu2` should run `update_bootloader` to fix flash memory lockout issue.

### app2.0/default_bootloader/Bootloader_APP2.0.bin
- Factory programmed default bootloader for APP2.0 board
- Supports USB and Bluetooth modes
- Flash this binary if you have accidentaly overwritten/erased the flash memory
- To flash `Bootloader_APP2.0.bin`,
  - Install [BOSSA](https://github.com/shumatech/BOSSA/releases) software and locate `bossac`
  - Short **J203** with jumper and power on the board to enter SAM-BA mode
  - Use the below command to flash the binary
```bash
$ bossac -e -w -v -b Bootloader_APP2.0.bin -U -p <com_port>
```
- Run `update_dd_fw.bat` after bootloader programming is done.
---
## **Application Board 3.0**
### app3.0/DevelopmentDesktop_2.0_Firmware_v0.8.bin
- The default firmware preloaded in APP3.0
- Required to use [Development Desktop 2.0](https://www.bosch-sensortec.com/software-tools/tools/development-desktop-software/), COINES and [coinespy](https://pypi.org/project/coinespy/)

### app3.0/bootloader_update/usb_dfu_bootloader.pkg
- Bootloader update package for APP3.0
- Comes preloaded on APP3.0 board's microcontroller
- Run `update_bootloader` script to update APP3.0 bootloader (requires a atleast a old bootloader!)
- Power on the board with T2 button pressed to go to bootloader mode (Blue LED lights up)
- Source code available in `coines_api/mcu_app30/usb_dfu_bootloader`

### app3.0/mtp_fw_update/usb_mtp.pkg
- MTP firmware update package for APP3.0
- Comes preloaded on APP3.0 board's microcontroller
- Required to read out files from APP3.0 external flash memory (The board shows up as a USB storage device)
- Run `update_mtp_fw` script to update APP3.0 MTP firmware (requires the bootloader to be updated)
- Power on the board with T1 button pressed to go to MTP mode (Green LED lights up)
- Format the storage device for first use.
- Source code available in `coines_api/mcu_app30/usb_mtp`
---
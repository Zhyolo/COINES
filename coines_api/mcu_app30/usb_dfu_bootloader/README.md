# APP3.0 USB DFU Bootloader

A USB bootloader for APP3.0/nRF52840 chip complying with [USB Device Class Specification for Device Firmware Upgrade](https://www.usb.org/sites/default/files/DFU_1.1.pdf). 

The sample USB serial bootloader provided in Nordic SDK was complex and was meant to be used in end-user devices. Hence a developer/hobbyist friendly bootloader was required.

## Key features
- Code download to RAM or FLASH
- Code read back (upload) from RAM or FLASH (Useful for taking firmware backups)
- Works with Windows, Linux, macOS and Android(via Chrome)
- No driver installation required on Windows 8 and above, Linux and macOS
- Tools available
  - [dfu-util](http://dfu-util.sourceforge.net/)
  - [dfu-tool](http://manpages.ubuntu.com/manpages/cosmic/man1/dfu-tool.1.html)
  - [WebDFU](https://devanlai.github.io/webdfu/dfu-util/) (Works only on Google Chrome,Chromium and Opera)

-------------------------------------------------------------------------------

## Quick Start

### Building the bootloader

```
$ mingw32-make
```
### Flashing the bootloader

```
$ mingw32-make flash
```
The above command does the following
- Writes MBR & BLE Softdevice - 0 kB to 160 kB area
- USB DFU Bootloader - 960 kB to 1024 kB area
- Writes `0xF0000` to `uicr_bootloader_start_address` (`0x10001014`) register

### Using the bootloader

Put the board to bootloader mode by powering on the board with T2 button pressed. 

#### Write firmware to Flash memory

```
$ dfu-util -a FLASH -D fw.bin
```

#### Write firmware to RAM and run. (`-R` is mandatory)

```
$ dfu-util -a RAM -D fw.bin -R
```

#### Read firmware from Flash memory
```
$ dfu-util -a FLASH -U fw_bkup.bin
```

#### Read firmware from RAM
```
$ dfu-util -a RAM -U fw_bkup.bin
```

If 2 (or) more DFU devices are connected, specify DFU device serial string

```
$ dfu-util --serial APP3.0-DFU -a FLASH -D test_app_flash.bin
```

> Windows 7 users require to install WinUSB driver before trying to download code via USB DFU. Try [Zadig tool](https://zadig.akeo.ie/).
>
> Linux users need to add `udev` rule to use `dfu-util` without `sudo` 

### Linker script settings

| Location | Code                    | Data                    | Code space | Data space|
|----------|:-----------------------:|:-----------------------:|:----------:|:---------:|
| RAM      | 0x00810000 - 0x0083FFF0 | 0x20000008 - 0x2000FFFF |   ~192kB   |  64kB     |
| RAM *    | 0x00810000 - 0x0083FFF0 | 0x20003000 - 0x2000FFFF |   ~192kB   |  52kB     |
| FLASH    | 0x00030000 - 0x000F0000 | 0x20000008 - 0x2003FFF0 |   768kB    |  ~256kB   |
| FLASH *  | 0x00030000 - 0x000F0000 | 0x20003000 - 0x2003FFF0 |   768kB    |  ~244kB   |

\* - BLE enabled

Last 16 Bytes are reserved for bootloader invocation and switching between applications (Eg: USB MTP)


### Flash memory layout
|         Firmware         |   Space   |  Address range       | Used space |
|--------------------------|:---------:|:--------------------:|:----------:|
| MBR & BLE Softdevice     | 160 kB    | 0x00000 - 0x28000    | 152 kB     |
| USB MTP Firmware         | 32 kB     | 0x28000 - 0x30000    | 26 kB      |
| Application              | 768 kB    | 0x30000 - 0xF0000    | <100 kB    |
| USB DFU Bootloader       | 64 kB     | 0xF0000 - 0x100000   | 13.4 kB    |

-------------------------------------------------------------------------------
## Invoking Bootloader

### Hardware

- Power on board with T2 button pressed

### Software

From Application Software
- Write "COIN" to `MAGIC_LOCATION` (0x2003FFF4)
- Write `0x0` or `0xF0000` to `APP_START_ADDR` (0x2003FFF8)
- Call `NVIC_SystemReset()`

```c
#define MAGIC_LOCATION     ((uint32_t *)0x2003FFF4)
#define APP_START_ADDR    *((uint32_t *)0x2003FFF8)

memcpy(MAGIC_LOCATION,"COIN",4); // *MAGIC_LOCATION = 0x4E494F43; // 'N','O','I','C'
APP_START_ADDR = 0xF0000; // APP_START_ADDR = 0x0;
NVIC_SystemReset(); // Soft reset
```
The same principle can be used for jumping to `USB MTP firmware` by writing `0x28000` to `APP_START_ADDR`.

-------------------------------------------------------------------------------


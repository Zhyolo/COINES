# APP3.0 BLE DFU Tool

`app30-ble-dfu` is a PC based tool written in Python to perform Device Firmware Upgrade (DFU) using native BLE hardware.

This tool works with nRF5 devices with [Adafruit nRF52 bootloader](https://github.com/adafruit/Adafruit_nRF52_Bootloader) and bootloader from Nordic SDK v11 and below.

Works with latest Bluetooth v4.0 USB dongles and recent notebook PCs with Bluetooth. Tested with CSR8510 dongle in Windows 10 (Build 16299 and above) and Ubuntu 18.04 LTS.

```
C:\>python app30-ble-dfu.py -d <device_mac_address> -f <firmware_binary_file>
```

---

## Usage

### Requirements
- [Python 3](https://www.python.org/downloads/)
- [bleak](https://pypi.org/project/bleak/) library

### Install dependencies in `requirements.txt`
```
$ pip install -r requirements.txt
```
### Perform DFU

```
C:\Tools> python app30-ble-dfu.py -d F4:2C:51:FA:2C:EF -f nrf52_ble_basic.bin
Application Board 3.0 BLE DFU tool
Bosch Sensortec GmbH (C) 2020

Connected to F4:2C:51:FA:2C:EF
nRF5 BLE DFU service found - v0.8

Download  [=========================]  100 % (28.07 kB/28.07 kB)

DFU Success !
```

### Show help
```
C:\Tools> python app30-ble-dfu.py --help
Application Board 3.0 BLE DFU tool
Bosch Sensortec GmbH (C) 2020

usage: app30-ble-dfu.py [-h] -d DEVICE_MAC_ADDR -f FIRMWARE_BIN_FILE

optional arguments:
  -h, --help            show this help message and exit

required arguments:
  -d DEVICE_MAC_ADDR    Specify device MAC address
  -f FIRMWARE_BIN_FILE  Specify firmware binary
```
---

## Background information

BLE in the days of Windows 7, 8 required the use of specialized dongle like [Silicon Labs BLED112](https://www.silabs.com/wireless/bluetooth/bluegiga-low-energy-legacy-modules/device.bled112) , [TI CC2540 BLE USB](http://www.ti.com/tool/TIDC-CC2540-BLE-USB), [Nordic nRF51 dongle](https://www.nordicsemi.com/Software-and-tools/Development-Kits/nRF51-Dongle), etc ., which is locked to vendor specific software.

With the advent of Windows 10 Fall Creator's Update (Build 16299/Version 1709), BLE is well supported natively. This tool is an attempt to push the use of native Bluetooth hardware built in notebook PC and USB dongles without getting locked into vendor specific hardware and software.

---
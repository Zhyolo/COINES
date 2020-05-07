# Drivers

## Windows
- Run `app_board_usb_driver.exe` to install the below USB drivers
  - Development Desktop 2.0/COINES PC USB driver
  - WinUSB drivers for APP2.0 and APP3.0 boards (Required only for Windows 7)
  - USB serial drivers for APP2.0 and APP3.0 boards (Required only for Windows 7)

## Linux
- No special USB drivers are required
- However to access the board without `sudo`, install `udev` rules
- `udev` rules can be installed by running `install_driver.sh`

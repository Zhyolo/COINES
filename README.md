# Bosch Sensortec **COINES**
## **CO**mmunication with **IN**ertial and **E**nvironmental **S**ensors

**COINES** allows users to evaluate sensors using the Bosch Sensortec Application Board. Sensor configuration and data readout can be easily done using the `coines_api` from PC side using C (or) Python.

 To overcome the limitations (Eg: inaccurate delays,etc.,) due to latencies in USB communication, some C examples can also be cross-compiled and run directly on the Application Board's microcontroller.

## Quick Start
* Clone this repository
* Install GCC Toolchain and GNU Make
* Install USB drivers and libraries
  * Windows - app_board_usb_driver.exe
  * Linux - `libusb-dev` package and `udev` rules
    * Debian based distros - `sudo apt install libusb-1.0-0-dev`
    * Red Hat based distros - `sudo yum install libusbx-devel`
  * macOS - `brew install libusb`
* Connect the Bosch Sensortec Application Board 2.0 (or) 3.0 to PC with any sensor shuttle mounted. 
* Go to any example and run `make` (Eg: `template/c`)
* Run the compiled binary


## Running examples on Application Board microcontroller
* Update to the latest DD2.0 firmware ( v3.1 and above )
* Install `dfu-util`
  * Windows - Not required.Available at `util/usb-dfu`
  * Linux
    * Debian based distros - `sudo apt install dfu-util`
    * Red Hat based distros - `sudo yum instal dfu-util`
  * macOS - `brew install dfu-util`
* Get [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm), extract and add to PATH 
* Go to any example and run `make TARGET=MCU_APP20 download` (or) `make TARGET=MCU_APP30 download` (Eg: `template/c`)
* Open Application Board USB serial port with any serial terminal program (Ensure DTR signal is asserted)

## Installing and using `coinespy`
`coinespy` can be used to access the Application Board via Python
* Install Python 3.x
* Install `coinespy` by using any of the below command set.
  ```bash
  $ pip install coinespy
  ```
  ```bash
  $ cd coines_api/pc/python
  $ python setup.py install
  ```
* Go to `template/python` and run `coinespy_test.py`

## Creating new examples
* For creating new examples, see `template/c`
* Use `template/c/Makefile` as a reference for including additional C, Assembly, C++ files and binary libraries.

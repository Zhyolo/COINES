# `coines_api` - APP3.0 MCU

## Quirky things

### coines_open_comm_intf

- Initializes nRF52840 microcontroller and waits indefinitely for serial port connection(DTR should be asserted) or T2 button to be pressed. (Bluetooth is not supported) 
- Configures CPU to run at 64 MHz.
- Tries to mount filesystem in W25M02 NAND memory or does a clean format.

### coines_close_comm_intf

Flushes `stdout`/USB serial write buffer.

### coines_config_i2c_bus

- `SDO` pin made low to be consistent with PC `coines_api`
- 1.7 MHz and 3.4 MHz I2C is not supported

### coines_config_spi_bus
- SPI speed mapping is approximate

### Unsupported APIs

- `coines_config_streaming`,
- `coines_start_stop_streaming`
- `coines_read_stream_sensor_data`
- `coines_trigger_timer`

Use the below APIs instead
- `coines_attach_interrupt`
- `coines_detach_interrupt`

### Integration with standard C library
- `printf`, `puts` work with USB serial.
- `fopen`, `fclose`, `fprintf`, `fgets`, `remove` etc., work with filesystem on NAND flash

### Switching to bootloader or MTP mode 

Open and close USB serial port at 
 - 1200 baud for bootloader mode
 - 2400 baud for MTP mode.
 
See `app_switch` tool for more details.

Usage of APP2.0 pins in code intended for APP3.0 is permitted provided the shuttle EEPROM is loaded with APP2.0-APP3.0 pin mappings.

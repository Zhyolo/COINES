/**
 * Copyright (C) 2018 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    coines.h
 * @brief   This file contains COINES layer function prototypes, variable declarations and Macro definitions
 *
 */

/*!
 * @addtogroup coines_api
 * @{*/

#ifndef COINES_H_
#define COINES_H_

/* C++ Guard macro - To prevent name mangling by C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/**********************************************************************************/
/* header includes */
/**********************************************************************************/
#include <stdint.h>

/**********************************************************************************/
/* macro definitions */
/**********************************************************************************/
/*! Streaming states */
#define COINES_STREAMING_START           1
#define COINES_STREAMING_STOP            0

/*! COINES USB buffer max size */
#define COINES_DATA_BUF_SIZE             (1024)

/*! maximum no of sensor support */
#define COINES_MIN_SENSOR_ID             1
#define COINES_MAX_SENSOR_ID             2
#define COINES_MAX_SENSOR_COUNT          (COINES_MAX_SENSOR_ID + 1)

/*! coines stream response buffer size */
#define COINES_STREAM_RSP_BUF_SIZE       1048576

/*! coines success code */
#define COINES_SUCCESS                  0
/*! coines error code - failure */
#define COINES_E_FAILURE               -1
/*! coines error code - IO error */
#define COINES_E_COMM_IO_ERROR         -2
/*! coines error code - Init failure */
#define COINES_E_COMM_INIT_FAILED      -3
/*! coines error code - failure to open device */
#define COINES_E_UNABLE_OPEN_DEVICE    -4
/*! coines error code - Device not found */
#define COINES_E_DEVICE_NOT_FOUND      -5
/*! coines error code - failure to claim interface */
#define COINES_E_UNABLE_CLAIM_INTF     -6
/*! coines error code - failure to allocate memory */
#define COINES_E_MEMORY_ALLOCATION     -7
/*! coines error code - Feature not supported */
#define COINES_E_NOT_SUPPORTED         -8
/*! coines error code - Null pointer */
#define COINES_E_NULL_PTR              -9
/*! coines error code - Wrong response */
#define COINES_E_COMM_WRONG_RESPONSE   -10
/*! coines error code - Not configured */
#define COINES_E_SPI16BIT_NOT_CONFIGURED -11

/**********************************************************************************/
/* data structure declarations  */
/**********************************************************************************/
/*!
 * @brief communication interface type
 */
enum coines_comm_intf
{
    COINES_COMM_INTF_USB, /*< communication interface USB */
    COINES_COMM_INTF_VCOM, /*< communication interface VCOM */
    COINES_COMM_INTF_BLE /*< communication interface BLE */
};

/*!
 * @brief MULTIO Pin direction
 */
enum coines_pin_direction
{
    COINES_PIN_DIRECTION_IN = 0, /*< PIN direction IN */
    COINES_PIN_DIRECTION_OUT /*< PIN direction OUT */
};

/*!
 * @brief MULTIO Pin value
 */
enum coines_pin_value
{
    COINES_PIN_VALUE_LOW = 0, /*< PIN value LOW */
    COINES_PIN_VALUE_HIGH /*< PIN value HIGH */
};

/*!
 * @brief I2C speed mode settings
 */
enum coines_i2c_mode
{
    COINES_I2C_STANDARD_MODE = 0x00, /*< I2C speed in standard mode */
    COINES_I2C_FAST_MODE = 0x01, /*< I2C speed in fast mode */
    COINES_I2C_SPEED_3_4_MHZ = 0x02, /*< I2C speed in 3.4 MHz */
    COINES_I2C_SPEED_1_7_MHZ = 0x03 /*< I2C speed in 1.7 MHz */
};

/*!
 * @brief COINES sampling unit
 */
enum coines_sampling_unit
{
    COINES_SAMPLING_TIME_IN_MICRO_SEC = 0x01, /*< sampling unit in micro second */
    COINES_SAMPLING_TIME_IN_MILLI_SEC = 0x02 /*< sampling unit in milli second */
};

/*!
 * @brief SPI mode settings
 *
 * > Note - don't change the values
 */
enum coines_spi_speed
{

    COINES_SPI_SPEED_10_MHZ = 6, /*< 10 MHz */
    COINES_SPI_SPEED_7_5_MHZ = 8, /*< 7.5 MHz */
    COINES_SPI_SPEED_6_MHZ = 10, /*< 6 MHz */
    COINES_SPI_SPEED_5_MHZ = 12, /*< 5 MHz */
    COINES_SPI_SPEED_3_75_MHZ = 16, /*< 3.75 MHz */
    COINES_SPI_SPEED_3_MHZ = 20, /*< 3 MHz */
    COINES_SPI_SPEED_2_5_MHZ = 24, /*< 2.5 MHz */
    COINES_SPI_SPEED_2_MHZ = 30, /*< 2 MHz */
    COINES_SPI_SPEED_1_5_MHZ = 40, /*< 1.5 MHz */
    COINES_SPI_SPEED_1_25_MHZ = 48, /*< 1.25 MHz */
    COINES_SPI_SPEED_1_2_MHZ = 50, /*< 1.2 MHz */
    COINES_SPI_SPEED_1_MHZ = 60, /*< 1 MHz */
    COINES_SPI_SPEED_750_KHZ = 80, /*< 750 kHz */
    COINES_SPI_SPEED_600_KHZ = 100, /*< 600 kHz */
    COINES_SPI_SPEED_500_KHZ = 120, /*< 500 kHz */
    COINES_SPI_SPEED_400_KHZ = 150, /*< 400 kHz */
    COINES_SPI_SPEED_300_KHZ = 200, /*< 300 kHz */
    COINES_SPI_SPEED_250_KHZ = 240 /*< 250 kHz */
};

/*!
 *  @brief user configurable Shuttle board pin description
 */
enum coines_multi_io_pin
{
    COINES_SHUTTLE_PIN_7 = 0x09,  /*<  CS pin*/
    COINES_SHUTTLE_PIN_8 = 0x05,  /*<  Multi-IO 5*/
    COINES_SHUTTLE_PIN_9 = 0x00,  /*<  Multi-IO 0*/
    COINES_SHUTTLE_PIN_14 = 0x01, /*<  Multi-IO 1*/
    COINES_SHUTTLE_PIN_15 = 0x02, /*<  Multi-IO 2*/
    COINES_SHUTTLE_PIN_16 = 0x03, /*<  Multi-IO 3*/
    COINES_SHUTTLE_PIN_19 = 0x08, /*<  Multi-IO 8*/
    COINES_SHUTTLE_PIN_20 = 0x06, /*<  Multi-IO 6*/
    COINES_SHUTTLE_PIN_21 = 0x07, /*<  Multi-IO 7*/
    COINES_SHUTTLE_PIN_22 = 0x04, /*<  Multi-IO 4*/

#if defined (PC) || defined(MCU_APP30)
    COINES_MINI_SHUTTLE_PIN_1_4 = 0x10, /*<  GPIO0 */
    COINES_MINI_SHUTTLE_PIN_1_5 = 0x11, /*<  GPIO1 */
    COINES_MINI_SHUTTLE_PIN_1_6 = 0x12, /*<  GPIO2/INT1 */
    COINES_MINI_SHUTTLE_PIN_1_7 = 0x13, /*<  GPIO3/INT2 */
    COINES_MINI_SHUTTLE_PIN_2_5 = 0x14, /*<  GPIO4 */
    COINES_MINI_SHUTTLE_PIN_2_6 = 0x15, /*<  GPIO5 */
    COINES_MINI_SHUTTLE_PIN_2_1 = 0x16, /*<  CS */
    COINES_MINI_SHUTTLE_PIN_2_3 = 0x17, /*<  SDO */
#endif

};

/*!
 * @brief SPI mode settings
 */
enum coines_spi_mode
{
    COINES_SPI_MODE0 = 0x00, /*< SPI Mode 0: CPOL=0; CPHA=0 */
    COINES_SPI_MODE1 = 0x01, /*< SPI Mode 1: CPOL=0; CPHA=1 */
    COINES_SPI_MODE2 = 0x02, /*< SPI Mode 2: CPOL=1; CPHA=0 */
    COINES_SPI_MODE3 = 0x03 /*< SPI Mode 3: CPOL=1; CPHA=1 */
};


/*!
 * @brief SPI Number of bits per transfer
 */
enum coines_spi_transfer_bits
{
    COINES_SPI_TRANSFER_8BIT = 8, /**< Transfer 8 bit */
    /* Before selecting the below 16 bit, ensure that,
     * the intended sensor supports 16bit register read/write
     */
    COINES_SPI_TRANSFER_16BIT = 16 /**< Transfer 16 bit */
};

/*!
 * @brief interface type
 */
enum coines_sensor_intf
{
    COINES_SENSOR_INTF_SPI, /*< SPI Interface */
    COINES_SENSOR_INTF_I2C /*< I2C Interface */
};

/*!
 * @brief i2c bus
 */
enum coines_i2c_bus
{

    COINES_I2C_BUS_0, /*< I2C bus 0 */
    COINES_I2C_BUS_1 /*< I2C bus 1 */
};
/*!
 * @brief spi bus
 */
enum coines_spi_bus
{

    COINES_SPI_BUS_0, /*< SPI bus 0 */
    COINES_SPI_BUS_1 /*< SPI bus 1 */
};
/*!
 * @brief timer configuration
 */
enum coines_timer_config
{
    COINES_TIMER_STOP, /*< TIMER Stop */
    COINES_TIMER_START, /*< TIMER Start */
    COINES_TIMER_RESET /*< TIMER Reset */
};
/*!
 * @brief times stamp config
 */
enum coines_time_stamp_config
{
    COINES_TIMESTAMP_ENABLE = 0x03, /*< TIMESTAMP Enable */
    COINES_TIMESTAMP_DISABLE = 0x04 /*< TIMESTAMP Disable */
};
/*!
 * @brief coines streaming mode
 */
enum coines_streaming_mode
{
    COINES_STREAMING_MODE_POLLING, /*< Polling mode streaming */
    COINES_STREAMING_MODE_INTERRUPT /*< Interrupt mode streaming */
};

/*!
 * @brief Structure to store the board related information
 */
struct coines_board_info
{
    uint16_t hardware_id; /*< Board hardware ID */
    uint16_t software_id; /*< Board software ID */
    uint8_t board; /*< Type of the board like APP2.0, Arduino Due */
    uint16_t shuttle_id; /*< Shuttle ID of the sensor connected */
};
/*!
 * @brief streaming address block
 */
struct coines_streaming_blocks
{

    uint16_t no_of_blocks; /*< Number of blocks */
    uint8_t reg_start_addr[10]; /*< Register start address */
    uint8_t no_of_data_bytes[10]; /*< Number of data bytes */
};

/*!
 * @brief streaming config settings
 */
struct coines_streaming_config
{
    enum coines_sensor_intf intf; /*< Sensor Interface */
    enum coines_i2c_bus i2c_bus; /*< I2C bus */
    enum coines_spi_bus spi_bus; /*< SPI bus */
    uint8_t dev_addr; /*< I2C -Device address */
    uint8_t cs_pin; /*< Chip select */
    uint16_t sampling_time; /*< Sampling time */
    enum coines_sampling_unit sampling_units; /*< micro second / milli second - Sampling unit */
    enum coines_multi_io_pin int_pin; /*< Interrupt pin */
    uint8_t int_timestamp; /*< 1- enable /0- disable time stamp for corresponding sensor */
};
/*!
 * @brief Pin interrupt modes
 */
enum coines_pin_interrupt_mode
{
    COINES_PIN_INTERRUPT_CHANGE, /*< Trigger interrupt on pin state change */
    COINES_PIN_INTERRUPT_RISING_EDGE, /*< Trigger interrupt when pin changes from low to high */
    COINES_PIN_INTERRUPT_FALLING_EDGE /*< Trigger interrupt when pin changes from high to low */
};

/**********************************************************************************/
/* function prototype declarations */
/**********************************************************************************/
/*!
 * @brief This API is used to initialize the communication according to interface type.
 *
 * @param[in] intf_type: Type of interface(USB, COM, or BLE).
 *
 * @return Result of API execution status
 * @retval Zero -> Success
 * @retval Negative -> Error
 */
int16_t coines_open_comm_intf(enum coines_comm_intf intf_type);
/*!
 * @brief This API is used to close the active communication(USB,COM or BLE).
 *
 * @param[in] intf_type: Type of interface(USB, COM, or BLE).
 *
 * @return Result of API execution status
 * @retval Zero -> Success
 * @retval Negative -> Error
 */
int16_t coines_close_comm_intf(enum coines_comm_intf intf_type);

/*!
 *  @brief This API is used to get the board information.
 *
 *  @param[out] data  : Board information details.
 *
 *  @return Result of API execution status.
 *  @retval 0 -> Success.
 *  @retval Any non zero value -> Fail.
 *
 */

int16_t coines_get_board_info(struct coines_board_info *data);
/*!
 *  @brief This API is used to configure the pin(MULTIIO/SPI/I2C in shuttle board).
 *
 *  @param[in] pin_number : pin to be configured.
 *  @param[in] direction : pin direction information(COINES_PIN_DIRECTION_IN or COINES_PIN_DIRECTION_OUT) *
 *  @param[in] pin_value : pin value information(COINES_PIN_VALUE_LOW or COINES_PIN_VALUE_HIGH)
 *
 *  @return Results of API execution status.
 *  @retval 0 -> Success
 *  @retval Any non zero value -> Fail
 */
int16_t coines_set_pin_config(enum coines_multi_io_pin pin_number,
                              enum coines_pin_direction direction,
                              enum coines_pin_value pin_value);
/*!
 *  @brief This API function is used to get the pin direction and pin state.
 *
 *  @param[in] pin_number : pin number for getting the status.
 *  @param[out] pin_direction : pin direction information(COINES_PIN_DIRECTION_IN or COINES_PIN_DIRECTION_OUT)
 *  @param[out] pin_value : pin value information(COINES_PIN_VALUE_LOW or COINES_PIN_VALUE_HIGH)*
 *
 *  @return Results of API execution status.
 *  @retval 0 -> Success
 *  @retval Any non zero value -> Fail
 */
int16_t coines_get_pin_config(enum coines_multi_io_pin pin_number,
                              enum coines_pin_direction *pin_direction,
                              enum coines_pin_value *pin_value);
/*!
 *  @brief This API is used to configure the VDD and VDDIO of the sensor.
 *
 *  @param[in] vdd_millivolt     : VDD voltage to be set in sensor.
 *  @param[in] vddio_millivolt   : VDDIO voltage to be set in sensor.
 *
 *  @note In APP2.0 board, voltage level of 0 or 3300mV is supported.
 *        In APP3.0 board, voltage levels of 0, 1800mV and 2800mV are supported.
 *
 *  @return Results of API execution status.
 *  @retval 0 -> Success
 *  @retval Any non zero value -> Fail
 */
int16_t coines_set_shuttleboard_vdd_vddio_config(uint16_t vdd_millivolt, uint16_t vddio_millivolt);
/*!
 *  @brief This API is used to configure the SPI bus
 *
 *  @param[in] bus         : bus
 *  @param[in] spi_speed   : SPI speed
 *  @param[in] spi_mode    : SPI mode
 *
 *  @return Results of API execution status.
 *  @retval 0 -> Success
 *  @retval Any non zero value -> Fail
 */
int16_t coines_config_spi_bus(enum coines_spi_bus bus, enum coines_spi_speed spi_speed, enum coines_spi_mode spi_mode);
/*!
 *  @brief This API is used to configure the SPI bus as either 8 bit or 16 bit length
 *
 *  @param[in] bus         : bus
 *  @param[in] spi_speed   : spi speed
 *  @param[in] spi_mode    : spi mode
 *  @param[in] spi_transfer_bits    : bits to transfer
 *  @return Results of API execution status.
 *  @retval 0 -> Success
 *  @retval Any non zero value -> Fail
 */
int16_t coines_config_word_spi_bus(enum coines_spi_bus bus,
                                   enum coines_spi_speed spi_speed,
                                   enum coines_spi_mode spi_mode,
                                   enum coines_spi_transfer_bits spi_transfer_bits);
/*!
 *  @brief This API is used to configure the I2C bus
 *
 *  @param[in] bus : i2c bus
 *  @param[in] i2c_mode   : i2c_mode
 *
 *  @return Results of API execution status.
 *  @retval 0 -> Success
 *  @retval Any non zero value -> Fail
 */
int16_t coines_config_i2c_bus(enum coines_i2c_bus bus, enum coines_i2c_mode i2c_mode);
/*!
 *  @brief This API is used to write 8-bit register data on the I2C device.
 *
 *  @param[in] dev_addr : Device address for I2C write.
 *  @param[in] reg_addr : Starting address for writing the data.
 *  @param[in] reg_data : Data to be written.
 *  @param[in] count    : Number of bytes to write.
 *
 *  @return Results of API execution status.
 *  @retval 0 -> Success
 *  @retval Any non zero value -> Fail
 *
 */
int8_t coines_write_i2c(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t count);
/*!
 *  @brief This API is used to read 8-bit register data from the I2C device.
 *
 *  @param[in] dev_addr  : Device address for I2C read.
 *  @param[in] reg_addr  : Starting address for reading the data.
 *  @param[out] reg_data : Data read from the sensor.
 *  @param[in] count     : Number of bytes to read.
 *
 *  @return Results of API execution status.
 *  @retval 0 -> Success
 *  @retval Any non zero value -> Fail
 *
 */
int8_t coines_read_i2c(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t count);
/*!
 *  @brief This API is used to write 16-bit register data on the SPI device.
 *
 *  @param[in] cs       : Chip select pin number for SPI write.
 *  @param[in] reg_addr : Starting address for writing the data.
 *  @param[in] reg_data : Data to be written.
 *  @param[in] count    : Number of words to write.
 *
 *  @return Results of API execution status.
 *  @retval 0 -> Success
 *  @retval Any non zero value -> Fail
 *
 */
int8_t coines_write_16bit_spi(uint8_t cs, uint16_t reg_addr, uint16_t *reg_data, uint16_t count);
/*!
 *  @brief This API is used to write 8-bit register data on the SPI device.
 *
 *  @param[in] dev_addr : Chip select pin number for SPI write.
 *  @param[in] reg_addr : Starting address for writing the data.
 *  @param[in] reg_data : Data to be written.
 *  @param[in] count    : Number of bytes to write.
 *
 *  @return Results of API execution status.
 *  @retval 0 -> Success
 *  @retval Any non zero value -> Fail
 *
 */
int8_t coines_write_spi(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t count);
/*!
 *  @brief This API is used to read 16-bit register data from the SPI device.
 *
 *  @param[in] cs        : Chip select pin number for SPI read.
 *  @param[in] reg_addr  : Starting address for reading the data.
 *  @param[out] reg_data : Data read from the sensor.
 *  @param[in] count     : Number of words(2 byte) to read.
 *
 *  @return Results of API execution status.
 *  @retval 0 -> Success
 *  @retval Any non zero value -> Fail
 *
 */
int8_t coines_read_16bit_spi(uint8_t cs, uint16_t reg_addr, uint16_t *reg_data, uint16_t count);
/*!
 *  @brief This API is used to read the data in SPI communication.
 *
 *  @param[in] dev_addr : Chip select pin number for SPI read.
 *  @param[in] reg_addr : Starting address for reading the data.
 *  @param[out] reg_data : Data read from the sensor.
 *  @param[in] count    : Number of bytes to read.
 *
 *  @return Results of API execution status.
 *  @retval 0 -> Success
 *  @retval Any non zero value -> Fail
 *
 */
int8_t coines_read_spi(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t count);
/*!
 *  @brief This API is used for introducing a delay in milliseconds
 *
 *  @param[in] delay_ms   :  delay in milliseconds.
 *
 *  @return void
 */
void coines_delay_msec(uint32_t delay_ms);
/*!
 *  @brief This API is used for introducing a delay in microseconds
 *
 *  @param[in] delay_us   :  delay in microseconds.
 *
 *  @return void
 */
void coines_delay_usec(uint32_t delay_us);
/*!
 * @brief This API is used to send the streaming settings to the board.
 *
 * @param[in] channel_id    :  channel identifier (Possible values - 1,2)
 * @param[in] stream_config :  stream_config
 * @param[in] data_blocks   :  data_blocks
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval Any non zero value -> Fail
 */
int16_t coines_config_streaming(uint8_t channel_id,
                                struct coines_streaming_config *stream_config,
                                struct coines_streaming_blocks *data_blocks);
/*!
 * @brief This API is used to start or stop the streaming.
 *
 * @param[in] stream_mode :  stream_mode
 * @param[in] samples     :  samples
 * @param[in] start_stop  :  start or stop steaming
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval Any non zero value -> Fail
 */
int16_t coines_start_stop_streaming(enum coines_streaming_mode stream_mode, uint8_t start_stop);
/*!
 * @brief This API is used to read the streaming sensor data.
 *
 * @param[in] sensor_id             :  Sensor Identifier.
 * @param[in] number_of_samples     :  Number of samples to be read.
 * @param[out] data                 :  Buffer to retrieve the sensor data.
 * @param[out] valid_samples_count  :  Count of valid samples available.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval Any non zero value -> Fail
 */
int16_t coines_read_stream_sensor_data(uint8_t sensor_id,
                                       uint32_t number_of_samples,
                                       uint8_t *data,
                                       uint32_t *valid_samples_count);
/*!
 * @brief This API is used to trigger the timer in firmware and enable or disable system time stamp
 *
 * @param[in] tmr_cfg : timer config value
 * @param[in] ts_cfg : timer stamp cfg value
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval Any non zero value -> Fail
 */
int16_t coines_trigger_timer(enum coines_timer_config tmr_cfg, enum coines_time_stamp_config ts_cfg);
/*!
 * @brief This API returns the number of milliseconds passed since the program started
 *
 * @return Time in milliseconds
 */
uint32_t coines_get_millis();
/*!
 * @brief Attaches a interrupt to a Multi-IO pin
 *
 * @param[in] pin_number : Multi-IO pin
 * @param[in] callback : Name of the function to be called on detection of interrupt
 * @param[in] mode : Trigger modes - change,rising edge,falling edge
 *
 * @return void
 */
void coines_attach_interrupt(enum coines_multi_io_pin pin_number,
                             void (*callback)(void),
                             enum coines_pin_interrupt_mode int_mode);
/*!
 * @brief Detaches a interrupt from a Multi-IO pin
 *
 * @param[in] pin_number : Multi-IO pin
 *
 * @return void
 */
void coines_detach_interrupt(enum coines_multi_io_pin pin_number);

#ifdef __cplusplus
}
#endif

#endif /* COINES_H_ */

/** @}*/

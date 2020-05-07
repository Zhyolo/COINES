/**
 * Copyright (C) 2018 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file   coines.c
 * @brief  This module defines coines APIs to be used by the user applications
 *
 */

/*!
 * @defgroup coines_api coines
 * @{*/

/*! \mainpage Introduction
 *
 * - COINES provides a low-level interface to Bosch Sensortec's application board APP2.0.
 * - The user can access Bosch Sensortec's MEMS sensors using the SensorAPI through a C
 *   and a python interface, compile, modify and test sample applications.
 * - COINES can be used to see how to use the SensorAPI in an embedded environment,
 *   but also allows convenient data logging.
 *
 */
/*********************************************************************/
/* system header files */
/*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
/*********************************************************************/
/* own header files */
/**********************************************************************/
#include "coines.h"
#include "coines_defs.h"
#include "comm_intf.h"
#if defined (ZEUS_QUIRK)
#include "zeus.h"
#endif
/*********************************************************************/
/* global variables */
/*********************************************************************/
/*! Structure to hold the non streaming response data */
coines_rsp_buffer_t coines_rsp_buf;
/*! Structure to hold the stream response data*/
coines_stream_rsp_buffer_t coines_stream_rsp_buf;
/*! Variable to hold the board type*/
coines_board_t coines_board;

/*! Flag for SPI 16 bit.
 * if 1 -> SPI 16 bit is configured
 * else SPI 16 bit is not configured */
uint8_t spi_16bit_enable = 0;
/*!
 * @brief global structure to hold the streaming config data
 */
struct coines_streaming_settings
{
    uint8_t channel_id; /*< streaming channel 1/2 */
    struct coines_streaming_config stream_config; /*< streaming config */
    struct coines_streaming_blocks data_blocks; /*< streaming data blocks */
};

comm_stream_info_t comm_intf_sensor_info;
/*! variable to hold the maximum no of streaming configuration buffer*/
struct coines_streaming_settings coines_streaming_cfg_buf[COINES_MAX_SENSOR_COUNT];

/*********************************************************************/
/* static variables */
/*! Variable to hold sensor count */
static uint8_t coines_sensor_id_count = 0;

/*********************************************************************/
/* static function declarations */
/*********************************************************************/

/*! coines data read */
static int16_t coines_read(enum coines_sensor_intf intf,
                           uint8_t cs_pin,
                           uint8_t dev_addr,
                           uint8_t reg_addr,
                           uint8_t *reg_data,
                           uint16_t count);
/*! coines data write */
static int16_t coines_write(enum coines_sensor_intf intf,
                            uint8_t cs_pin,
                            uint8_t dev_addr,
                            uint8_t reg_addr,
                            uint8_t *reg_data,
                            uint16_t count);
/*! coines 16bit data read */
static int16_t coines_read_16bit(uint8_t cs_pin, uint16_t reg_addr, uint16_t *reg_data, uint16_t count);

/*! coines 16bit data write */
static int16_t coines_write_16bit(uint8_t cs_pin, uint16_t reg_addr, uint16_t *reg_data, uint16_t word_count);

/*********************************************************************/
/* functions */
/*********************************************************************/
/*!
 * @brief This API is used to initialize the communication according to interface type.
 */
int16_t coines_open_comm_intf(enum coines_comm_intf intf_type)
{
    int16_t rslt;
    rslt = comm_intf_open(intf_type, &coines_board);
    if (rslt == COINES_SUCCESS)
    {
        coines_start_stop_streaming(COINES_STREAMING_MODE_INTERRUPT, COINES_STREAMING_STOP);
        coines_delay_msec(150);
        coines_trigger_timer(COINES_TIMER_STOP, COINES_TIMESTAMP_DISABLE);
        coines_delay_msec(150);
        coines_start_stop_streaming(COINES_STREAMING_MODE_POLLING, COINES_STREAMING_STOP);
        coines_delay_msec(150);
    }
    return rslt;

}
/*********************************************************************/
/*!
 * @brief This API is used to close the active communication(USB,COM or BLE).
 */
int16_t coines_close_comm_intf(enum coines_comm_intf intf_type)
{
    comm_intf_close(intf_type);
    return COINES_SUCCESS;
}

/*********************************************************************/
/*!
 *  @brief This API is used to get the board information.
 *
 */
int16_t coines_get_board_info(struct coines_board_info *data)
{
    int16_t rslt;

    if (data == NULL)
        return COINES_E_NULL_PTR;

    comm_intf_init_command_header(COINES_DD_GET, COINES_CMDID_BOARDINFORMATION);
    rslt = comm_intf_send_command(&coines_rsp_buf);
    if (rslt == COINES_SUCCESS)
    {
        data->shuttle_id = (coines_rsp_buf.buffer[6] << 8) | coines_rsp_buf.buffer[7];
        data->hardware_id = (coines_rsp_buf.buffer[8] << 8) | coines_rsp_buf.buffer[9];
        data->software_id = (coines_rsp_buf.buffer[10] << 8) | coines_rsp_buf.buffer[11];
        data->board = coines_rsp_buf.buffer[12];
    }

    return rslt;
}
/*********************************************************************/
/*!
 *  @brief This API is used to configure the pin(MULTIIO/SPI/I2C in shuttle board).
 *
 */
int16_t coines_set_pin_config(enum coines_multi_io_pin pin_number,
                              enum coines_pin_direction direction,
                              enum coines_pin_value pin_value)
{
    uint16_t pin_number_value;

    memset(coines_rsp_buf.buffer, 0, COINES_DATA_BUF_SIZE);
    comm_intf_init_command_header(COINES_DD_SET, COINES_CMDID_MULTIO_CONFIGURATION);
    if (pin_number < COINES_MINI_SHUTTLE_PIN_1_4)
    {
        pin_number_value = (1 << pin_number);
        comm_intf_put_u16(pin_number_value);
        comm_intf_put_u16(direction ? pin_number_value : 0);
        comm_intf_put_u16(pin_value ? pin_number_value : 0);
    }
    else /* APP3.0 shuttle pin */
    {
        comm_intf_put_u16(COINES_MINI_SHUTTLE_PIN_ID | pin_number);
        comm_intf_put_u16(direction);
        comm_intf_put_u16(pin_value);
    }

    return comm_intf_send_command(&coines_rsp_buf);
}
/*********************************************************************/
/*!
 *  @brief This API function is used to get the pin direction and pin state.
 *         Either 'pin_direction' (or) 'pin_value' parameter can be NULL but not both at the same time.
 */
int16_t coines_get_pin_config(enum coines_multi_io_pin pin_number,
                              enum coines_pin_direction *pin_direction,
                              enum coines_pin_value *pin_value)
{
    int16_t rslt;
    uint16_t pin_number_value = (1 << pin_number); /* pin_direction_response, pin_value_response; */

    if ((pin_value != NULL) || (pin_direction != NULL))
    {
        memset(coines_rsp_buf.buffer, 0, COINES_DATA_BUF_SIZE);
        comm_intf_init_command_header(COINES_DD_GET, COINES_CMDID_MULTIO_CONFIGURATION);

        if (pin_number < COINES_MINI_SHUTTLE_PIN_1_4)
            comm_intf_put_u16(pin_number_value);
        else  /* APP3.0 shuttle pin */
            comm_intf_put_u16(COINES_MINI_SHUTTLE_PIN_ID | pin_number);

        rslt = comm_intf_send_command(&coines_rsp_buf);

        if (rslt == COINES_SUCCESS)
        {
            if (pin_direction != NULL)
            {
                if (pin_number < COINES_MINI_SHUTTLE_PIN_1_4)
                {
                    /* Direction available at eight position */
                    *pin_direction = (enum coines_pin_direction)((coines_rsp_buf.buffer[8] << 8) | coines_rsp_buf.buffer[9]);
                    *pin_direction = (*pin_direction == pin_number_value) ? COINES_PIN_DIRECTION_OUT : COINES_PIN_DIRECTION_IN;
                }
                else  /* APP3.0 shuttle pin */
                {
                    *pin_direction = (enum coines_pin_direction)((coines_rsp_buf.buffer[8] << 8) | coines_rsp_buf.buffer[9]) ? \
                            COINES_PIN_DIRECTION_OUT : COINES_PIN_DIRECTION_IN;
                }
            }
            if (pin_value != NULL)
            {
                if (pin_number < COINES_MINI_SHUTTLE_PIN_1_4)
                {
                    /* Pin value available at tenth position */
                    *pin_value = (enum coines_pin_value)((coines_rsp_buf.buffer[10] << 8) | coines_rsp_buf.buffer[11]);
                    *pin_value = (*pin_value == pin_number_value) ? COINES_PIN_VALUE_HIGH : COINES_PIN_VALUE_LOW;
                }
                else  /* APP3.0 shuttle pin */
                {
                    *pin_value = (enum coines_pin_value)((coines_rsp_buf.buffer[10] << 8) | coines_rsp_buf.buffer[11]) ? \
                            COINES_PIN_VALUE_HIGH : COINES_PIN_VALUE_LOW;
                }
            }
        }
    }
    else
    {
        rslt = COINES_E_NULL_PTR;
    }

    return rslt;
}
/*********************************************************************/
/*!
 *  @brief This API is used to configure the VDD and VDDIO of the sensor.
 */
int16_t coines_set_shuttleboard_vdd_vddio_config(uint16_t vdd_millivolt, uint16_t vddio_millivolt)
{
    if ((vdd_millivolt > 3600) || (vddio_millivolt > 3600))
        return COINES_E_NOT_SUPPORTED;

    memset(coines_rsp_buf.buffer, 0, COINES_DATA_BUF_SIZE);
    comm_intf_init_command_header(COINES_DD_SET, COINES_CMDID_SHUTTLEBOARD_VDD_VDDIO_CONFIGURATION);
    comm_intf_put_u16(vdd_millivolt);
    comm_intf_put_u8(vdd_millivolt ? 1 : 0);
    comm_intf_put_u16(vddio_millivolt);
    comm_intf_put_u8(vddio_millivolt ? 1 : 0);
    return comm_intf_send_command(&coines_rsp_buf);
}
/*********************************************************************/
/*!
 *  @brief This API is used to configure the spi bus
 *
 */
int16_t coines_config_spi_bus(enum coines_spi_bus bus, enum coines_spi_speed spi_speed, enum coines_spi_mode spi_mode)
{
    int16_t rslt;

    memset(coines_rsp_buf.buffer, 0, COINES_DATA_BUF_SIZE);
    comm_intf_init_command_header(COINES_DD_SET, COINES_CMDID_INTERFACE);

    comm_intf_put_u8(COINES_SENSOR_INTF_SPI);
    comm_intf_put_u8(COINES_INTF_SDO_LOW); /*sdo low/high */
    rslt = comm_intf_send_command(&coines_rsp_buf);

    if (rslt == COINES_SUCCESS)
    {
        memset(coines_rsp_buf.buffer, 0, COINES_DATA_BUF_SIZE);
        comm_intf_init_command_header(COINES_DD_SET, COINES_CMDID_SPISETTINGS);
        comm_intf_put_u8(bus);
        comm_intf_put_u8(spi_mode);
        comm_intf_put_u8(8); /*8bit */
        comm_intf_put_u8(spi_speed);
        rslt = comm_intf_send_command(&coines_rsp_buf);
    }

    /* Disable SPI 16bit config*/
    if (spi_16bit_enable)
    {
        spi_16bit_enable = 0;
    }

    return rslt;
}


/*!
 *  @brief This API is used to configure the spi bus with 8 bit or 16 bit length
 *
 */
int16_t coines_config_word_spi_bus(enum coines_spi_bus bus,
                                   enum coines_spi_speed spi_speed,
                                   enum coines_spi_mode spi_mode,
                                   enum coines_spi_transfer_bits spi_transfer_bits)
{
    int16_t rslt;

    memset(coines_rsp_buf.buffer, 0, COINES_DATA_BUF_SIZE);
    comm_intf_init_command_header(COINES_DD_SET, COINES_CMDID_INTERFACE);

    comm_intf_put_u8(COINES_SENSOR_INTF_SPI);
    comm_intf_put_u8(COINES_INTF_SDO_LOW); /*sdo low/high */
    rslt = comm_intf_send_command(&coines_rsp_buf);

    if (rslt == COINES_SUCCESS)
    {
        memset(coines_rsp_buf.buffer, 0, COINES_DATA_BUF_SIZE);
        comm_intf_init_command_header(COINES_DD_SET, COINES_CMDID_SPISETTINGS);
        comm_intf_put_u8(bus);
        comm_intf_put_u8(spi_mode);
        if (COINES_SPI_TRANSFER_16BIT == spi_transfer_bits)
        {
            spi_16bit_enable = 1;
            comm_intf_put_u8(spi_transfer_bits);
        }
        else
        {
            spi_16bit_enable = 0;
        }
        comm_intf_put_u8(spi_speed);
        rslt = comm_intf_send_command(&coines_rsp_buf);
    }

    return rslt;
}
/*********************************************************************/
/*!
 *  @brief This API is used to configure the i2c bus
 */
int16_t coines_config_i2c_bus(enum coines_i2c_bus bus, enum coines_i2c_mode i2c_mode)
{
    int16_t rslt;
    memset(coines_rsp_buf.buffer, 0, COINES_DATA_BUF_SIZE);
    comm_intf_init_command_header(COINES_DD_SET, COINES_CMDID_INTERFACE);

    comm_intf_put_u8(COINES_SENSOR_INTF_I2C);
    comm_intf_put_u8(COINES_INTF_SDO_LOW); /*sdo low/high */
    rslt = comm_intf_send_command(&coines_rsp_buf);

    if (rslt == COINES_SUCCESS)
    {
        memset(coines_rsp_buf.buffer, 0, COINES_DATA_BUF_SIZE);
        comm_intf_init_command_header(COINES_DD_SET, COINES_CMDID_I2CSPEED);
        comm_intf_put_u8(bus);
        comm_intf_put_u8(i2c_mode);
        rslt = comm_intf_send_command(&coines_rsp_buf);
    }

    return rslt;
}
/*********************************************************************/
/*!
 *  @brief This API is used to write the data in I2C communication.
 */
int8_t coines_write_i2c(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t count)
{
    return (int8_t)coines_write(COINES_SENSOR_INTF_I2C, 0, dev_addr, reg_addr, reg_data, count);
}
/*********************************************************************/
/*!
 *  @brief This API is used to read the data in I2C communication.
 */
int8_t coines_read_i2c(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t count)
{
    return (int8_t)coines_read(COINES_SENSOR_INTF_I2C, 0, dev_addr, reg_addr, reg_data, count);
}

/*********************************************************************/
/*!
 *  @brief This API is used to write the data in SPI communication.
 *
 */
int8_t coines_write_spi(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t count)
{
    return (int8_t)coines_write(COINES_SENSOR_INTF_SPI, dev_addr, 0, reg_addr, reg_data, count);
}

/*!
 *  @brief This API is used to write the 16bit data(word per transfer) in SPI communication.
 *
 */
int8_t coines_write_16bit_spi(uint8_t cs, uint16_t reg_addr, uint16_t *reg_data, uint16_t count)
{
    return (int8_t)coines_write_16bit(cs, reg_addr, reg_data, count);
}

/*********************************************************************/
/*!
 *  @brief This API is used to read the data in SPI communication.
 *
 */
int8_t coines_read_spi(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t count)
{
    return (int8_t)coines_read(COINES_SENSOR_INTF_SPI, dev_addr, 0, reg_addr, reg_data, count);
}

/*!
 *  @brief This API is used to read the 16 bit data(word per transfer) in SPI communication.
 *
 */
int8_t coines_read_16bit_spi(uint8_t cs, uint16_t reg_addr, uint16_t *reg_data, uint16_t count)
{
    return (int8_t)coines_read_16bit(cs, reg_addr, reg_data, count);
}

/*********************************************************************/
/*!
 *  @brief This API is used for introducing a delay in milliseconds
 */
void coines_delay_msec(uint32_t delay_ms)
{
    comm_intf_delay(delay_ms);
}

/*********************************************************************/
/*!
 *  @brief This API is used for introducing a delay in microseconds
 */

void coines_delay_usec(uint32_t delay_us)
{
    if (delay_us >= 1000)
        coines_delay_msec(delay_us/1000);
    else
        coines_delay_msec(1);
}

/*********************************************************************/
/*!
 * @brief This API is used to send the streaming settings to the board.
 */
int16_t coines_config_streaming(uint8_t channel_id,
                                struct coines_streaming_config *stream_config,
                                struct coines_streaming_blocks *data_blocks)
{
    int16_t rslt = COINES_SUCCESS;
    if ((stream_config != NULL) && (data_blocks != NULL))
    {
        if (coines_sensor_id_count > COINES_MAX_SENSOR_ID)
            return COINES_E_FAILURE;

        coines_streaming_cfg_buf[coines_sensor_id_count].channel_id = channel_id;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.intf = stream_config->intf;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.i2c_bus = stream_config->i2c_bus;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.spi_bus = stream_config->spi_bus;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.dev_addr = stream_config->dev_addr;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.cs_pin = stream_config->cs_pin;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.sampling_time = stream_config->sampling_time;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.sampling_units = stream_config->sampling_units;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.int_pin = stream_config->int_pin;
        coines_streaming_cfg_buf[coines_sensor_id_count].stream_config.int_timestamp = stream_config->int_timestamp;
        coines_streaming_cfg_buf[coines_sensor_id_count].data_blocks.no_of_blocks = data_blocks->no_of_blocks;
        memcpy(coines_streaming_cfg_buf[coines_sensor_id_count].data_blocks.reg_start_addr,
               data_blocks->reg_start_addr,
               10);
        memcpy(coines_streaming_cfg_buf[coines_sensor_id_count].data_blocks.no_of_data_bytes,
               data_blocks->no_of_data_bytes,
               10);
        coines_sensor_id_count++;
    }
    else
    {
        rslt = COINES_E_NULL_PTR;
    }
    return rslt;
}

/*********************************************************************/
/*!
 * @brief This API is used to start or stop the streaming.
 */
int16_t coines_start_stop_streaming(enum coines_streaming_mode stream_mode, uint8_t start_stop)
{
    int16_t rslt = COINES_SUCCESS;
    double sampling_time[2];
    double remaining = 0;
    uint8_t sampling_unit[2];
    uint16_t gcd_sampling_time;
    enum coines_sampling_unit gcd_sampling_unit;
    uint32_t i, index;
    uint16_t no_of_bytes_read = 0;
    uint8_t samples;
    /*check the if it is start request for polling streaming*/
    if (start_stop)
    {
        samples = COINES_STREAM_INFINITE_SAMPLES;
        comm_intf_sensor_info.no_of_sensors_enabled = coines_sensor_id_count;
        if (stream_mode == COINES_STREAMING_MODE_POLLING)
        {
            /*check if sensor id count is greater than 1*/
            if (coines_sensor_id_count > 1)
            {
                for (i = 0; i < coines_sensor_id_count; i++)
                {
                    sampling_time[i] = (double)coines_streaming_cfg_buf[i].stream_config.sampling_time;
                    sampling_unit[i] = coines_streaming_cfg_buf[i].stream_config.sampling_units;
                    sampling_time[i] =
                        (sampling_unit[i] ==
                         COINES_SAMPLING_TIME_IN_MICRO_SEC) ? (sampling_time[i] / 1000.00) : sampling_time[i];
                }
                /* Calculate GCD */
                while (sampling_time[1] != 0)
                {
                    remaining = (double)fmod(sampling_time[0], sampling_time[1]);
                    sampling_time[0] = sampling_time[1];
                    sampling_time[1] = remaining;
                }
                /* If decimal point is present, convert to microsecond */
                if ((sampling_time[0] - (int32_t)sampling_time[0]) != 0)
                {
                    /* Need to convert to microsecond */
                    gcd_sampling_time = (uint16_t)(sampling_time[0] * 1000);
                    gcd_sampling_unit = COINES_SAMPLING_TIME_IN_MICRO_SEC;
                }
                else
                {
                    gcd_sampling_time = (uint16_t)sampling_time[0];
                    gcd_sampling_unit = COINES_SAMPLING_TIME_IN_MILLI_SEC;
                }
            }
            else
            {
                gcd_sampling_time = coines_streaming_cfg_buf[coines_sensor_id_count - 1].stream_config.sampling_time;
                gcd_sampling_unit = coines_streaming_cfg_buf[coines_sensor_id_count - 1].stream_config.sampling_units;
            }
            /*general streaming settings*/
            comm_intf_init_command_header(COINES_DD_GENERAL_STREAMING_SETTINGS, coines_sensor_id_count);
            comm_intf_put_u8(1); /*data packet fixed packet count */
            comm_intf_put_u16(gcd_sampling_time); /*sampling time */
            comm_intf_put_u8(gcd_sampling_unit); /*sampling unit */
            rslt = comm_intf_send_command(&coines_rsp_buf);
        }

        /*dd streaming settings*/
        for (i = 0; i < coines_sensor_id_count; i++)
        {
            /* select interface */
            if (rslt == COINES_SUCCESS)
            {
                uint8_t interface_sel;
                if (coines_streaming_cfg_buf[i].stream_config.intf == COINES_SENSOR_INTF_I2C)
                {
                    interface_sel = 0; /*interface I2C 0 */
                }
                else /* if (coines_streaming_cfg_buf[i].stream_config.intf == COINES_SENSOR_INTF_SPI) */
                {
                    if (coines_streaming_cfg_buf[i].stream_config.cs_pin < COINES_MINI_SHUTTLE_PIN_1_4)
                    {
                        /* update the CS pin as defined by the following values in DD2.0 protocol command */
                        /*  0 -> I2C  1 -> SPI(CS_SENSOR)  2 -> SPI(CS_MULTIO_0)  3 -> SPI(CS_MULTIO_1)
                         * 4 -> SPI(CS_MULTIO_2) 5 -> SPI(CS_MULTIO_3)  6 -> SPI(CS_MULTIO_4)  7 -> SPI(CS_MULTIO_5)
                         * 8 -> SPI(CS_MULTIO_6) 9 -> SPI(CS_MULTIO_7)  10 -> SPI(CS_MULTIO_8) */
                        if (coines_streaming_cfg_buf[i].stream_config.cs_pin <= 8)
                        {
                            interface_sel = coines_streaming_cfg_buf[i].stream_config.cs_pin + 2;
                        }
                        else
                        {
                            /* On default select the CS_SENSOR which is 7th pin in the shuttle board*/
                            interface_sel = 1;
                        }
                    }
                    else /* APP3.0 shuttle pin */
                    {
                        interface_sel = coines_streaming_cfg_buf[i].stream_config.cs_pin;
                    }
                }

                if (stream_mode == COINES_STREAMING_MODE_POLLING)
                {
                    comm_intf_init_command_header(COINES_CMDIDEXT_STREAM_POLLING,
                                                  coines_streaming_cfg_buf[i].channel_id);
                    comm_intf_put_u8(0);
                    comm_intf_put_u8(interface_sel);
                    comm_intf_put_u8(1); /*Analog switch set to 1 */
                    comm_intf_put_u16(coines_streaming_cfg_buf[i].stream_config.dev_addr);
                    comm_intf_put_u16(coines_streaming_cfg_buf[i].stream_config.sampling_time);
                    comm_intf_put_u8(coines_streaming_cfg_buf[i].stream_config.sampling_units);
                }
                else /* if (stream_mode == COINES_STREAMING_MODE_INTERRUPT) */
                {
                    comm_intf_init_command_header(COINES_CMDIDEXT_STREAM_INT, coines_streaming_cfg_buf[i].channel_id);
                    comm_intf_put_u8(coines_streaming_cfg_buf[i].stream_config.int_timestamp);
                    comm_intf_put_u8(interface_sel);
                    comm_intf_put_u8(coines_streaming_cfg_buf[i].stream_config.int_pin);
                    comm_intf_put_u16(coines_streaming_cfg_buf[i].stream_config.dev_addr);
                }

                comm_intf_put_u8(1); /*read mode 1 n chunks */
                comm_intf_put_u8(coines_streaming_cfg_buf[i].data_blocks.no_of_blocks);
                for (index = 0; index < coines_streaming_cfg_buf[i].data_blocks.no_of_blocks; index++)
                {
                    no_of_bytes_read += coines_streaming_cfg_buf[i].data_blocks.no_of_data_bytes[index];
                    comm_intf_put_u8(coines_streaming_cfg_buf[i].data_blocks.reg_start_addr[index]);
                    comm_intf_put_u16(coines_streaming_cfg_buf[i].data_blocks.no_of_data_bytes[index]);
                }

                if (stream_mode == COINES_STREAMING_MODE_INTERRUPT)
                {
                    no_of_bytes_read += 4; /* for packet i */
                    if (coines_streaming_cfg_buf[i].stream_config.int_timestamp)
                    {
                        no_of_bytes_read += 6; /* for packet i */
                    }

                    comm_intf_put_u16(COINES_INTERRUPT_TIMEOUT); /* timeout */

                    comm_intf_put_u8(0); /* no of interrupt lines 1/2 */
                    comm_intf_put_u8(0);
                    comm_intf_put_u8(0);

                    comm_intf_put_u16(0); /* delay */
                }
                else /* if (stream_mode == COINES_STREAMING_MODE_POLLING) */
                {
                    comm_intf_put_u32(0);
                }

                rslt = comm_intf_send_command(&coines_rsp_buf);

                comm_intf_sensor_info.sensors_byte_count[i] = no_of_bytes_read;
                no_of_bytes_read = 0;
            }
        }
    }
    else
    {
        samples = 0;
    }

    if (rslt == COINES_SUCCESS)
    {
        if (stream_mode == COINES_STREAMING_MODE_POLLING)
        {
            comm_intf_init_command_header(COINES_CMDIDEXT_STARTSTOP_STREAM_POLLING, samples);
        }
        else /* if (stream_mode == COINES_STREAMING_MODE_INTERRUPT) */
        {
            comm_intf_init_command_header(COINES_CMDIDEXT_STARTSTOP_STREAM_INT, samples);
        }

        rslt = comm_intf_send_command(&coines_rsp_buf);
        if (rslt == COINES_SUCCESS)
        {
            rslt = comm_intf_start_stop_streaming(start_stop, &comm_intf_sensor_info);
        }
    }

    return rslt;
}

/*********************************************************************/

/*!
 * @brief This API is used to read the streaming sensor data.
 */
int16_t coines_read_stream_sensor_data(uint8_t sensor_id,
                                       uint32_t number_of_samples,
                                       uint8_t *data,
                                       uint32_t *valid_samples_count)
{
    int16_t rslt;

    if ((data == NULL) || (valid_samples_count == NULL))
        return COINES_E_NULL_PTR;

    coines_rsp_buf.buffer_size = 0;
    memset(coines_rsp_buf.buffer, 0, COINES_DATA_BUF_SIZE);
    rslt = comm_intf_process_stream_response(sensor_id, number_of_samples, &coines_stream_rsp_buf);
    if (rslt == COINES_SUCCESS && (coines_stream_rsp_buf.buffer_size > 0))
    {
        *valid_samples_count = coines_stream_rsp_buf.buffer_size /
                               comm_intf_sensor_info.sensors_byte_count[sensor_id - 1];
        memcpy(data, coines_stream_rsp_buf.buffer, coines_stream_rsp_buf.buffer_size);
        DEBUG_PRINT("coines_read_stream_sensor_data SUCCESFUL! sample_count: %d  bufsize: %d\n",
                    *valid_samples_count,
                    coines_stream_rsp_buf.buffer_size);
    }
    else
    {
        rslt = COINES_E_FAILURE;
        DEBUG_PRINT("coines_read_stream_sensor_data FAIL!\n");
    }

    return rslt;
}

/*********************************************************************/
/*!
 * @brief This API is used to trigger the timer in firmware and enable or disable the time stamp feature
 */
int16_t coines_trigger_timer(enum coines_timer_config tmr_cfg, enum coines_time_stamp_config ts_cfg)
{
    int16_t rslt = COINES_SUCCESS;

    memset(coines_rsp_buf.buffer, 0, COINES_DATA_BUF_SIZE);
    comm_intf_init_command_header(COINES_DD_SET, COINES_CMDID_TIMER_CFG_CMD_ID);

    comm_intf_put_u8(tmr_cfg);
    rslt = comm_intf_send_command(&coines_rsp_buf);

    /* ???????????????????????? why twice */

    if (rslt == COINES_SUCCESS)
    {
        comm_intf_init_command_header(COINES_DD_GET, COINES_CMDID_TIMER_CFG_CMD_ID);
        comm_intf_put_u8(ts_cfg);
        rslt = comm_intf_send_command(&coines_rsp_buf);
    }

    return rslt;
}

/*********************************************************************/

/*!
 * @brief This API is used to read 16 bit SPI data
 *
 * @param[in] cs_pin : Chip select Pin
 * @param[in] reg_addr ; register address
 * @param[out] reg_data : register data
 * @param[in] count : count variable
 *
 * @return Result of API execution status
 */

static int16_t coines_read_16bit(uint8_t cs_pin, uint16_t reg_addr, uint16_t *reg_data, uint16_t count)
{
    int16_t rslt = COINES_SUCCESS;
    uint16_t bytes_remaining = count * 2; /* 2 bytes per word */
    uint16_t rsp_buf_pos = 0;
    uint16_t data_bytes_filled = 0;
    int16_t pkt_len = 0;
    uint16_t data_pos = 0;
    uint16_t index = 0;
    uint8_t msb_byte = 0;
    uint8_t lsb_byte = 0;
    uint8_t cnt = 0;

    if (reg_data == NULL)
        return COINES_E_NULL_PTR;

    /* Check if 16 bit SPI is configured. else return failure*/
    if (spi_16bit_enable == 0)
        return COINES_E_SPI16BIT_NOT_CONFIGURED;

    coines_rsp_buf.buffer_size = 0;
    memset(coines_rsp_buf.buffer, 0, COINES_DATA_BUF_SIZE);

    /* package size cannot be larger than 64 bytes (with header) */
    comm_intf_init_command_header(COINES_DD_GET, COINES_CMDID_16BIT_SPIWRITEANDREAD);

    /* always burst mode */
    comm_intf_put_u8(COINES_DD_BURST_MODE);

    /* update the CS pin as defined by the following values in DD2.0 protocol command */

    /*  0 -> I2C  1 -> SPI(CS_SENSOR)  2 -> SPI(CS_MULTIO_0)  3 -> SPI(CS_MULTIO_1)  4 -> SPI(CS_MULTIO_2)
     *       5 -> SPI(CS_MULTIO_3)  6 -> SPI(CS_MULTIO_4)  7 -> SPI(CS_MULTIO_5)  8 -> SPI(CS_MULTIO_6)
     *       9 -> SPI(CS_MULTIO_7)  10 -> SPI(CS_MULTIO_8) */
    if (cs_pin <= 8)
    {
        comm_intf_put_u8((cs_pin + 2));
    }
    else
    {
        /* On default select the CS_SENSOR which is 7th pin in the shuttle board*/
        comm_intf_put_u8(1);
    }

    comm_intf_put_u8(1); /* sensor id */
    comm_intf_put_u16(reg_addr);
    comm_intf_put_u16(count); /* Number of words(2 byte) */
    comm_intf_put_u8(1); /* write only once */
    comm_intf_put_u8(0); /* delay between writes */
    comm_intf_put_u8(1); /* read response */

    rslt = comm_intf_send_command(&coines_rsp_buf);

    if (rslt == COINES_SUCCESS)
    {
        if (coines_rsp_buf.buffer[COINES_IDENTIFIER_POSITION] != COINES_DD_RESP_ID)
            return COINES_E_COMM_WRONG_RESPONSE;
        if (coines_rsp_buf.buffer[COINES_RESPONSE_STATUS_POSITION] != COINES_SUCCESS)
            return COINES_E_COMM_IO_ERROR;
    }
    while (bytes_remaining > 0)
    {
        if (coines_rsp_buf.buffer[COINES_DD_FEATURE_POSITION] == COINES_CMDID_16BIT_SPIWRITEANDREAD)
        {
            /* 14 -> header size 12 bytes + packet delimiter 2 bytes*/
            pkt_len = (int16_t)coines_rsp_buf.buffer[rsp_buf_pos + COINES_BYTEPOS_PACKET_SIZE] - 14;
        }

        if ((pkt_len > 0) && (pkt_len <= (count * 2)))
        {
            data_pos = COINES_SPI_16BIT_READ_WRITE_DATA_START_POSITION + rsp_buf_pos;
            index = data_bytes_filled;
            for (cnt = 0; cnt < pkt_len; cnt += 2)
            {
                /*data_pos += cnt; */
                msb_byte = coines_rsp_buf.buffer[data_pos++];
                lsb_byte = coines_rsp_buf.buffer[data_pos++];
                reg_data[index++] = ((msb_byte << 8) | lsb_byte);
            }
        }
        /* Packet length will be negative when there is no valid packet in the response buffer.
         * In that case, reading the ring buffer again and see if any valid packet exists.if so,
         * copy to response buffer */
        else if (pkt_len < 0)
        {
            /* Resetting the response buffer */
            coines_rsp_buf.buffer_size = 0;
            memset(coines_rsp_buf.buffer, 0, COINES_DATA_BUF_SIZE);

            /* Reading the ring buffer and fill the response buffer */
            rslt = comm_intf_process_non_streaming_response(&coines_rsp_buf);

            /* Checking if the buffer is valid */
            if (coines_rsp_buf.buffer[COINES_IDENTIFIER_POSITION] != COINES_DD_RESP_ID)
                return COINES_E_COMM_WRONG_RESPONSE;
            if (coines_rsp_buf.buffer[COINES_RESPONSE_STATUS_POSITION] != COINES_SUCCESS)
                return COINES_E_COMM_IO_ERROR;

            if (coines_rsp_buf.buffer[COINES_DD_FEATURE_POSITION] == COINES_CMDID_16BIT_SPIWRITEANDREAD)
            {
                /* 14 -> header size 12 bytes + packet delimiter 2 bytes*/
                pkt_len = (int16_t)coines_rsp_buf.buffer[COINES_BYTEPOS_PACKET_SIZE] - 14;
            }

            if (pkt_len > 0)
            {
                /* Resetting the response buffer position */
                rsp_buf_pos = 0;

                /*Copy the data and adjust the buffer position*/
                data_pos = COINES_SPI_16BIT_READ_WRITE_DATA_START_POSITION;
                /*for(cnt=0,loop_cnt=0; cnt < (pkt_len * 2); cnt++,loop_cnt++) */
                for (cnt = 0; cnt < (pkt_len); cnt += 2)
                {
                    msb_byte = coines_rsp_buf.buffer[data_pos++];
                    lsb_byte = coines_rsp_buf.buffer[data_pos++];
                    reg_data[index++] = ((msb_byte << 8) | lsb_byte);
                }
            }
        }
        else
        {
            break;
        }
        if (pkt_len > 0)
        {
            /* Increment the packet to point to subsequent packet */
            data_bytes_filled = data_bytes_filled + pkt_len;

            /* Increase the response buffer position with the packet size */
            rsp_buf_pos = rsp_buf_pos + COINES_PACKET_SIZE;

            /* calculate the bytes remaining */
            bytes_remaining = bytes_remaining - pkt_len;
        }
    }

    return rslt;

}

/*!
 * @brief This API is used to read coines data
 *
 * @param[in] interface_type: Type of interface(USB, COM, or BLE).
 * @param[in] cs_pin : Chip select Pin
 * @param[in] dev_addr : device address
 * @param[in] reg_addr ; register address
 * @param[out] reg_data : register data
 * @param[in] count : count variable
 *
 * @return Result of API execution status
 */

static int16_t coines_read(enum coines_sensor_intf intf,
                           uint8_t cs_pin,
                           uint8_t dev_addr,
                           uint8_t reg_addr,
                           uint8_t *reg_data,
                           uint16_t count)
{

    int16_t rslt = COINES_SUCCESS;

    uint16_t bytes_remaining = count;
    uint16_t rsp_buf_pos = 0;
    uint16_t data_bytes_filled = 0;
    int16_t pkt_len = 0;

    if (reg_data == NULL)
        return COINES_E_NULL_PTR;

    coines_rsp_buf.buffer_size = 0;
    memset(coines_rsp_buf.buffer, 0, COINES_DATA_BUF_SIZE);

    comm_intf_init_command_header(COINES_DD_GET, COINES_CMDID_SENSORWRITEANDREAD);

    /* always burst mode */
    comm_intf_put_u8(COINES_DD_BURST_MODE);
    if (intf == COINES_SENSOR_INTF_I2C)
    {
        comm_intf_put_u8(0); /*interface I2C 0 */
    }
    else /* if (intf == COINES_SENSOR_INTF_SPI) */
    {
        if (cs_pin < COINES_MINI_SHUTTLE_PIN_1_4)
        {
            /* update the CS pin as defined by the following values in DD2.0 protocol command */
            /*  0 -> I2C  1 -> SPI(CS_SENSOR)  2 -> SPI(CS_MULTIO_0)  3 -> SPI(CS_MULTIO_1)  4 -> SPI(CS_MULTIO_2)
             5 -> SPI(CS_MULTIO_3)  6 -> SPI(CS_MULTIO_4)  7 -> SPI(CS_MULTIO_5)  8 -> SPI(CS_MULTIO_6)
             9 -> SPI(CS_MULTIO_7)  10 -> SPI(CS_MULTIO_8) */
            if (cs_pin <= 8)
            {
                comm_intf_put_u8((cs_pin + 2));
            }
            else
            {
                /* On default select the CS_SENSOR which is 7th pin in the shuttle board*/
                comm_intf_put_u8(1);
            }
        }
        else /* APP3.0 shuttle pin */
        {
            comm_intf_put_u8(cs_pin); /* APP3.0 */
        }
    }

    comm_intf_put_u8(1); /* sensor id */
    comm_intf_put_u8(1); /* analog switch */
    comm_intf_put_u16(dev_addr);
    comm_intf_put_u8(reg_addr);
    comm_intf_put_u16(count); /* byte count */
    comm_intf_put_u8(1); /* write only once */
    comm_intf_put_u8(0); /* delay between writes */
    comm_intf_put_u8(1); /* read response */
    rslt = comm_intf_send_command(&coines_rsp_buf);

    if (rslt == COINES_SUCCESS)
    {
        if (coines_rsp_buf.buffer[COINES_IDENTIFIER_POSITION] != COINES_DD_RESP_ID)
            return COINES_E_COMM_WRONG_RESPONSE;
        if (coines_rsp_buf.buffer[COINES_RESPONSE_STATUS_POSITION] != COINES_SUCCESS)
            return COINES_E_COMM_IO_ERROR;

        while (bytes_remaining > 0)
        {
            if (coines_rsp_buf.buffer[COINES_DD_COMMAND_ID_RESPONSE_POSITION] != COINES_EXTENDED_READ_RESPONSE_ID)
            {
                /* -13 -> header size 11 bytes + packet delimiter 2 bytes*/
                pkt_len = (int16_t)coines_rsp_buf.buffer[rsp_buf_pos + COINES_BYTEPOS_PACKET_SIZE] - 13;
            }
            else
            {
                /* Reading packet length from "No. of bytes" MSB and LSB bytes*/
                pkt_len = COINES_CALC_PACKET_LENGTH(coines_rsp_buf.buffer[COINES_BYTEPOS_LEN_MSB],
                                                    coines_rsp_buf.buffer[COINES_BYTEPOS_LEN_LSB]);
            }

            if ((pkt_len > 0) && (pkt_len <= count))
            {
                /*Copy the data and adjust the buffer position*/
                memcpy(&reg_data[data_bytes_filled],
                       &coines_rsp_buf.buffer[COINES_DD_READ_WRITE_DATA_START_POSITION + rsp_buf_pos], pkt_len);
            }
            /* Packet length will be negative when there is no valid packet in the response buffer.
             * In that case, reading the ring buffer again and see if any valid packet exists.if so,
             * copy to response buffer */
            else if (pkt_len < 0)
            {
                /* Resetting the response buffer */
                coines_rsp_buf.buffer_size = 0;
                memset(coines_rsp_buf.buffer, 0, COINES_DATA_BUF_SIZE);

                /* Reading the ring buffer and fill the response buffer */
                rslt = comm_intf_process_non_streaming_response(&coines_rsp_buf);

                if (rslt == COINES_SUCCESS)
                {
                    /* Checking if the buffer is valid */
                    if (coines_rsp_buf.buffer[COINES_IDENTIFIER_POSITION] != COINES_DD_RESP_ID)
                        return COINES_E_COMM_WRONG_RESPONSE;
                    if (coines_rsp_buf.buffer[COINES_RESPONSE_STATUS_POSITION] != COINES_SUCCESS)
                        return COINES_E_COMM_IO_ERROR;

                    pkt_len = (int16_t)coines_rsp_buf.buffer[COINES_BYTEPOS_PACKET_SIZE] - 13;

                    if (pkt_len > 0)
                    {
                        /* Resetting the response buffer position */
                        rsp_buf_pos = 0;

                        /*Copy the data and adjust the buffer position*/
                        memcpy(&reg_data[data_bytes_filled],
                               &coines_rsp_buf.buffer[COINES_DD_READ_WRITE_DATA_START_POSITION + rsp_buf_pos], pkt_len);
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
            if (pkt_len > 0)
            {
                /* Increment the packet to point to subsequent packet */
                data_bytes_filled = data_bytes_filled + pkt_len;
                /* Increase the response buffer position with the packet size */
                rsp_buf_pos = rsp_buf_pos + COINES_PACKET_SIZE;
                /* calculate the bytes remaining */
                bytes_remaining = bytes_remaining - pkt_len;
            }
        }
    }
    return rslt;
}

/*********************************************************************/
/*!
 * @brief This API is used to write number of words(2 bytes)
 *
 * @param[in] cs_pin : Chip select Pin
 * @param[in] reg_addr ; register address
 * @param[in] reg_data : register data
 * @param[in] word_count : word count variable
 *
 * @return Result of API execution status
 */
static int16_t coines_write_16bit(uint8_t cs_pin, uint16_t reg_addr, uint16_t *reg_data, uint16_t word_count)
{
    int16_t rslt = COINES_SUCCESS;
    uint16_t index = 0;
    uint16_t data_length = 0;
    uint16_t data_index = 0;

    if (reg_data == NULL)
        return COINES_E_NULL_PTR;
    if (spi_16bit_enable == 0)
        return COINES_E_SPI16BIT_NOT_CONFIGURED;

    while (word_count != 0)
    {
        if (word_count > 24)
        {
            word_count -= 24;
            data_length = 24;
        }
        else
        {
            data_length = word_count;
            word_count = 0;
        }
        coines_rsp_buf.buffer_size = 0;

        comm_intf_init_command_header(COINES_DD_SET, COINES_CMDID_16BIT_SPIWRITEANDREAD);

        /* always burst mode */
        comm_intf_put_u8(COINES_DD_BURST_MODE);

        /* update the CS pin as defined by the following values in DD2.0 protocol command */

        /*  0 -> I2C  1 -> SPI(CS_SENSOR)  2 -> SPI(CS_MULTIO_0)  3 -> SPI(CS_MULTIO_1)  4 -> SPI(CS_MULTIO_2)
         *   5 -> SPI(CS_MULTIO_3)  6 -> SPI(CS_MULTIO_4)  7 -> SPI(CS_MULTIO_5)  8 -> SPI(CS_MULTIO_6)
         *   9 -> SPI(CS_MULTIO_7)  10 -> SPI(CS_MULTIO_8) */
        if (cs_pin <= 8)
        {
            comm_intf_put_u8((cs_pin + 2));
        }
        else
        {
            /* On default select the CS_SENSOR which is 7th pin in the shuttle board*/
            comm_intf_put_u8(1);
        }

        comm_intf_put_u8(1); /*< sensor id */
        comm_intf_put_u16(reg_addr); /*< register address*/
        comm_intf_put_u16(data_length); /*< word count */
        comm_intf_put_u8(1); /*< write only once */
        comm_intf_put_u8(0); /*< delay between writes */
        comm_intf_put_u8(0); /*< write response */
        for (index = 0; index < data_length; index++)
        {
            comm_intf_put_u16(reg_data[index + data_index]);
        }
        rslt = comm_intf_send_command(&coines_rsp_buf);

        data_index += data_length;
    }

    return rslt;
}

static int16_t coines_write(enum coines_sensor_intf intf,
                            uint8_t cs_pin,
                            uint8_t dev_addr,
                            uint8_t reg_addr,
                            uint8_t *reg_data,
                            uint16_t count)
{
#if defined (ZEUS_QUIRK)
    if (coines_board == COINES_BOARD_ZEUS)
        return zeus_coines_write(intf, cs_pin, dev_addr, reg_addr, reg_data, count);
#endif
    int16_t rslt = COINES_SUCCESS;
    uint16_t index = 0;
    uint16_t data_length = 0;
    uint16_t data_index = 0;

    if (reg_data == NULL)
        return COINES_E_NULL_PTR;

    while (count != 0)
    {
        if (count > COINES_PACKET_PAYLOAD)
        {
            #if defined(DEBUG)
            printf("Write length exceeds %d !\n",COINES_PACKET_PAYLOAD);
            #endif
            return COINES_E_MEMORY_ALLOCATION;
        }
        else
        {
            data_length = count;
            count = 0;
        }
        coines_rsp_buf.buffer_size = 0;
        memset(coines_rsp_buf.buffer, 0, COINES_DATA_BUF_SIZE);

        comm_intf_init_command_header(COINES_DD_SET, COINES_CMDID_SENSORWRITEANDREAD);
        /* always burst mode */
        comm_intf_put_u8(COINES_DD_BURST_MODE);
        if (intf == COINES_SENSOR_INTF_I2C)
        {
            comm_intf_put_u8(0); /*interface I2C 0 */
        }
        else /* if (intf == COINES_SENSOR_INTF_SPI) */
        {
            if (cs_pin < COINES_MINI_SHUTTLE_PIN_1_4)
            {
                /* update the CS pin as defined by the following values in DD2.0 protocol command */
                /*  0 -> I2C  1 -> SPI(CS_SENSOR)  2 -> SPI(CS_MULTIO_0)  3 -> SPI(CS_MULTIO_1)  4 -> SPI(CS_MULTIO_2)
                 5 -> SPI(CS_MULTIO_3)  6 -> SPI(CS_MULTIO_4)  7 -> SPI(CS_MULTIO_5)  8 -> SPI(CS_MULTIO_6)
                 9 -> SPI(CS_MULTIO_7)  10 -> SPI(CS_MULTIO_8) */
                if (cs_pin <= 8)
                {
                    comm_intf_put_u8((cs_pin + 2));
                }
                else
                {
                    /* On default select the CS_SENSOR which is 7th pin in the shuttle board*/
                    comm_intf_put_u8(1);
                }
            }
            else /* APP3.0 shuttle pin */
            {
                comm_intf_put_u8(cs_pin); /* APP3.0 */
            }
        }
        comm_intf_put_u8(1); /*< sensor id */
        comm_intf_put_u8(1); /*< analog switch */
        comm_intf_put_u16(dev_addr); /*< device address*/
        comm_intf_put_u8(reg_addr); /*< register address*/
        comm_intf_put_u16(data_length); /*< byte count */
        comm_intf_put_u8(1); /*< write only once */
        comm_intf_put_u8(0); /*< delay between writes */
        comm_intf_put_u8(0); /*< read response */
        for (index = 0; index < data_length; index++)
        {
            comm_intf_put_u8(reg_data[index + data_index]);
        }
        rslt = comm_intf_send_command(&coines_rsp_buf);

        data_index += data_length;
    }

    return rslt;
}

/*!
 * @brief This API returns the number of milliseconds passed since the program started
 *
 * @return Time in milliseconds
 */
uint32_t coines_get_millis()
{
    return (uint32_t)clock();
}

/** @}*/

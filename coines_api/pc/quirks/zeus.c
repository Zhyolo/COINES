/**
 * Copyright (C) 2018 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    zeus.c
 * @brief 	This file contains protocol implementation specific to Zeus board
 *
 */
#include "zeus.h"

extern coines_rsp_buffer_t coines_rsp_buf;
/**********************************************************************************/
/* functions */
/**********************************************************************************/
/*!
 *
 * @brief       : API to get the hardware pin
 *
 */
int16_t zeus_coines_write(enum coines_sensor_intf intf,
                          uint8_t cs_pin,
                          uint8_t dev_addr,
                          uint8_t reg_addr,
                          uint8_t *reg_data,
                          uint16_t count)
{
    int16_t rslt = COINES_SUCCESS;
    uint16_t index = 0;

    if (reg_data == NULL)
        return COINES_E_NULL_PTR;

    coines_rsp_buf.buffer_size = 0;
    comm_intf_init_command_header(COINES_DD_SET, COINES_CMDID_SENSORWRITEANDREAD);
    /* always burst mode */
    comm_intf_put_u8(COINES_DD_BURST_MODE);
    if (intf == COINES_SENSOR_INTF_I2C)
    {
        comm_intf_put_u8(0);   //interface I2C 0
    }
    else /* if (intf == COINES_SENSOR_INTF_SPI) */
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
    comm_intf_put_u8(1); /*< sensor id */
    comm_intf_put_u8(1); /*< analog switch */
    comm_intf_put_u16(dev_addr); /*< device address */
    comm_intf_put_u8(reg_addr); /*< register address */
    comm_intf_put_u16(count); /*< byte count */
    comm_intf_put_u8(1); /*< write only once */
    comm_intf_put_u8(0); /*< delay between writes */
    comm_intf_put_u8(0); /*< read response */
    for (index = 0; index < count; index++)
    {
        comm_intf_put_u8(reg_data[index]);
    }
    rslt = comm_intf_send_command(&coines_rsp_buf);

    return rslt;
}

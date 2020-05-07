/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    usb_dfu.c
 * @date    Apr 23, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief   Cointains USB DFU Handlers for APP3.0
 */
/**********************************************************************************/
/*  header includes */
/**********************************************************************************/
#include "usb_dfu.h"
#include <string.h>

/**********************************************************************************/
/* variables */
/**********************************************************************************/
static uint8_t *flash_ptr = (uint8_t *) FLASH_START;
static uint8_t *ram_ptr = (uint8_t *) RAM_START;

static uint8_t code[64];
static uint16_t code_size;
static uint8_t flash_write_buffer[FLASH_BUFF_SIZE + 1] = {};
uint8_t setting = RAM_SETTING;

static dfu_status_t dfu_status;

static nrf_drv_usbd_setup_t setup;
static uint32_t buffer_index = 0;

/**********************************************************************************/
/* functions */
/**********************************************************************************/
/*!
 *
 * @brief       : This API is used to set the interface (RAM/FLASH)
 */
void interface_set_setting(nrf_drv_usbd_setup_t const * const p_setup)
{
    setting = (p_setup->wValue) & 0xFF;
    dfu_reset();
    if (setting == RAM_SETTING)
    {
        mem_read = RAM_read;
        mem_write = RAM_write;
        ram_ptr = (uint8_t *) RAM_START;
    }
    else if (setting == FLASH_SETTING)
    {
        mem_read = Flash_read;
        mem_write = Flash_write;
        flash_ptr = (uint8_t *) FLASH_START;
    }
}
/*!
 * @brief       : This API is used to reset Device Firmware Upgrade
 *
 */
void dfu_reset()
{
    dfu_status.bStatus = DFU_OK;
    dfu_status.bState = DFU_dfuIDLE;
    dfu_op_cb = NULL;

    flash_ptr = (uint8_t *) FLASH_START;
    ram_ptr = (uint8_t *) RAM_START;
}
/*!
 *
 * @brief       : USB callback handler to service DFU requests
 */
void service_dfu_requests(nrf_drv_usbd_setup_t const * const p_setup)
{
    memcpy(&setup, p_setup, sizeof(setup));
    if (p_setup->bmRequestType == 0x21)
    {
        switch (p_setup->bmRequest)
        {
            case DFU_ABORT:
                dfu_reset();
                break;

            case DFU_CLRSTATUS:
                dfu_reset();
                break;

            case DFU_DETACH:
                if (setting == RAM_SETTING)
                    APP_START_ADDR = RAM_START;
                else if (setting == FLASH_SETTING)
                    APP_START_ADDR = FLASH_START;

                memcpy(MAGIC_INFO_ADDR, "COIN", 4);
                __DSB();
                __ISB();
                __DMB();
                NVIC_SystemReset();
                break;

            case DFU_DNLOAD:
                dfu_op_cb = mem_write;
                code_size = p_setup->wLength;
                initiate_setup_data(p_setup, code, code_size);

                /* wLength = 0 denotes end of transfer */
                if (setup.wLength == 0
                    && setting == FLASH_SETTING && (uint32_t)flash_ptr < FLASH_END)
                {
                    dfu_status.bStatus = DFU_OK;
                    dfu_status.bState = DFU_dfuDNBUSY;

                    for (int i = 0; i < FLASH_BUFF_SIZE / 4096; i++)
                        nrf_nvmc_page_erase((uint32_t)(flash_ptr + i * 4096));

                    nrf_nvmc_write_words((uint32_t)flash_ptr,
                                         (uint32_t *)flash_write_buffer,
                                         (buffer_index / 4) + 1);
                    buffer_index = 0;

                    dfu_status.bStatus = DFU_OK;
                    dfu_status.bState = DFU_dfuDNLOAD_IDLE;
                }
                break;
            default:
                break;

        }

        return;
    }

    if (p_setup->bmRequestType == 0xA1)
    {
        switch (p_setup->bmRequest)
        {
            case DFU_GETSTATUS:
                respond_setup_data(p_setup, &dfu_status, sizeof(dfu_status));
                break;

            case DFU_UPLOAD:
                if (setting == RAM_SETTING)
                {
                    memcpy(code, ram_ptr, p_setup->wLength);
                    ram_ptr += p_setup->wLength;
                }
                else if (setting == FLASH_SETTING)
                {
                    memcpy(code, flash_ptr, p_setup->wLength);
                    flash_ptr += p_setup->wLength;
                }
                respond_setup_data(p_setup, code, p_setup->wLength);
                dfu_op_cb = mem_read;

                break;
            default:
                break;
        }

        return;
    }
}
/*!
 *
 * @brief       : USB callback handler for writing to FLASH
 */
void Flash_write()
{
    if ((uint32_t)flash_ptr > (FLASH_END - 1))
    {
        dfu_status.bStatus = DFU_errTARGET;
        dfu_status.bState = DFU_dfuERROR;
        return;
    }

    memcpy(&flash_write_buffer[buffer_index], code, code_size);
    buffer_index += code_size;

    if (buffer_index == FLASH_BUFF_SIZE)
    {
        dfu_status.bStatus = DFU_OK;
        dfu_status.bState = DFU_dfuDNBUSY;

        for (int i = 0; i < FLASH_BUFF_SIZE / 4096; i++)
            nrf_nvmc_page_erase((uint32_t)(flash_ptr + i * 4096));

        nrf_nvmc_write_words((uint32_t)flash_ptr,
                             (uint32_t *)flash_write_buffer,
                             FLASH_BUFF_SIZE / 4);

        flash_ptr += FLASH_BUFF_SIZE;
        memset(flash_write_buffer, 0xFF, FLASH_BUFF_SIZE);

        buffer_index = 0;
    }

    dfu_status.bStatus = DFU_OK;
    dfu_status.bState = DFU_dfuDNLOAD_IDLE;
}
/*!
 *
 * @brief       : USB callback handler for reading FLASH
 *
 */
void Flash_read()
{
    if ((uint32_t)flash_ptr > FLASH_END)
    {
        dfu_reset();
        nrf_drv_usbd_stop();
        nrf_drv_usbd_disable();
        nrf_drv_usbd_enable();
        nrf_drv_usbd_start(true);
        return;
    }

    dfu_status.bStatus = DFU_OK;
    dfu_status.bState = DFU_dfuUPLOAD_IDLE;
}
/*!
 *
 * @brief       : USB callback handler for writing to RAM
 *
 */
void RAM_write()
{
    if ((uint32_t)ram_ptr > (RAM_END - 1))
    {
        dfu_status.bStatus = DFU_errTARGET;
        dfu_status.bState = DFU_dfuERROR;
        return;
    }

    dfu_status.bStatus = DFU_OK;
    dfu_status.bState = DFU_dfuDNBUSY;

    memcpy(ram_ptr, code, code_size);
    ram_ptr += code_size;

    dfu_status.bStatus = DFU_OK;
    dfu_status.bState = DFU_dfuDNLOAD_IDLE;
}
/*!
 *
 * @brief       : USB callback handler for reading RAM
 *
 */
void RAM_read()
{
    if ((uint32_t)ram_ptr > RAM_END)
    {
        dfu_reset();
        nrf_drv_usbd_stop();
        nrf_drv_usbd_disable();
        nrf_drv_usbd_enable();
        nrf_drv_usbd_start(true);
        return;
    }

    dfu_status.bStatus = DFU_OK;
    dfu_status.bState = DFU_dfuUPLOAD_IDLE;
}
/*!
 *
 * @brief       : USB transfer completion callback handler
 *
 */
void usb_control_transfer_complete_cb()
{
    if (dfu_op_cb != NULL)
    {
        dfu_op_cb();
        dfu_op_cb = NULL;
    }
}


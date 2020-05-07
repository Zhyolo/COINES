/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    usb_core.h
 * @date    Apr 23, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief   Structure and function declarations for USB core
 *
 */

#ifndef USB_CORE_H_
#define USB_CORE_H_
/**********************************************************************************/
/*  header includes */
/**********************************************************************************/
#include "nrf.h"
#include "nrf_drv_usbd.h"
#include "nrf_drv_power.h"

/**********************************************************************************/
/* function declaration */
/**********************************************************************************/
/*!
 *
 * @brief       : API to initialize USB
 *
 * @param[in]   : None
 *
 * @return      : None
 */
void init_usb(void);
/*!
 *
 * @brief       : API for responding  to setup data
 *
 * @param[in]   : nrf_drv_usbd_setup_t const * const p_setup, void const * p_data, size_t size
 *
 * @return      : None
 */
void respond_setup_data(nrf_drv_usbd_setup_t const * const p_setup,
                        void const * p_data,
                        size_t size);
/*!
 *
 * @brief       : API to initiate setup data
 *
 * @param[in]   : nrf_drv_usbd_setup_t const * const p_setup, void const * p_data, size_t size
 *
 * @return      : None
 */
void initiate_setup_data(nrf_drv_usbd_setup_t const * const p_setup,
                         void * p_data,
                         size_t size);
/*!
 *
 * @brief       : USB transfer completion callback handler
 *
 * @param[in]   : None
 *
 * @return      : None
 */
void usb_control_transfer_complete_cb();
/*!
 *
 * @brief       : Microsoft WCID descriptor
 */
typedef struct
{
    uint8_t bFirstInterfaceNumber; /* first interface number */
    uint8_t reserved1; /* reserved */
    uint8_t compatibleID[8]; /* compatible ID */
    uint8_t subCompatibleID[8]; /* subcompatible ID */
    uint8_t reserved2[6]; /* reserved */
}__attribute__((packed)) MicrosoftCompatibleDescriptor_Interface;

/*!
 *
 * @brief       : Microsoft WCID descriptor
 */
typedef struct
{
    uint32_t dwLength; /* length */
    uint16_t bcdVersion; /* BCD version */
    uint16_t wIndex; /* Index */
    uint8_t bCount; /* count */
    uint8_t reserved[7]; /* reserved */
    MicrosoftCompatibleDescriptor_Interface interfaces[];
}__attribute__((packed)) MicrosoftCompatibleDescriptor;

#endif /* USB_CORE_H_ */

/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    usb_core.h
 * @date    Apr 23, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief   Definitions for USB core
 *
 */

#ifndef USB_CORE_H_
#define USB_CORE_H_

/**********************************************************************************/
/*header files */
/**********************************************************************************/
#include "nrf.h"
#include "nrf_drv_usbd.h"
#include "nrf_drv_power.h"

/**********************************************************************************/
/*function declarations */
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
 * @brief       : API to create response data for setup data
 *
 * @param[in]   : nrf_drv_usbd_setup_t const * const p_setup, const * p_data, size
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
 * @param[in]   : nrf_drv_usbd_setup_t const * const p_setup,void * p_data,size
 *
 * @return      : None
 */
void initiate_setup_data(nrf_drv_usbd_setup_t const * const p_setup,
                         void * p_data,
                         size_t size);
/*!
 *
 * @brief       : API to send data to host
 *
 * @param[in]   : void * p_data,size
 *
 * @return      : None
 */
void send_data_to_host(void * p_data, size_t size);
/*!
 *
 * @brief       : API to recveive data from host
 *
 * @param[in]   : void * p_data,size
 *
 * @return      : None
 */
void receive_data_from_host(void * p_data, size_t size);
/*!
 *
 * @brief       : API to transmit data through USB
 *
 * @param[in]   : None
 *
 * @return      : None
 */
void usb_transmit_complete_cb(void);
/*!
 *
 * @brief       : API to receive data from USB
 *
 * @param[in]   : None
 *
 * @return      : None
 */
void usb_receive_complete_cb(void);
/*!
 *
 * @brief       : API to initiate mtp
 *
 * @param[in]   : None
 *
 * @return      : None
 */
void mtp_init();

/*!
 *
 * @brief       : Microsoft WCID descriptor
 */
typedef struct
{
    uint8_t bFirstInterfaceNumber; /*< first interface number */
    uint8_t reserved1; /*< reversed 1 */
    uint8_t compatibleID[8]; /*< compatible ID */
    uint8_t subCompatibleID[8]; /*< subcompatible ID */
    uint8_t reserved2[6]; /*< reversed 2*/
}__attribute__((packed)) MicrosoftCompatibleDescriptor_Interface;
/*!
 * @brief		:structure for Microsoft compatible descriptor
 */
typedef struct
{
    uint32_t dwLength; /*< length */
    uint16_t bcdVersion; /*< bcd version */
    uint16_t wIndex; /*< index */
    uint8_t bCount; /*< count */
    uint8_t reserved[7];/*< reserved */
    MicrosoftCompatibleDescriptor_Interface interfaces[];
}__attribute__((packed)) MicrosoftCompatibleDescriptor;

#endif /* USB_CORE_H_ */

/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    usb_dfu.h
 * @date    Apr 23, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief   Declarations and Definitions for USB DFU
 *
 */
#ifndef USB_DFU_H_
#define USB_DFU_H_

/**********************************************************************************/
/* system header includes */
/**********************************************************************************/
#include <stdint.h>
#include "nrf_drv_usbd.h"
#include "nrf_nvmc.h"
#include "usb_core.h"

/**********************************************************************************/
/* macro definitions */
/**********************************************************************************/

#define  DFU_DETACH     0x00
#define  DFU_DNLOAD     0x01
#define  DFU_UPLOAD     0x02
#define  DFU_GETSTATUS  0x03
#define  DFU_CLRSTATUS  0x04
#define  DFU_GETSTATE   0x05
#define  DFU_ABORT      0x06

/* DFU Status */
#define  DFU_OK                    0x00
#define  DFU_errTARGET             0x01
#define  DFU_errFILE               0x02
#define  DFU_errWRITE              0x03
#define  DFU_errERASE              0x04
#define  DFU_errCHECK_ERASED       0x05
#define  DFU_errPROG               0x06
#define  DFU_errVERIFY             0x07
#define  DFU_errADDRESS            0x08
#define  DFU_errNOTDONE            0x09
#define  DFU_errFIRMWARE           0x0A
#define  DFU_errVENDOR             0x0B
#define  DFU_errUSBR               0x0C
#define  DFU_errPOR                0x0D
#define  DFU_errUNKNOWN            0x0E
#define  DFU_errSTALLEDPK          0x0F

/* DFU States */
#define  DFU_appIDLE                 0x00
#define  DFU_appDETACH               0x01
#define  DFU_dfuIDLE                 0x02
#define  DFU_dfuDNLOAD_SYNC          0x03
#define  DFU_dfuDNBUSY               0x04
#define  DFU_dfuDNLOAD_IDLE          0x05
#define  DFU_dfuMANIFEST_SYNC        0x06
#define  DFU_dfuMANIFEST             0x07
#define  DFU_dfuMANIFEST_WAIT_RESET  0x08
#define  DFU_dfuUPLOAD_IDLE          0x09
#define  DFU_dfuERROR                0x0A

#define RAM_SETTING    0
#define FLASH_SETTING  1

#define FLASH_BUFF_SIZE  8192

#define FLASH_START			(0x00030000)  // Start at 192kB
#define FLASH_END 			(FLASH_START + 0xC0000) // 192kB + 768kB = 960kB

// 960 - 1024kB (64kB Bootloader)

#define RAM_START           (0x00810000) // Start at 64kB
#define RAM_END             (RAM_START + 0x30000 - 16) // End at approx. 256kB, 16B - reserved

/* Reserved RAM location - 16 bytes */
#define  MAGIC_LOCATION         (0x2003FFF4)
#define  MAGIC_INFO_ADDR        ((int8_t *)(MAGIC_LOCATION))
#define  APP_START_ADDR         (*(uint32_t *)(MAGIC_LOCATION+4))
#define  APP_SP_VALUE           (*(uint32_t *)APP_START_ADDR)
#define  APP_RESET_HANDLER_ADDR (*(uint32_t *)(APP_START_ADDR+4))

typedef struct
{
    uint8_t bStatus;
    uint8_t bwPollTimeout[3];
    uint8_t bState;
    uint8_t iString;
}__attribute__((packed)) dfu_status_t;

/*!
 *
 * @brief       : This API is used to reset Device Firmware Upgrade
 *
 * @param[in]   : None
 *
 * @return      : None
 */
void dfu_reset();
/*!
 *
 * @brief       : Callback handler to set the interface (RAM/FLASH)
 *
 * @param[in]   : nrf_drv_usbd_setup_t const * const p_setup
 *
 * @return      : None
 */
void interface_set_setting(nrf_drv_usbd_setup_t const * const p_setup);
/*!
 *
 * @brief       : USB callback handler to service DFU requests
 *
 * @param[in]   : nrf_drv_usbd_setup_t const * const p_setup
 *
 * @return      : None
 */
void service_dfu_requests(nrf_drv_usbd_setup_t const * const p_setup);
/*!
 *
 * @brief       : USB callback handler for writing to FLASH
 *
 * @param[in]   : None
 *
 * @return      : None
 */
void Flash_write();
/*!
 *
 * @brief       : USB callback handler for reading FLASH
 *
 * @param[in]   : None
 *
 * @return      : None
 */
void Flash_read();
/*!
 *
 * @brief       : USB callback handler for writing to RAM
 *
 * @param[in]   : None
 *
 * @return      : None
 */
void RAM_write();
/*!
 *
 * @brief       : USB callback handler for reading RAM
 *
 * @param[in]   : None
 *
 * @return      : None
 */
void RAM_read();

void (*mem_read)(void);
void (*mem_write)(void);

void (*dfu_op_cb)(void);

#endif /* USB_DFU_USB_DFU_H_ */

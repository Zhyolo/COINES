/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    usb_dfu_bootloader.h
 * @date    Apr 26, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief   Definitions for USB DFU Bootloader for  Nüwa Board | nRF52840 chip
 *
 */
#ifndef USB_DFU_BOOTLOADER_H_
#define USB_DFU_BOOTLOADER_H_

/**********************************************************************************/
/* header includes */
/**********************************************************************************/
#include <stdint.h>
#include "nrf.h"
#include "nrf_drv_clock.h"
#include "usb_core.h"
#include "usb_dfu.h"
#include "nrf_gpio.h"

/**********************************************************************************/
/* macro definitions */
/**********************************************************************************/
#define BOOTLOADER_START_ADDR (0xF0000)
#define USB_MTP_FW_START_ADDR (0x28000)
#define BUTTON_1              NRF_GPIO_PIN_MAP(1,9)
#define BUTTON_2              NRF_GPIO_PIN_MAP(0,25)
#define MCU_LED_B             NRF_GPIO_PIN_MAP(0,12)

#ifndef BOOTLOADER_VERSION
#define BOOTLOADER_VERSION "unknown"
#endif
/**********************************************************************************/
/* function declarations */
/**********************************************************************************/
/*!
 *
 * @brief       : API to switch application
 *
 * @param[in]   : None
 *
 * @return      : None
 */
void app_switch();

#endif /* USB_DFU_BOOTLOADER_H_ */

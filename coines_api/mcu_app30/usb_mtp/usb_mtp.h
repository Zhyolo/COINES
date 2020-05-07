/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    usb_mtp.h
 * @date    June 22, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief   Definitions for USB MTP firmware for APP3.0 Board
 *
 */
#ifndef USB_MTP_H_
#define USB_MTP_H_
/**********************************************************************************/
/* header files */
/**********************************************************************************/
#include <stdint.h>
#include <stddef.h>

#include "nrf.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_power.h"
#include "nrf_drv_usbd.h"
#include "nrf_gpio.h"
#include "usb_core.h"
#include "mtp.h"
#include "mtp_support.h"

#define MCU_LED_G      NRF_GPIO_PIN_MAP(0,11)

#ifndef USB_MTP_FW_VERSION
#define USB_MTP_FW_VERSION "unknown"
#endif

#endif /* USB_MTP_H_ */

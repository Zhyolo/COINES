/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    usb_mtp.c
 * @date    Jun 22, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief   USB MTP firmware for APP3.0 Board
 *          Compliant with USB Class MTP Specification v1.1
 *          https://www.usb.org/sites/default/files/MTPv1_1.zip
 */

#include "usb_mtp.h"

/*  Firmware info. can be retreived after compilation using the below command
 *    strings usb_mtp.bin | grep info
 */
char volatile * const build_info =
"info : "__DATE__ "," __TIME__ " | "
"GCC " __VERSION__ " | "
USB_MTP_FW_VERSION ;
/**********************************************************************************/
/* static functions                                                                      */
/**********************************************************************************/
/*!
 *
 * @brief       : API to initialize power clock
 *
 * @param[in]   : None
 *
 * @return      : None
 */
static void init_power_clock(void);

int main()
{
    init_power_clock();
    init_usb();
    nrf_gpio_cfg_output(MCU_LED_G); // Make Green LED pin as output
    nrf_gpio_pin_clear(MCU_LED_G); // Turn on Green LED

    while (1)
    {
        mtp_loop();
    }
    return 0;
}
/*!
 *
 * @brief       : API to initialize power clock
 */
static void init_power_clock(void)
{
    /* Initializing power and clock */
    nrf_drv_clock_init();
    nrf_drv_power_init(NULL);
    nrf_drv_clock_hfclk_request(NULL);
    nrf_drv_clock_lfclk_request(NULL);
    while (!(nrf_drv_clock_hfclk_is_running() &&
             nrf_drv_clock_lfclk_is_running()))
    {
        /* Just waiting */
    }

}

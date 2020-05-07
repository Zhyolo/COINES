/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    usb_dfu_bootloader.c
 * @date    Apr 23, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief   USB DFU Bootloader for  APP3.0 Board | nRF52840 chip
 *          Compliant with USB Class DFU Specification v1.1
 *          https://www.usb.org/sites/default/files/DFU_1.1.pdf
 *          Tools available
 *          - dfu-util : http://dfu-util.sourceforge.net/
 *          - WebDFU : https://devanlai.github.io/webdfu/dfu-util/
 */

#include "usb_dfu_bootloader.h"
#include "nrf_mbr.h"
#include "nrf_sdm.h"

/*  Bootloader info. can be retreived after compilation using the below command
 *    strings usb_dfu_bootloader.bin | grep info
 */
char volatile * const build_info =
"info : "__DATE__ "," __TIME__ " | "
"GCC " __VERSION__ " | "
BOOTLOADER_VERSION ;

/**********************************************************************************/
/* static function declaration */
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
    nrf_gpio_cfg_input(BUTTON_1, NRF_GPIO_PIN_PULLUP); // Configure BUTTON_1 as input
    nrf_gpio_cfg_input(BUTTON_2, NRF_GPIO_PIN_PULLUP); // Configure BUTTON_2 as input
    nrf_gpio_cfg_output(MCU_LED_B); // Make Blue LED pin as output
    nrf_gpio_pin_set(MCU_LED_B); // Turn off Blue LED

    app_switch();
    init_power_clock();
    init_usb();

    while (1)
    {

    }
    return 0;
}
/*!
 *
 * @brief       : API to initiate power clock
 *
 */
static void init_power_clock(void)
{
    /* Initializing power and clock */
    nrf_drv_clock_init();
    nrf_drv_power_init(NULL);
    nrf_drv_clock_hfclk_request(NULL);
    nrf_drv_clock_lfclk_request(NULL);
    while (!(nrf_drv_clock_hfclk_is_running()
             && nrf_drv_clock_lfclk_is_running()))
    {
        /* Just waiting */
    }

}

/*
 * Switch to application specified at APP_START_ADDR if magic info. "COIN" is set (or)
 * Jump to Application residing in Flash memory if BUTTON_2 is not pressed (or)
 * Stay in Bootloader mode itself
 */
/*!
 *
 * @brief       : API to switch application
 *
 */
void app_switch()
{
    sd_mbr_command_t com = { SD_MBR_COMMAND_INIT_SD, };
    sd_mbr_command(&com);

    sd_softdevice_disable();

    void (*App_ResetJumpAddr)();

    /* Set to USB MTP firmware start addr. if BUTTON_1 is pressed */
    if (!nrf_gpio_pin_read(BUTTON_1))
        APP_START_ADDR = USB_MTP_FW_START_ADDR;

    /* Set to Bootloader start addr. if BUTTON_2 is pressed */
    else if (!nrf_gpio_pin_read(BUTTON_2))
        APP_START_ADDR = BOOTLOADER_START_ADDR;

    /* If magic info "COIN" is not present in MAGIC_LOCATION,then start application at FLASH_START */
    else if (*((uint32_t *) MAGIC_LOCATION) != 0x4E494F43 || /* 0x4E494F43 ==> 'N' ,'I', 'O', 'C' */
    !(APP_SP_VALUE > 0x20000000 && APP_SP_VALUE <= 0x20040000) /*Check if the application has invalid Stack pointer value*/
    )
        APP_START_ADDR = FLASH_START;

    if ((APP_SP_VALUE > 0x20000000 && APP_SP_VALUE <= 0x20040000) && /*Check if the application has vaild Stack pointer value*/
    (APP_START_ADDR != 0 && APP_START_ADDR != BOOTLOADER_START_ADDR) /*Check if address is not of bootloader*/
    )
    {
        sd_softdevice_vector_table_base_set(APP_START_ADDR); /*Switch to new Application's Interrupt Vector Table */
        App_ResetJumpAddr = (void (*)()) APP_RESET_HANDLER_ADDR;
        App_ResetJumpAddr(); /*Jump to new Application*/
    }

    /* None of them matches ! Just set the Interrupt vector table address of Bootloader */
    sd_softdevice_vector_table_base_set(BOOTLOADER_START_ADDR);
    nrf_gpio_pin_clear(MCU_LED_B); /* Indicate bootloader mode */
}

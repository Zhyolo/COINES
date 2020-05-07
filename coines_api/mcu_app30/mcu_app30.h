/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    mcu_app30.h
 * @date    Mar 26, 2019
 * @brief   This file contains COINES layer function prototypes, variable declarations and Macro definitions
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>

#include "nrf.h"
#include "nrf_drv_usbd.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "nrf_delay.h"
#include "nrf_drv_power.h"
#include "nrfx_spim.h"
#include "nrfx_twim.h"

#include "app_error.h"
#include "app_util.h"
#include "app_usbd_core.h"
#include "app_usbd.h"
#include "app_usbd_string_desc.h"
#include "app_usbd_cdc_acm.h"
#include "app_usbd_serial_num.h"

#include "app30_eeprom.h"
#include "flogfs.h"
#include "w25n01gwtbig.h"

#define VDD_SEL				NRF_GPIO_PIN_MAP(0, 27)
#define VDD_PS_EN			NRF_GPIO_PIN_MAP(0, 3)
#define VDDIO_PS_EN			NRF_GPIO_PIN_MAP(0, 28)

#define  GPIO_0    NRF_GPIO_PIN_MAP(0,14)   /* SB1_4 - P0.14 (Formerly APP_SCL) */
#define  GPIO_1    NRF_GPIO_PIN_MAP(0,13)   /* SB1_5 - P0.13 (Formerly APP_SDA) */
#define  GPIO_2    NRF_GPIO_PIN_MAP(1,1)    /* INT1 - SB1_6 - P1.01 (Formerly APP_INT1) */
#define  GPIO_3    NRF_GPIO_PIN_MAP(1,8)    /* INT2 - SB1_7 - P1.08 (Formerly APP_INT2) */
#define  GPIO_CS   NRF_GPIO_PIN_MAP(0,24)   /* SB2_1 - P0.24 */
#define  GPIO_SDO  NRF_GPIO_PIN_MAP(0,15)   /* SB2_3 - P0.15*/
#define  GPIO_4    NRF_GPIO_PIN_MAP(1,3)    /* SB2_5 - P1.03 (Formerly APP_RX) */
#define  GPIO_5    NRF_GPIO_PIN_MAP(1,2)    /* SB2_6 - P1.02 (Formerly APP_TX) */

#define I2C_SEN_INSTANCE			0
#define I2C_SEN_SDA					NRF_GPIO_PIN_MAP(0,6)
#define I2C_SEN_SCL					NRF_GPIO_PIN_MAP(0,16)

#define SPI_INSTANCE				1
#define SPI_I2C_SEN_SCK				NRF_GPIO_PIN_MAP(0, 16) /* Sharing I2C with SPI Pins*/
#define SPI_I2C_SEN_MOSI			NRF_GPIO_PIN_MAP(0, 6) /* Sharing I2C with SPI Pins*/
#define SPI_SEN_MISO				NRF_GPIO_PIN_MAP(0, 15)

#define MAX_FILE_DESCRIPTORS        5

#define CDC_ACM_COMM_INTERFACE  0
#define CDC_ACM_COMM_EPIN       NRF_DRV_USBD_EPIN2

#define CDC_ACM_DATA_INTERFACE  1
#define CDC_ACM_DATA_EPIN       NRF_DRV_USBD_EPIN1
#define CDC_ACM_DATA_EPOUT      NRF_DRV_USBD_EPOUT1

#define MCU_LED_R               NRF_GPIO_PIN_MAP(0,7)
#define MCU_LED_G               NRF_GPIO_PIN_MAP(0,11)
#define MCU_LED_B               NRF_GPIO_PIN_MAP(0,12)

#define SWITCH1                 NRF_GPIO_PIN_MAP(1,9)
#define SWITCH2                 NRF_GPIO_PIN_MAP(0,25)

#define LED_BLINK_MAX_DELAY     (64)
/**********************************************************************************/
/* functions */
/**********************************************************************************/
static void cdc_acm_user_ev_handler(app_usbd_class_inst_t const * p_inst,
        app_usbd_cdc_acm_user_event_t event);

APP_USBD_CDC_ACM_GLOBAL_DEF(m_app_cdc_acm,
        cdc_acm_user_ev_handler,
        CDC_ACM_COMM_INTERFACE,
        CDC_ACM_DATA_INTERFACE,
        CDC_ACM_COMM_EPIN,
        CDC_ACM_DATA_EPIN,
        CDC_ACM_DATA_EPOUT,
        APP_USBD_CDC_COMM_PROTOCOL_NONE
        );

typedef void (*ISR_CB)(void);
static ISR_CB isr_cb[24];
static void gpioHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

/****** Reserved Memory Area for performing application switch - 16 bytes******/
#define  MAGIC_LOCATION         (0x2003FFF4)
#define  MAGIC_INFO_ADDR        ((int8_t *)(MAGIC_LOCATION))
#define  APP_START_ADDR         (*(uint32_t *)(MAGIC_LOCATION+4))
#define  APP_SP_VALUE           (*(uint32_t *)APP_START_ADDR)
#define  APP_RESET_HANDLER_ADDR (*(uint32_t *)(APP_START_ADDR+4))


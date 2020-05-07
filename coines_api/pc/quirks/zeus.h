/**
 * Copyright (C) 2018 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    zeus.h
 * @brief This file contains COINES layer function prototypes, variable declarations and Macro definitions
 *
 */
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
/*********************************************************************/
/* own header files */
/*********************************************************************/
#include "coines.h"
#include "coines_defs.h"
#include "comm_intf.h"
/*********************************************************************/
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
                          uint16_t count);

/**
 * Copyright (C) 2018 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    app30_eeprom.h
 * @date    Sep 20, 2018
 * @brief This file contains support for 1-wire EEPROM
 *
 */

/*!
 * @addtogroup nuwa_eeprom
 * @{*/

#ifndef PLATFORM_EEPROM_APP30_EEPROM_H_
#define PLATFORM_EEPROM_APP30_EEPROM_H_

/**********************************************************************************/
/* functions declarations*/
/**********************************************************************************/
/*!
 *
 * @brief       : API initialize APP3.0 EEPROM
 *
 * @param[in]   : None
 *
 * @return      : None
 */
void app30_eeprom_init(void);
/*!
 *
 * @brief       : API to read the internal ROM ID, and checks whether the read id is correct or not
 *
 * @param[in]   : uint8_t *buffer
 *
 * @return      : true  : read the correct rom id
 * 			      false : unable to read the rom id or slave is not responding
 */
bool app30_eeprom_romid(uint8_t *buffer);
/*!
 *
 * @brief       : API to read APP3.0 EEPROM
 *
 * @param[in]   : address,pointer to the buffer,length
 *
 * @return      : true/false
 */
bool app30_eeprom_read(uint16_t address, uint8_t *buffer, uint8_t length);
/*!
 *
 * @brief       : API to write APP3.0 EEPROM
 *
 * @param[in]   : address,pointer to the buffer,length
 *
 * @return      : true/false
 */
bool nuwa_eeprom_write(uint8_t address, uint8_t *buffer, uint8_t length);

#endif /* PLATFORM_POWER_NUWA_POWER_H_ */

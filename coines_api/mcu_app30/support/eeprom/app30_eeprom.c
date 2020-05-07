/**
 * Copyright (C) 2018 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    app30_eeprom.c
 * @date    Sep 25, 2018
 * @brief   This file contains read/write support for 1-wire EEPROM
 *
 */

/*!
 * @addtogroup app30_eeprom
 * @{*/

/**********************************************************************************/
/* header files */
/**********************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "ds28e05.h"

/**********************************************************************************/
/* functions */
/**********************************************************************************/
/*!
 *
 * @brief       : API to initiatialize APP3.0 EEPROM
 */

void app30_eeprom_init(void)
{
    ds28e05_Init();
}

/*!
 *
 * @brief       : API to read the internal ROM ID, and checks whether the read id is correct or not
 *
 */

bool app30_eeprom_romid(uint8_t *buffer)
{
    if (buffer != NULL)
    {

        return ds28e05_read_rom_id(buffer);
    }
    else
    {
        return false;
    }
}
/*!
 *
 * @brief       : API to read APP3.0 EEPROM
 */
bool app30_eeprom_read(uint16_t address, uint8_t *buffer, uint8_t length)
{
    if (buffer != NULL)
    {
        uint8_t i;

        if (ds28e05_reset() == 0)
            return 0;
        /* Skip Rom command, sending this command is compulsory as per the 1-wire protocol */
        ds28e05_writebyte(DS28E05_SKIP_ROM_CMD);

        ds28e05_writebyte(DS28E05_READ_MEMORY_CMD);

        ds28e05_writebyte(address);
        ds28e05_writebyte(0x00);

        for (i = 0; i < length; i++)
            buffer[i] = ds28e05_readbyte();

        return 1;
    }
    else
    {
        return false;
    }
}
/*!
 *
 * @brief       : API to write APP3.0 EEPROM
 */
bool app30_eeprom_write(uint8_t address, uint8_t *buffer, uint8_t length)
{
    if (buffer != NULL)

    {
        uint8_t databyte1, databyte2;
        uint8_t sf, ef;
        uint8_t is_command_success;
        uint8_t sb[2] = { 0 };
        uint8_t eb[2] = { 0 };
        uint8_t spage, epage, npage, wpage;
        uint8_t nseg, wseg = 0;
        uint8_t wBytes = 0, rBytes = 0, wAddr = 0;

        /* Calculate pages */
        spage = (address & DS28E05_PAGE_MASK) >> 4;
        epage = ((address + length) & DS28E05_PAGE_MASK) >> 4;
        if (epage == DS28E05_NUM_PAGES)
            epage = DS28E05_NUM_PAGES - 1;
        npage = epage - spage;

        /* This memory must be written respecting 16bits boundaries */
        sf = (address & 0x01) != 0;
        ef = ((address + length) & 0x01) != 0;

        if (ef)
        {
            app30_eeprom_read(address + length, &eb[1], 1);
            eb[0] = buffer[length - 1];
            length++;
        }
        if (sf)
        {
            app30_eeprom_read(address - 1, &sb[0], 1);
            sb[1] = buffer[0];
            length++;
            address--;
        }

        /* Write pages */
        for (wpage = 0; wpage <= npage; wpage++)
        {
            wAddr = address + wBytes;
            /* Calculate segments to write */
            if ((length - wBytes) > (DS28E05_BYTES_PER_PAGE))
                /* Will we pass a page boundary */
                if (wAddr % (DS28E05_SEG_SIZE * DS28E05_BYTES_PER_SEG) == 0)
                    nseg = DS28E05_SEG_SIZE;
                else
                    nseg = (DS28E05_BYTES_PER_PAGE - (wAddr % (DS28E05_BYTES_PER_PAGE))) >> 1;
            else
                nseg = ((length - wBytes) & DS28E05_SEG_MASK) >> 1;

            if (ds28e05_reset() == 0)
                return 0;
            /* Skip Rom command, sending this command is compulsory as per the 1-wire protocol */
            ds28e05_writebyte(DS28E05_SKIP_ROM_CMD);

            ds28e05_writebyte(DS28E05_WRITE_MEMORY_CMD);
            ds28e05_writebyte(wAddr);
            ds28e05_writebyte(0xff);
            /* Write segments within page */
            for (wseg = 0; wseg < nseg; wseg++)
            {
                if (sf)
                {
                    ds28e05_writebyte(sb[0]);
                    ds28e05_writebyte(sb[1]);
                    wBytes += 2;
                    rBytes++;
                    sf = 0;
                }
                else if (ef && (length - wBytes) <= 2)
                {
                    ds28e05_writebyte(eb[0]);
                    ds28e05_writebyte(eb[1]);
                    wBytes += 2;
                    rBytes++;
                    ef = 0;
                }
                else
                {
                    ds28e05_writebyte(buffer[rBytes]);
                    ds28e05_writebyte(buffer[rBytes + 1]);
                    wBytes += 2;
                    rBytes += 2;
                }

                databyte1 = ds28e05_readbyte();
                databyte2 = ds28e05_readbyte();

                ds28e05_writebyte(0xff);

                ds28e05_msDelay(DS28E05_tPROG);

                is_command_success = ds28e05_readbyte();

                if (is_command_success != DS28E05_COMMAND_SUCCESS)
                {
                    ds28e05_reset();
                    return 0;
                }
            }

            ds28e05_reset();
        }
        (void)databyte1;
        (void)databyte2;
        return 1;
    }
    else
    {
        return false;
    }
}


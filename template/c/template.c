/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    template.c
 * @date    Oct-15-2019
 * @brief   Template project for COINES example
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 *
 */

#include <stdio.h>
#include "coines.h"

int main()
{
    int rslt = coines_open_comm_intf(COINES_COMM_INTF_USB); // Connect to Application board
    if( rslt < 0 )
    {
        printf("Unable to connect to Application Board !\n");
        return rslt;
    }

    // Some test C code to get Application Board information
    struct coines_board_info data = {};
    rslt = coines_get_board_info(&data);

    if (rslt == COINES_SUCCESS)
    {
        printf("Hardware - v%d.%d \n", (data.hardware_id & 0xF0) >> 4 , data.hardware_id & 0x0F);
        printf("Software - v%d.%d \n", (data.software_id & 0xF0) >> 4 , data.software_id & 0x0F);
        printf("Type of board - %d \n", data.board );
        printf("Shuttle ID - 0x%x\n", data.shuttle_id);
    }

    // Your C code goes here ...
    // Refer doc/BST-COINES-SD001.pdf for information regarding COINES API.
    // Use COINES with sensorAPI to play with our sensors.
    // Get sensorAPI from https://github.com/BoschSensortec

    coines_close_comm_intf(COINES_COMM_INTF_USB);
    return 0;
}

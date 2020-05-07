/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *  
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    mtp_config.h
 * @date    June 24, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief   MTP stack configuration file
 */
#ifndef MTP_CONFIG_H_
#define MTP_CONFIG_H_

#define MTP_BUFF_SIZE 1024

#define MTP_STR_MANUFACTURER_NAME "Bosch Sensortec GmbH"
#define MTP_STR_MODEL_NAME "APP3.0 Board"
#define MTP_STR_DEVICE_VERSION "1.1"
#define MTP_STR_DEVICE_SERIAL "30031993"

#define MTP_STR_VOLUME_ID "W25M02 External Memory"
#define MTP_STR_STORAGE_DESCRIPTOR MTP_STR_VOLUME_ID
#define MTP_STR_DEVICE_FRIENDLY_NAME MTP_STR_MODEL_NAME

#define MTP_MAX_NUM_OF_FILES 128
/**********************************************************************************/
/* function declarations */
/**********************************************************************************/
/*!
 *
 * @brief       : API to start FlogFS
 *
 * @param[in]   : None
 *
 * @return      : None
 */
void flogfs_glue_start();
/*!
 *
 * @brief       : API to start DummyFS
 *
 * @param[in]   : None
 *
 * @return      : None
 */
void dummyfs_start();

#endif /* MTP_CONFIG_H_ */

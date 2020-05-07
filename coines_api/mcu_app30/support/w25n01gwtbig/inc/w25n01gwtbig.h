/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file        w25n01gw.h
 *
 * @brief		Header file for w25n01gw
 *
 *
 */

/*!
 * @addtogroup interpreter
 * @brief
 * @{*/

#ifndef DRIVERS_W25N01GW_H_
#define DRIVERS_W25N01GW_H_

#include <stdint.h>
#include <stddef.h>

#ifdef _cplusplus
extern "C"
{
#endif

#define W25N01GW_NO_OF_SEC		4 //No.of Sectors per page
#define W25N01GW_NO_OF_PAGES	64//No.of Pages per block
#define W25N01GW_NO_OF_BLOCKS	1024//No.of Blocks in variant W25N01GWTBIG
#define W25N01GW_SECTOR_SIZE	512 //Size of the sector
#define W25N01GW_PAGE_SIZE 		(W25N01GW_NO_OF_SEC*W25N01GW_SECTOR_SIZE) //Size of the page
#define W25N01GW_BLOCK_SIZE 	(W25N01GW_NO_OF_PAGES*W25N01GW_PAGE_SIZE) //Size of the block
#define W25N01GW_FLASH_SIZE		(W25N01GW_BLOCK_SIZE*W25N01GW_NO_OF_BLOCKS) //Size of the flash
#define W25N01GW_TOTAL_SECTORS	(W25N01GW_NO_OF_SEC*W25N01GW_NO_OF_PAGES*W25N01GW_NO_OF_BLOCKS) //Total no. of sectors
#define W25N01GW_SECTORS_PER_BLOCK	(W25N01GW_NO_OF_PAGES*W25N01GW_NO_OF_SEC) //No. of sectors per block

/*
 * @brief	Enum for defining W25N01GW errorCode
 */
typedef enum W25N01GW_errorCode_e
{
    W25N01GW_ERROR, /*< Error*/
    W25N01GW_ERR_LOCATION_INVALID, /*< invalid location */
    W25N01GW_ERR_BUFFER_INVALID, /*< invalid buffer */
    W25N01GW_ERR_BYTE_LEN_INVALID, /*< invalid Byte length */
    W25N01GW_UNINITIALIZED, /*< Uninitialized */
    W25N01GW_INITIALIZED, /*< Initialized */
    W25N01GW_INITIALIZATION_FAILED, /*< Intialization failed */
    W25N01GW_ERASE_SUCCESS, /*< Erase sucess */
    W25N01GW_ERASE_FAILURE, /*< Erase failure */
    W25N01GW_READ_SUCCESS, /*< Read sucess */
    W25N01GW_ECC_FAILURE, /*< ECC failure*/
    W25N01GW_WRITE_SUCCESS, /*< Wrire sucess */
    W25N01GW_WRITE_FAILURE, /*< Write failure*/
} W25N01GW_errorCode_t;
/**********************************************************************************/
/* header includes */
/**********************************************************************************/

/*
 * @brief	Enum for defining W25N01GW register enum type
 */
typedef enum W25N01GW_reg_enum_type
{
    W25N01GW_PROTECT_REG_ADDR = 0xA0,/*< protect register address*/
    W25N01GW_CONFIG_REG_ADDR = 0xB0,/*< configure register address*/
    W25N01GW_STATUS_REG_ADDR = 0xC0/*< status register address*/

} W25N01GW_reg_t;

/**
 * @brief structure to hold the flash device information
 */
typedef struct W25N01GW_deviceInfo_s
{
    /**
     * @brief Manufacturer Id
     */
    uint8_t mfgId;
    /**
     * @brief Device Id
     */
    uint16_t deviceId;

} W25N01GW_deviceInfo_t;

/**
 * @brief Structure to return the characteristics of the storage device
 */
typedef struct W25N01GW_memoryParams_s
{
    /**
     * @brief Total memory size of the storage device in bytes
     */
    uint32_t memorySize;
    /**
     * @brief Sector size
     */
    uint16_t sectorSize;
    /**
     * @brief Total number of Sectors in storage device
     */
    uint32_t noOfSectors;
    /**
     * @brief Erase block size in unit of pages
     */
    uint16_t eraseBlockUnits;
} W25N01GW_memoryParams_t;

/**********************************************************************************/
/* function declarations */
/**********************************************************************************/
/*!
 *
 * @brief       : API for initialize w25n01gw
 *
 * @param[in]   : void
 *
 * @return      : W25N01GW errorCode
 */
W25N01GW_errorCode_t W25N01GW_Init();
/*!
 *
 * @brief       : API to get device initialization status
 *
 * @param[in]   : void
 *
 * @return      : W25N01GW errorCode
 */
W25N01GW_errorCode_t W25N01GW_getDeviceInitStatus(void);
/*!
 *
 * @brief       : API to erase block
 *
 * @param[in]   : position,length
 *
 * @return      : W25N01GW errorCode
 */
W25N01GW_errorCode_t W25N01GW_eraseBlock(uint32_t pos, uint32_t len);
/*!
 *
 * @brief       : API to read
 *
 * @param[in]   : data pointer, No of bytes to read and read location
 *
 * @return      : W25N01GW errorCode
 */
W25N01GW_errorCode_t W25N01GW_read(uint8_t* dataPtr, uint32_t noOfbytesToRead,
                                   uint32_t readLoc);
/*!
 *
 * @brief       : API to read spare
 *
 * @param[in]   : data pointer,No. of byes to read,page number and pageoff
 *
 * @return      : W25N01GW errorCode
 */
W25N01GW_errorCode_t W25N01GW_read_spare(uint8_t* dataPtr,
                                         int8_t noOfbytesToRead,
                                         uint16_t pageNum, uint16_t pageOff);
/*!
 *
 * @brief       : API to write the spare
 *
 * @param[in]   : data pointer,No. of byes to read,page number and pageoff
 *
 * @return      : W25N01GW errorCode
 */
W25N01GW_errorCode_t W25N01GW_write_spare(const uint8_t* dataPtr,
                                          uint8_t noOfbytesToWrite,
                                          uint16_t pageNum, uint16_t pageOff);
/*!
 *
 * @brief       : API to write
 *
 * @param[in]   : data pointer,No. of byes to read,page number and pageoff
 *
 * @return      : W25N01GW errorCode
 */
W25N01GW_errorCode_t W25N01GW_write(const uint8_t* dataPtr,
                                    uint32_t noOfbytesToWrite,
                                    uint32_t writeLoc);
/*!
 *
 * @brief       : API for read register
 *
 * @param[in]   : W25N01GW register
 *
 * @return      : buffer
 */
uint8_t W25N01GW_readReg(W25N01GW_reg_t reg);
/*!
 *
 * @brief       : API for write register
 *
 * @param[in]   : W25N01GW_reg_t reg, uint8_t reg_value
 *
 * @return      : None
 */
void W25N01GW_writeReg(W25N01GW_reg_t reg, uint8_t reg_value);
/*!
 *
 * @brief       : API to get memory parameters
 *
 * @param[in]   : W25N01GW_memoryParams_t* flashMemoryParams
 *
 * @return      : None
 */
void W25N01GW_getMemoryParams(W25N01GW_memoryParams_t* flashMemoryParams);
/*!
 *
 * @brief       : API to get manufacture and device ID
 *
 * @param[in]   : W25N01GW_deviceInfo_t* info
 *
 * @return      : None
 */
void W25N01GW_getManufactureAndDevId(W25N01GW_deviceInfo_t* info);

#ifdef _cplusplus
}
#endif

#endif /* DRIVERS_W25N01GW_H_ */

/** @}*/

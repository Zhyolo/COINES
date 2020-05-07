/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    mtp_support.h
 * @date    Jun 30, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief   Data structures and function definitions for MTP support
 */

#ifndef MTP_SUPPORT_H_
#define MTP_SUPPORT_H_

/**********************************************************************************/
/* header includes */
/**********************************************************************************/
#include <stdint.h>
#include <string.h>
/*!
 * @brief   MTP Operation Dataset.See 4.5.2 - Operation dataset in MTP Specification v1.1
 */
typedef struct
{
    uint32_t length; /*< length */
    uint16_t type; /*< type */
    uint16_t operation; /*< operation */
    uint32_t transaction_id; /*< transaction ID */
    uint32_t parameter[5]; /*< parameter */
}__attribute__((packed)) mtp_container_t;

/*!
 * @brief   File object representation
 */
typedef struct
{
    uint32_t handle; /*< handle*/
    char name[32]; /*< file name */
    char date_created[15]; /*< created date */
    char date_modified[15]; /*< modified date */
    uint32_t size; /*< size */
    void *content; /*< content */
} mtp_file_object_t;

/*!
 * @brief   File system capacity information
 */
typedef struct
{
    uint64_t capacity; /*< total capacity */
    uint64_t free_space; /*< free space */
} mtp_storage_info_t;

/*!
 * @brief   MTP protocol - file system interface handlers
 */
typedef struct
{
    int32_t (*fs_init)(); /*< MTP OpenSession callback (D.2.2)*/
    int32_t (*fs_format)(); /*< MTP FormatStore callback (D.2.15)*/
    int32_t (*fs_get_storage_info)(mtp_storage_info_t * storage_info); /*< MTP GetStorageInfo callback (D.2.5)*/
    int32_t (*fs_get_object_handles)(uint32_t *handles, uint32_t *len); /*< MTP GetObjectHandles callback (D.2.7)*/
    int32_t (*fs_get_object_info_by_handle)(uint32_t handle, mtp_file_object_t *fs_object); /*< MTP GetObjectInfo callback (D.2.8)*/
    int32_t (*fs_file_read)(uint32_t handle, uint32_t offset, uint8_t *buff, uint32_t len); /*< MTP GetObject callback (D.2.9)*/
    int32_t (*fs_file_delete)(uint32_t handle); /*< MTP DeleteObject callback (D.2.11)*/
    int32_t (*fs_send_object_info_by_handle)(uint32_t *handle, mtp_file_object_t *fs_object); /*< MTP SendObjectInfo callback (D.2.12)*/
    int32_t (*fs_file_write)(uint32_t handle, uint32_t offset, uint8_t *buff, uint32_t len); /*< MTP SendObject callback (D.2.13)*/
    int32_t (*fs_deinit)(); /*< MTP CloseSession callback (D.2.3)*/
    int32_t (*fs_pending_task)(); /*< Do any deferred/pending tasks in FS*/
} mtp_operation_t;

/**********************************************************************************/
/* MTP support functions */
/**********************************************************************************/
/*!
 *  @brief    Registers filesystem operations to the MTP stack
 *
 *
 *  @param[in]   : mtp_operation_t *mtp_fs_op
 *
 *  @return      : None
 */
void mtp_register_handlers(mtp_operation_t *mtp_fs_op);
/*!
 *  @brief    MTP command receive callback
 *
 *
 *  @param[in]   : mtp_container_t *container
 *
 *  @return      : None
 */
void mtp_packet_process_cb(mtp_container_t *container);
/*!
 *  @brief    Put uint64_t into global array - mtp_transmit_buff
 *
 *
 *  @param[in]   : value
 *
 *  @return      : None
 */
void mtp_buff_put_u64(uint64_t value);
/*!
 *  @brief      Put uint32_t into global array - mtp_transmit_buff
 *
 *
 *  @param[in]   : value
 *
 *  @return      : None
 */
void mtp_buff_put_u32(uint32_t value);
/*!
 *  @brief    Put uint16_t into global array - mtp_transmit_buff
 *
 *
 *  @param[in]   : value
 *
 *  @return      : None
 */
void mtp_buff_put_u16(uint16_t value);
/*!
 *  @brief    Put uint8_t into global array - mtp_transmit_buff
 *
 *
 *  @param[in]   : value
 *
 *  @return      : None
 */
void mtp_buff_put_u8(uint8_t value);
/*!
 *  @brief    Put byte array into global array - mtp_transmit_buff
 *
 *
 *  @param[in]   : value,length
 *
 *  @return      : None
 */
void mtp_buff_put_byte_array(uint8_t *value, uint32_t len);
/*!
 *  @brief    Put string into global array - mtp_transmit_buff (uint8_t length and UTF-16LE string)
 *
 *
 *  @param[in]   : char *string
 *
 *  @return      : None
 */
void mtp_buff_put_string(char *string);

uint64_t mtp_buff_get_u64();
uint32_t mtp_buff_get_u32();
uint16_t mtp_buff_get_u16();
uint8_t mtp_buff_get_u8();
/*!
 *  @brief    Read byte array from global variable - mtp_receive_buff
 *
 *
 *  @param[in]   : uint8_t *value, length
 *
 *  @return      : None
 */
void mtp_buff_get_byte_array(uint8_t *value, uint32_t len);
/*!
 *  @brief    Read UTF-16 string from global variable - mtp_receive_buff
 *
 *
 *  @param[in]   : char *string
 *
 *  @return      : None
 */
void mtp_buff_get_string(char *string);
/*!
 *  @brief Create new file and return MTP handle
 *
 *
 *  @param[in]   : uint32_t *handle
 *
 *  @return      : None
 */
void mtp_buff_get_file_info_by_handle(uint32_t *handle);
/*!
 *  @brief    Get file information given the MTP handle
 *
 *
 *  @param[in]   : handle
 *
 *  @return      : None
 */
void mtp_buff_put_file_info_by_handle(uint32_t handle);
/*!
 *  @brief    Send file to MTP initiator (USB Host) provided the handle
 *
 *
 *  @param[in]   : handle
 *
 *  @return      : length
 */
int32_t mtp_transmit_file(uint32_t handle);
/*!
 *  @brief    Receive file from MTP initiator (USB Host) given the MTP handle
 *
 *
 *  @param[in]   : handle
 *
 *  @return      : length
 */
int32_t mtp_receive_file(uint32_t handle);
/*!
 *  @brief  Send file (array) to MTP initiator (USB Host)
 *
 *
 *  @param[in]   : uint8_t *file_data,file_size
 *
 *  @return      : None
 */
void mtp_transmit_mem_file(uint8_t *file_data, uint32_t file_size);
/*!
 *  @brief  Call this API after forming the MTP packet
 *
 *
 *  @param[in]   : None
 *
 *  @return      : None
 */
void mtp_transmit_packet();
/*!
 *  @brief    MTP Task
 *
 *
 *
 *  @return      : None
 */
void mtp_loop();

#endif

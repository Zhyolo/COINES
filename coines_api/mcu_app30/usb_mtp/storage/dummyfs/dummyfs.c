/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    dummyfs.c
 * @date    Jun 30, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief   Dummy file system for testing MTP feature
 */

#include <stdint.h>
#include <string.h>
#include "mtp.h"
#include "mtp_support.h"
#include "dummyfs.h"

mtp_operation_t dummyfs_op = {
        .fs_init = dummyfs_init,
        .fs_format = dummyfs_format,
        .fs_get_storage_info = dummyfs_get_storage_info,
        .fs_get_object_handles = dummyfs_get_object_handles,
        .fs_get_object_info_by_handle = dummyfs_get_object_info_by_handle,
        .fs_file_read = dummyfs_file_read,
        .fs_file_delete = dummyfs_file_delete,
        .fs_send_object_info_by_handle = dummyfs_send_object_info_by_handle,
        .fs_file_write = dummyfs_file_write,
        .fs_deinit = dummyfs_deinit,
};

extern uint32_t _binary_storage_dummyfs_content_README_txt_start;
extern uint32_t _binary_storage_dummyfs_content_ReleaseNotes_COINES_pdf_start;
extern uint32_t _binary_storage_dummyfs_content_README_txt_size;
extern uint32_t _binary_storage_dummyfs_content_ReleaseNotes_COINES_pdf_size;

/*MTP Test Data*/
static uint32_t num_of_files = 5;

static mtp_file_object_t mtp_file_obj[5] = {
        [0] = {
        .name = "flash_mem_backup.bin",
        .size = 768*1024,
        .date_created = "20190622T220030",
        .date_modified = "20190622T220030",
        .content  = (uint8_t *)0x30000,
        },

        [1] = {
        .name = "usb_dfu_bootloader.bin",
        .size = 16*1024,
        .date_created = "20190530T170022",
        .date_modified = "20190530T170022",
        .content = (uint8_t *)0xF0000,
        },

        [2] = {
        .name = "nrf52840_ble_sd.bin",
        .size = 192*1024,
        .date_created = "20190530T170022",
        .date_modified = "20190530T170022",
        .content = (uint8_t *)0xF0000,
        },

        [3] = {
        .name = "ReleaseNotes_COINES.pdf",
        .size = (uint32_t)&_binary_storage_dummyfs_content_ReleaseNotes_COINES_pdf_size,
        .date_created = "19930330T170022",
        .date_modified = "19930330T170022",
        .content = &_binary_storage_dummyfs_content_ReleaseNotes_COINES_pdf_start,
        },

        [4] = {
        .name = "README.txt",
        .size = (uint32_t )&_binary_storage_dummyfs_content_README_txt_size,
        .date_created = "20190701T103322",
        .date_modified = "20190701T103322",
        .content = &_binary_storage_dummyfs_content_README_txt_start,
        },

};
/**********************************************************************************/
/* functions */
/**********************************************************************************/
/*!
 *
 * @brief       : Handler to start Dummy FS
 *
 */
void dummyfs_start()
{
    mtp_register_handlers(&dummyfs_op);
}
/*!
 *
 * @brief       : Handler to initialize Dummy FS
 *
 */
int32_t dummyfs_init()
{
    return 0;
}
/*!
 *
 * @brief       : Handler to format Dummy FS
 */
int32_t dummyfs_format()
{
    return -1;
}
/*!
 *
 * @brief       : Handler to get storage information
 *
 */
int32_t dummyfs_get_storage_info(mtp_storage_info_t * storage_info)
{
    storage_info->capacity = 128 * 1024 * 1024;
    storage_info->free_space = 22 * 1024 * 1024;
    return 0;
}
/*!
 *
 * @brief       : Handler to get object handles
 */
int32_t dummyfs_get_object_handles(uint32_t *handles, uint32_t *len)
{
    for (int i = 1; i <= num_of_files; i++)
        handles[i - 1] = i;
    *len = num_of_files;
    return 0;
}
/*!
 *
 * @brief       : Handler to get object information by handle
 *
 */
int32_t dummyfs_get_object_info_by_handle(uint32_t handle,
                                          mtp_file_object_t *fs_object)
{
    memcpy(fs_object, &mtp_file_obj[handle - 1], sizeof(mtp_file_object_t));
    return 0;
}
/*!
 *
 * @brief       : Handler to read Dummy FS file
 *
 */
int32_t dummyfs_file_read(uint32_t handle, uint32_t offset, uint8_t *buff,
                          uint32_t len)
{
    if (mtp_file_obj[handle - 1].content != NULL && buff != NULL)
    {
        memcpy(buff, mtp_file_obj[handle - 1].content + offset, len);
        return len;
    }
    else
        return 0;
}
/*!
 *
 * @brief       : Handler to delete Dummy FS file
 */
int32_t dummyfs_file_delete(uint32_t handle)
{
    return -1;
}
/*!
 *
 * @brief       : Handler to send object information by handle
 */
int32_t dummyfs_send_object_info_by_handle(uint32_t *handle,
                                           mtp_file_object_t *fs_object)
{
    return -1;
}
/*!
 *
 * @brief       : Handler to write dummy file
 */
int32_t dummyfs_file_write(uint32_t handle, uint32_t offset, uint8_t *buff,
                           uint32_t len)
{
    return 0;
}
/*!
 *
 * @brief       : Handler to deinitialize Dummy FS
 *
 */
int32_t dummyfs_deinit()
{
    return 0;
}

/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    flogfs_glue.c
 * @date    Jun 30, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief
 */
/**********************************************************************************/
/* header includes */
/**********************************************************************************/
#include <stdint.h>
#include <string.h>
#include "mtp.h"
#include "mtp_support.h"
#include "mtp_config.h"
#include "flogfs.h"
#include "flogfs_glue.h"
#include "w25n01gwtbig.h"

mtp_operation_t flogfs_glue_op = {
		.fs_init = flogfs_glue_init,
		.fs_format = flogfs_glue_format,
		.fs_get_storage_info = flogfs_glue_get_storage_info,
		.fs_get_object_handles = flogfs_glue_get_object_handles,
		.fs_get_object_info_by_handle = flogfs_glue_get_object_info_by_handle,
		.fs_file_read = flogfs_glue_file_read,
		.fs_file_delete = flogfs_glue_file_delete,
		.fs_send_object_info_by_handle = flogfs_glue_send_object_info_by_handle,
		.fs_file_write = flogfs_glue_file_write,
		.fs_deinit = flogfs_glue_deinit,
		.fs_pending_task = flogfs_glue_delete_invalid_file_block,
};


mtp_file_object_t file_obj[MTP_MAX_NUM_OF_FILES];

int num_of_files = 0;
uint64_t free_space = STORAGE_CAPACITY;
bool pending_delete = false;
/*!
 *
 * @brief       : Handler to create default files after format
 *
 * @param[in]   : None
 *
 * @return      : None
 */
static void create_default_files_after_format()
{
    /*Variable names needs to be changed if path of README.txt is changed*/
    extern uint32_t _binary_storage_flogfs_w25m02_README_TXT_start;
    extern uint32_t _binary_storage_flogfs_w25m02_README_TXT_size;

    flog_write_file_t write_file;
    flogfs_open_write(&write_file, "README.TXT");
    flogfs_write(&write_file,
                 (uint8_t *)&_binary_storage_flogfs_w25m02_README_TXT_start,
                 (uint32_t) & _binary_storage_flogfs_w25m02_README_TXT_size);
    flogfs_close_write(&write_file);

}

/*!
 *
 * @brief       : Handler to start FlogFS
 */
void flogfs_glue_start()
{
    mtp_register_handlers(&flogfs_glue_op);
}
/*!
 *
 * @brief       : Handler to initialize FlogFS
 *
 * @param[in]   : None
 *
 * @return      : 0
 */
int32_t flogfs_glue_init()
{

    flog_initialize_params_t params = {
        .number_of_blocks = 1024,
        .pages_per_block = 64,
    };
    flogfs_initialize(&params);

    flogfs_mount();

    return 0;
}
/*!
 *
 * @brief       : Handler for formatting flash memory as FlogFS
 *
 * @param[in]   : None
 *
 * @return      : 0 or 1
 */
int32_t flogfs_glue_format()
{
    int error_code;
    flog_initialize_params_t params = {
        .number_of_blocks = 1024,
        .pages_per_block = 64,
    };
    flogfs_initialize(&params);
    for (uint16_t i = 0; i < 1024; i++)
    {
        W25N01GW_eraseBlock((i * W25N01GW_BLOCK_SIZE) + 1, W25N01GW_BLOCK_SIZE);
    }
    error_code = flogfs_format();

    flogfs_mount();
    create_default_files_after_format();

    if (error_code == FLOG_SUCCESS)
        return 0;
    else
        return 1;

}
/*!
 *
 * @brief       : Handler to get storage information of flash memory
 *
 * @param[in]   : mtp_storage_info_t * storage_info
 *
 * @return      : 0
 */
int32_t flogfs_glue_get_storage_info(mtp_storage_info_t * storage_info)
{
    storage_info->capacity = STORAGE_CAPACITY;
    storage_info->free_space =  flogfs_available_space();
    return 0;
}
/*!
 *
 * @brief       : Handler to get object handles of files
 *
 * @param[in]   : *handles, *len
 *
 * @return      : 0
 */
int32_t flogfs_glue_get_object_handles(uint32_t *handles, uint32_t *len)
{

    num_of_files = 0;
    flogfs_ls_iterator_t iter;
    flogfs_start_ls(&iter);

    /*Populate file names,size and handle in file object*/
    while (flogfs_ls_iterate(&iter, file_obj[num_of_files].name))
    {
        file_obj[num_of_files].handle = num_of_files + 1;
        flog_read_file_t read_file;
        flogfs_open_read(&read_file, file_obj[num_of_files].name);
        file_obj[num_of_files].size = flogfs_read_file_size(&read_file);
        flogfs_close_read(&read_file);
        num_of_files++;
    }

    flogfs_stop_ls(&iter);

    for (int i = 1; i <= num_of_files; i++)
        handles[i - 1] = i;
    *len = num_of_files;
    return 0;
}
/*!
 *
 * @brief       : Handler to get object information by handle
 *
 * @param[in]   : handle,mtp_file_object_t *fs_object
 *
 * @return      : 0
 */
int32_t flogfs_glue_get_object_info_by_handle(uint32_t handle, mtp_file_object_t *fs_object)
{
    memcpy(fs_object, &file_obj[handle - 1], sizeof(mtp_file_object_t));
    return 0;
}
/*!
 *
 * @brief       : Handler to read file by object handle
 *
 * @param[in]   : handle,mtp_file_object_t *fs_object
 *
 * @return      : Number of bytes read
 */
int32_t flogfs_glue_file_read(uint32_t handle, uint32_t offset, uint8_t *buff, uint32_t len)
{
    static flog_read_file_t read_file;

    if(offset == 0)  flogfs_open_read(&read_file,file_obj[handle-1].name);

    if(offset < file_obj[handle-1].size)  return flogfs_read(&read_file,buff,len);

    if(offset == file_obj[handle-1].size)  flogfs_close_read(&read_file);

    return 0;
}
/*!
 *
 * @brief       : Handler to delete a file
 *
 * @param[in]   : handle
 *
 * @return      : 0 or -1
 */
int32_t flogfs_glue_file_delete(uint32_t handle)
{
    if (pending_delete == false)
    {
        if (flogfs_invalidate(file_obj[handle - 1].name) == FLOG_SUCCESS)
        {
            pending_delete = true;
            return 0;
        }
        else
            return -1;
    }
    else
        return -1;
}
/*!
 *
 * @brief       : Handler to send the object information by handle
 *
 * @param[in]   : handle
 *
 * @return      : 0
 */
int32_t flogfs_glue_send_object_info_by_handle(uint32_t *handle, mtp_file_object_t *fs_object)
{
    *handle = ++num_of_files;
    memcpy(&file_obj[*handle - 1], fs_object, sizeof(mtp_file_object_t));
    return 0;
}
/*!
 *
 * @brief       : Handler to send the object information by handle
 *
 * @param[in]   : handle,offset,*buff,len
 *
 * @return      : Number of bytes written
 */
int32_t flogfs_glue_file_write(uint32_t handle, uint32_t offset, uint8_t *buff, uint32_t len)
{
    static flog_write_file_t write_file;

    if(offset == 0)  flogfs_open_write(&write_file,file_obj[handle-1].name);

    if(offset < file_obj[handle-1].size)  return flogfs_write(&write_file,buff,len);

    if(offset == file_obj[handle-1].size) flogfs_close_write(&write_file);

    return 0;
}
/*!
 *
 * @brief       : Handler to deinitialize filesystem
 *
 * @param[in]   : None
 *
 * @return      : 0
 */
int32_t flogfs_glue_deinit()
{
    return 0;
}

/*!
 *
 * @brief       : Handler to delete invalidated file block
 *
 * @param[in]   : None
 *
 * @return      : 0
 */
int32_t flogfs_glue_delete_invalid_file_block(void)
{
    if (pending_delete == true)
    {
        flog_delete_invalidated_block();
        pending_delete = false;
        return 0;
    }
    else
        return -1;
}

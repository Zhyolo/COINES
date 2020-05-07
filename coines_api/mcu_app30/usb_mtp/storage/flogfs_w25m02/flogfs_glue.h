/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    flogfs_glue.h
 * @date    Jun 30, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief
 */

#ifndef FLOGSFS_H_
#define FLOGSFS_H_

#include<stdint.h>

void flogfs_glue_start();
int32_t flogfs_glue_init();
int32_t flogfs_glue_format();
int32_t flogfs_glue_get_storage_info(mtp_storage_info_t * storage_info);
int32_t flogfs_glue_get_object_handles(uint32_t *handles,uint32_t *len);
int32_t flogfs_glue_get_object_info_by_handle(uint32_t handle,mtp_file_object_t *fs_object);
int32_t flogfs_glue_file_read(uint32_t handle,uint32_t offset,uint8_t *buff,uint32_t len);
int32_t flogfs_glue_file_delete(uint32_t handle);
int32_t flogfs_glue_send_object_info_by_handle(uint32_t *handle,mtp_file_object_t *fs_object);
int32_t flogfs_glue_file_write(uint32_t handle,uint32_t offset,uint8_t *buff,uint32_t len);
int32_t flogfs_glue_deinit();
int32_t flogfs_glue_delete_invalid_file_block(void);

#define STORAGE_CAPACITY (128*1024*1024)

#endif /* FLOGSFS_H_ */

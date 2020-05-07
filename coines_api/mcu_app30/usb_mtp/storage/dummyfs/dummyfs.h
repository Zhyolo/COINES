/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    dummyfs.h
 * @date    Jun 30, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief   Definitions for Dummy file system
 */

#ifndef DUMMYFS_DUMMYFS_H_
#define DUMMYFS_DUMMYFS_H_

/*!
 *
 * @brief       : API to start dummyfs
 *
 * @param[in]   : None
 *
 * @return      : 0
 */
void dummyfs_start();
/*!
 *
 * @brief       : API to initiate dummyfs
 *
 * @param[in]   : None
 *
 * @return      : 0
 */
int32_t dummyfs_init();
/*!
 *
 * @brief       : API to format dummyfs
 *
 * @param[in]   : None
 *
 * @return      : -1
 */
int32_t dummyfs_format();
/*!
 *
 * @brief       : API to get storage information
 *
 * @param[in]   : mtp_storage_info_t * storage_info
 *
 * @return      : 0
 */
int32_t dummyfs_get_storage_info(mtp_storage_info_t * storage_info);
/*!
 *
 * @brief       : API to get object handles
 *
 * @param[in]   : uint32_t *handles,uint32_t *len
 *
 * @return      : 0
 */
int32_t dummyfs_get_object_handles(uint32_t *handles, uint32_t *len);
/*!
 *
 * @brief       : API to get object information by handle
 *
 * @param[in]   : handle,mtp_file_object_t *fs_object
 *
 * @return      : 0
 */
int32_t dummyfs_get_object_info_by_handle(uint32_t handle,
                                          mtp_file_object_t *fs_object);
/*!
 *
 * @brief       : API to read dummyfs file
 *
 * @param[in]   : handle,offset,uint8_t *buff,len
 *
 * @return      : 0 or -1
 */
int32_t dummyfs_file_read(uint32_t handle, uint32_t offset, uint8_t *buff,
                          uint32_t len);
/*!
 *
 * @brief       : API to delete dummyfs file
 *
 * @param[in]   : handle
 *
 * @return      : -1
 */
int32_t dummyfs_file_delete(uint32_t handle);
/*!
 *
 * @brief       : API to send object information by handle
 *
 * @param[in]   : handle,mtp_file_object_t *fs_object
 *
 * @return      : 0
 */
int32_t dummyfs_send_object_info_by_handle(uint32_t *handle,
                                           mtp_file_object_t *fs_object);
/*!
 *
 * @brief       : API to write dummy file
 *
 * @param[in]   : handle,offset,uint8_t *buff,len
 *
 * @return      : 0
 */
int32_t dummyfs_file_write(uint32_t handle, uint32_t offset, uint8_t *buff,
                           uint32_t len);
/*!
 *
 * @brief       : API to deinitialize the dummyfs
 *
 * @param[in]   : handle,offset,uint8_t *buff,len
 *
 * @return      : 0
 */
int32_t dummyfs_deinit();

#endif /* STORAGE_DUMMYFS_DUMMYFS_H_ */

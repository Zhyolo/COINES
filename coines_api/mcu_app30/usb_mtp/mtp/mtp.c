/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    mtp.c
 * @date    Jun 22, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief   The minimal USB MTP stack (Doesn't implement the complete specification !)
 */

/**********************************************************************************/
/* header includes */
/**********************************************************************************/

#include "mtp.h"
#include "mtp_support.h"
#include "mtp_config.h"
#include "usb_core.h"

/*! MTP transmit and receive buffers */
uint8_t mtp_transmit_buff[MTP_BUFF_SIZE] = {};
uint8_t mtp_receive_buff[MTP_BUFF_SIZE] = {};
uint8_t mtp_receive_buff_temp[MTP_BUFF_SIZE] = {};

/*! Initial value transmit and receive buffer index */
uint16_t transmit_buffer_index = MTP_CONTAINER_HEADER_SIZE;
uint16_t receive_buffer_index = MTP_CONTAINER_HEADER_SIZE;

mtp_container_t mtp_container_cmd, mtp_container_rsp;
mtp_storage_info_t storage_info;

/*! Data transmit,receive status variables */
volatile bool transmit_complete_status = false;
volatile bool receive_complete_status = false;
volatile bool mtp_data_transfer_in_progress = false;

mtp_operation_t mtp_op;

/*! List of MTP operations supported by device */
uint16_t mtp_supported_operations[] = {
        MTP_OPERATION_GET_DEVICE_INFO,
        MTP_OPERATION_OPEN_SESSION,
        MTP_OPERATION_CLOSE_SESSION,
        MTP_OPERATION_GET_STORAGE_IDS,

        MTP_OPERATION_GET_STORAGE_INFO,
        MTP_OPERATION_GET_OBJECT_HANDLES,
        MTP_OPERATION_GET_OBJECT_INFO,

        MTP_OPERATION_GET_OBJECT,
        MTP_OPERATION_DELETE_OBJECT,
        MTP_OPERATION_SEND_OBJECT_INFO,
        MTP_OPERATION_SEND_OBJECT,

        MTP_OPERATION_FORMAT_STORE,
        MTP_OPERATION_GET_DEVICE_PROP_DESC,
};

/*!
 *  @brief Registers filesystem operations to the MTP stack
 */
void mtp_register_handlers(mtp_operation_t *mtp_fs_op)
{
    memcpy(&mtp_op, mtp_fs_op, sizeof(mtp_op));
}

/*!
 *  @brief Called when USB enumeration is complete
 */
void mtp_init()
{
    FS_START();
    transmit_complete_status = false;
    receive_complete_status = false;
    mtp_data_transfer_in_progress = false;
    receive_data_from_host(mtp_receive_buff_temp, MTP_BUFF_SIZE);
}

/*!
 *  @brief USB Bulk OUT transaction completion callback
 */
void usb_receive_complete_cb()
{
    if (mtp_data_transfer_in_progress == false)
        memcpy(&mtp_container_cmd, &mtp_receive_buff_temp[0], 32);

    memcpy(mtp_receive_buff, mtp_receive_buff_temp, MTP_BUFF_SIZE);
    receive_complete_status = true;

    if (mtp_data_transfer_in_progress == false)
        receive_data_from_host(mtp_receive_buff_temp, MTP_BUFF_SIZE);
}

/*!
 *  @brief USB Bulk IN transaction completion callback
 */
void usb_transmit_complete_cb()
{
    transmit_complete_status = true;
}

/*!
 *  @brief MTP Task
 */
void mtp_loop()
{
    while (1)
    {
        if (receive_complete_status == true
            && mtp_data_transfer_in_progress == false)
        {
            mtp_packet_process_cb(&mtp_container_cmd);
            receive_complete_status = false;
        }
        else if (receive_complete_status == true
                 && mtp_data_transfer_in_progress == true)
        {

        }

        if (mtp_op.fs_pending_task != NULL)
            mtp_op.fs_pending_task();
    }
}

/*!
 *  @brief MTP command receive callback
 */
void mtp_packet_process_cb(mtp_container_t *container)
{
    memset(&mtp_container_rsp, 0, sizeof(mtp_container_rsp));
    transmit_buffer_index = MTP_CONTAINER_HEADER_SIZE;
    receive_buffer_index = MTP_CONTAINER_HEADER_SIZE;
    uint8_t param_len = 0;
    static uint32_t file_handle,len,handles[MTP_MAX_NUM_OF_FILES];

    if (container->length >= MTP_CONTAINER_HEADER_SIZE)
    {
        mtp_container_rsp.operation = MTP_RESPONSE_OK;
        mtp_container_rsp.transaction_id = container->transaction_id;

        switch (container->operation)
        {
            case MTP_OPERATION_GET_DEVICE_INFO:
                mtp_buff_put_u16(MTP_STANDARD_VERSION);
                mtp_buff_put_u32(6);
                mtp_buff_put_u16(MTP_STANDARD_VERSION);
                mtp_buff_put_string("microsoft.com: 1.0;bosch-sensortec.com: 1.0;");
                mtp_buff_put_u16(0);
                mtp_buff_put_u32(sizeof(mtp_supported_operations) / 2);
                for (int i = 0; i < sizeof(mtp_supported_operations) / 2; i++)
                    mtp_buff_put_u16(mtp_supported_operations[i]);
                mtp_buff_put_u32(0);

                mtp_buff_put_u32(1); // Device properties (array of uint16)
                mtp_buff_put_u16(MTP_DEVICE_PROPERTY_DEVICE_FRIENDLY_NAME);

                mtp_buff_put_u32(0);       // Capture formats (array of uint16)

                mtp_buff_put_u32(1);       // Playback formats (array of uint16)
                mtp_buff_put_u16(MTP_FORMAT_UNDEFINED);

                mtp_buff_put_string(MTP_STR_MANUFACTURER_NAME);
                mtp_buff_put_string(MTP_STR_MODEL_NAME);
                mtp_buff_put_string(MTP_STR_DEVICE_VERSION);
                mtp_buff_put_string(MTP_STR_DEVICE_SERIAL);
                mtp_transmit_packet();
                break;

            case MTP_OPERATION_OPEN_SESSION:
                mtp_op.fs_init();
                break;

            case MTP_OPERATION_CLOSE_SESSION:
                mtp_op.fs_deinit();
                break;

            case MTP_OPERATION_GET_STORAGE_IDS:
                mtp_buff_put_u32(1);
                mtp_buff_put_u32(1);
                mtp_transmit_packet();
                break;

            case MTP_OPERATION_GET_STORAGE_INFO:
                mtp_op.fs_get_storage_info(&storage_info);
                mtp_buff_put_u16(MTP_STORAGE_REMOVABLE_RAM);
                mtp_buff_put_u16(MTP_STORAGE_FILESYSTEM_FLAT);
                mtp_buff_put_u16(MTP_STORAGE_READ_WRITE);
                mtp_buff_put_u64(storage_info.capacity);
                mtp_buff_put_u64(storage_info.free_space);
                mtp_buff_put_u32(0xFFFFFFFF);
                mtp_buff_put_string(MTP_STR_STORAGE_DESCRIPTOR);
                mtp_buff_put_string(MTP_STR_VOLUME_ID);
                mtp_transmit_packet();
                break;

            case MTP_OPERATION_GET_OBJECT_HANDLES:
                if (container->parameter[1])
                    mtp_container_rsp.operation = MTP_RESPONSE_SPECIFICATION_BY_FORMAT_UNSUPPORTED;
                else
                {
                    mtp_op.fs_get_object_handles(handles, &len);
                    mtp_buff_put_u32(len);
                    for (uint32_t i = 0; i < len; ++i)
                        mtp_buff_put_u32(handles[i]);
                    mtp_transmit_packet();
                }
                break;

            case MTP_OPERATION_GET_OBJECT_INFO:
                mtp_buff_put_file_info_by_handle(container->parameter[0]);
                mtp_transmit_packet();
                break;

            case MTP_OPERATION_GET_OBJECT:
                if (mtp_transmit_file(container->parameter[0]) == 0)
                    mtp_container_rsp.operation = MTP_RESPONSE_OK;
                else
                    mtp_container_rsp.operation = MTP_RESPONSE_INCOMPLETE_TRANSFER;
                break;

            case MTP_OPERATION_DELETE_OBJECT:
                if (mtp_op.fs_file_delete(container->parameter[0]) == 0)
                    mtp_container_rsp.operation = MTP_RESPONSE_OK;
                else
                    mtp_container_rsp.operation = MTP_RESPONSE_ACCESS_DENIED;
                break;

            case MTP_OPERATION_SEND_OBJECT_INFO:
                receive_complete_status = false;
                while (receive_complete_status == false)
                    ;
                mtp_container_rsp.parameter[0] = 1;
                mtp_container_rsp.parameter[1] = 0xFFFFFFFF;
                mtp_buff_get_file_info_by_handle(&file_handle);
                mtp_container_rsp.parameter[2] = file_handle;
                param_len = 12;
                break;

            case MTP_OPERATION_SEND_OBJECT:
                if (mtp_receive_file(file_handle) == 0)
                    mtp_container_rsp.operation = MTP_RESPONSE_OK;
                else
                    mtp_container_rsp.operation = MTP_RESPONSE_STORE_NOT_AVAILABLE;
                break;

            case MTP_OPERATION_FORMAT_STORE:
                if (mtp_op.fs_format() == 0)
                    mtp_container_rsp.operation = MTP_RESPONSE_OK;
                else
                    mtp_container_rsp.operation = MTP_RESPONSE_DEVICE_BUSY;
                break;

            case MTP_OPERATION_GET_DEVICE_PROP_DESC:
                if(container->parameter[0] == MTP_DEVICE_PROPERTY_DEVICE_FRIENDLY_NAME)
                {
                    mtp_buff_put_u16(MTP_DEVICE_PROPERTY_DEVICE_FRIENDLY_NAME);
                    mtp_buff_put_u16(MTP_TYPE_STR);
                    mtp_buff_put_u8(0);
                    mtp_buff_put_string(MTP_STR_DEVICE_FRIENDLY_NAME);
                    mtp_buff_put_string(MTP_STR_DEVICE_FRIENDLY_NAME);
                    mtp_buff_put_u8(0);
                    mtp_transmit_packet();
                }
                break;

            default:
                break;

        }
        mtp_container_rsp.length = MTP_CONTAINER_HEADER_SIZE + param_len;
        mtp_container_rsp.type = MTP_CONTAINER_TYPE_RESPONSE;
    }
    else
    {
        mtp_container_rsp.operation = MTP_RESPONSE_UNDEFINED;
    }

    send_data_to_host((uint8_t *)&mtp_container_rsp, mtp_container_rsp.length);
}

/*!
 *  @brief Get file information given the MTP handle 
 */
void mtp_buff_put_file_info_by_handle(uint32_t handle)
{
    mtp_file_object_t file_object = {};
    mtp_op.fs_get_object_info_by_handle(handle, &file_object);

    mtp_buff_put_u32(1);
    mtp_buff_put_u16(0);
    mtp_buff_put_u16(0);
    mtp_buff_put_u32(file_object.size);
    mtp_buff_put_u16(0); // thumb format
    mtp_buff_put_u32(0); // thumb size
    mtp_buff_put_u32(0); // thumb width
    mtp_buff_put_u32(0); // thumb height
    mtp_buff_put_u32(0); // pix width
    mtp_buff_put_u32(0); // pix height
    mtp_buff_put_u32(0); // bit depth
    mtp_buff_put_u32(0xFFFFFFFF); // parent
    mtp_buff_put_u16(MTP_ASSOCIATION_TYPE_UNDEFINED); // association type
    mtp_buff_put_u32(0); // association description
    mtp_buff_put_u32(0);  // sequence number
    mtp_buff_put_string(file_object.name);
    mtp_buff_put_string(file_object.date_created);  // date created
    mtp_buff_put_string(file_object.date_modified);  // date modified
    mtp_buff_put_string("");  // keywords
}

/*!
 *  @brief Create new file and return MTP handle
 */
void mtp_buff_get_file_info_by_handle(uint32_t *handle)
{
    mtp_file_object_t file_object = {};

    mtp_buff_get_u32(); // storage
    mtp_buff_get_u16(); // format
    mtp_buff_get_u16();  // protection
    file_object.size = mtp_buff_get_u32(); // size
    mtp_buff_get_u16(); // thumb format
    mtp_buff_get_u32(); // thumb size
    mtp_buff_get_u32(); // thumb width
    mtp_buff_get_u32(); // thumb height
    mtp_buff_get_u32(); // pix width
    mtp_buff_get_u32(); // pix height
    mtp_buff_get_u32(); // bit depth
    mtp_buff_get_u32(); // parent
    mtp_buff_get_u16(); // association type
    mtp_buff_get_u32(); // association description
    mtp_buff_get_u32(); // sequence number
    mtp_buff_get_string(file_object.name);
    mtp_buff_get_string(file_object.date_created);
    mtp_buff_get_string(file_object.date_modified);
    mtp_buff_get_string(NULL);

    mtp_op.fs_send_object_info_by_handle(handle, &file_object);
}

/*!
 *  @brief Put byte array into global array - mtp_transmit_buff
 */
void mtp_buff_put_byte_array(uint8_t *value, uint32_t length)
{

    memcpy(&mtp_transmit_buff[transmit_buffer_index], value, length);
    transmit_buffer_index += length;

}

/*!
 *  @brief Put uint64_t into global array - mtp_transmit_buff
 */
void mtp_buff_put_u64(uint64_t value)
{
    *(uint32_t *)(&mtp_transmit_buff[transmit_buffer_index]) = value & 0xFFFFFFFF;
    transmit_buffer_index += 4;
    *(uint32_t *)(&mtp_transmit_buff[transmit_buffer_index]) = (value >> 32);
    transmit_buffer_index += 4;
}

/*!
 *  @brief Put uint32_t into global array - mtp_transmit_buff
 */
void mtp_buff_put_u32(uint32_t value)
{
    *(uint32_t *) (&mtp_transmit_buff[transmit_buffer_index]) = value;
    transmit_buffer_index += 4;
}

/*!
 *  @brief Put uint16_t into global array - mtp_transmit_buff
 */
void mtp_buff_put_u16(uint16_t value)
{
    *(uint16_t *) (&mtp_transmit_buff[transmit_buffer_index]) = value;
    transmit_buffer_index += 2;
}

/*!
 *  @brief Put uint8_t into global array - mtp_transmit_buff
 */
void mtp_buff_put_u8(uint8_t value)
{
    mtp_transmit_buff[transmit_buffer_index] = value;
    transmit_buffer_index += 1;
}

/*!
 *  @brief Put string into global array - mtp_transmit_buff (uint8_t length and UTF-16LE string)
 */
void mtp_buff_put_string(char *string)
{
    if (*string)
    {
        mtp_buff_put_u8(strlen(string) + 1);
        while (*string)
        {
            mtp_buff_put_u16(*string);
            ++string;
        }
        mtp_buff_put_u16(0);
    }
    else
        mtp_buff_put_u8(0);
}

/*!
 *  @brief Send file to MTP initiator (USB Host) provided the handle
 */
int32_t mtp_transmit_file(uint32_t handle)
{
    #define MTP_TRANSMIT_DATA(len) \
    do { \
    transmit_complete_status = false; \
    send_data_to_host(mtp_transmit_buff,len); \
    while(transmit_complete_status == false); \
    } while(0)

    mtp_file_object_t file_obj;
    uint32_t initial_packet_size, transmit_parts = 0, remaining = 0;
    int32_t length,rslt = -1;

    mtp_op.fs_get_object_info_by_handle(handle, &file_obj);

    if (file_obj.size + MTP_CONTAINER_HEADER_SIZE > MTP_BUFF_SIZE)
    {
        initial_packet_size = MTP_BUFF_SIZE;
        transmit_parts = (file_obj.size + MTP_CONTAINER_HEADER_SIZE - MTP_BUFF_SIZE) / MTP_BUFF_SIZE;
        remaining = (file_obj.size + MTP_CONTAINER_HEADER_SIZE) % MTP_BUFF_SIZE;
    }
    else
        initial_packet_size = file_obj.size + MTP_CONTAINER_HEADER_SIZE;

    *(uint32_t *)&mtp_transmit_buff[MTP_CONTAINER_LENGTH_OFFSET] = file_obj.size + MTP_CONTAINER_HEADER_SIZE ;
    *(uint16_t *)&mtp_transmit_buff[MTP_CONTAINER_TYPE_OFFSET] = MTP_CONTAINER_TYPE_DATA;
    *(uint16_t *)&mtp_transmit_buff[MTP_CONTAINER_CODE_OFFSET] = mtp_container_cmd.operation;
    *(uint32_t *)&mtp_transmit_buff[MTP_CONTAINER_TRANSACTION_ID_OFFSET] = mtp_container_cmd.transaction_id;

    length = mtp_op.fs_file_read(handle,0,&mtp_transmit_buff[MTP_CONTAINER_HEADER_SIZE],initial_packet_size-MTP_CONTAINER_HEADER_SIZE);
    MTP_TRANSMIT_DATA(length + MTP_CONTAINER_HEADER_SIZE);

    if (length != initial_packet_size - MTP_CONTAINER_HEADER_SIZE)
        goto mtp_file_read_fail;


    for(uint32_t i=1; i <= transmit_parts; i++)
    {
        length = mtp_op.fs_file_read(handle,i*MTP_BUFF_SIZE-MTP_CONTAINER_HEADER_SIZE,mtp_transmit_buff,MTP_BUFF_SIZE);
        MTP_TRANSMIT_DATA(length);

        if (length != MTP_BUFF_SIZE)
            goto mtp_file_read_fail;
    }

    if(remaining)
    {
        length = mtp_op.fs_file_read(handle,(transmit_parts+1)*MTP_BUFF_SIZE-MTP_CONTAINER_HEADER_SIZE,mtp_transmit_buff,remaining);
        MTP_TRANSMIT_DATA(length);

        if(length != remaining)
            goto mtp_file_read_fail;
    }

    rslt = 0;

mtp_file_read_fail:
    mtp_op.fs_file_read(handle, file_obj.size, mtp_transmit_buff, 0); /* Close file --> Offset = file size, length = 0*/

    return rslt;

}

/*!
 *  @brief Receive file from MTP initiator (USB Host) given the MTP handle
 */
int32_t mtp_receive_file(uint32_t handle)
{
    #define MTP_RECEIVE_DATA(len)\
    do { \
        receive_complete_status = false; \
        receive_data_from_host(mtp_receive_buff_temp,len); \
        while(receive_complete_status == false); \
    } while(0)\

    mtp_data_transfer_in_progress = true;
    mtp_file_object_t file_obj;
    uint32_t initial_packet_size, receive_parts = 0, remaining = 0;
    int32_t length,rslt = -1;

    mtp_op.fs_get_object_info_by_handle(handle, &file_obj);

    if (file_obj.size + MTP_CONTAINER_HEADER_SIZE > MTP_BUFF_SIZE)
    {
        initial_packet_size = MTP_BUFF_SIZE;
        receive_parts = (file_obj.size + MTP_CONTAINER_HEADER_SIZE - MTP_BUFF_SIZE) / MTP_BUFF_SIZE;
        remaining = (file_obj.size + MTP_CONTAINER_HEADER_SIZE) % MTP_BUFF_SIZE;
    }
    else
        initial_packet_size = file_obj.size + MTP_CONTAINER_HEADER_SIZE;

    MTP_RECEIVE_DATA(initial_packet_size);
    length = mtp_op.fs_file_write(handle,0,&mtp_receive_buff[MTP_CONTAINER_HEADER_SIZE],initial_packet_size - MTP_CONTAINER_HEADER_SIZE);
    if (length != initial_packet_size - MTP_CONTAINER_HEADER_SIZE)
        goto mtp_file_write_fail;

    for (uint32_t i = 1; i <= receive_parts; i++)
    {
        MTP_RECEIVE_DATA(MTP_BUFF_SIZE);
        length = mtp_op.fs_file_write(handle,i*MTP_BUFF_SIZE-MTP_CONTAINER_HEADER_SIZE,mtp_receive_buff,MTP_BUFF_SIZE);
        if (length != MTP_BUFF_SIZE)
            goto mtp_file_write_fail;
    }

    if (remaining)
    {
        MTP_RECEIVE_DATA(MTP_BUFF_SIZE);
        length = mtp_op.fs_file_write(handle,(receive_parts+1)*MTP_BUFF_SIZE-MTP_CONTAINER_HEADER_SIZE,mtp_receive_buff,remaining);
        if (length != remaining)
            goto mtp_file_write_fail;
    }

    rslt = 0;

mtp_file_write_fail:
    mtp_op.fs_file_write(handle, file_obj.size, mtp_receive_buff, 0); /* Close file --> Offset = file size, length = 0 */
    receive_complete_status = false;
    /* File data transfer is done. Start receiving commands from MTP host again */
    mtp_data_transfer_in_progress = false;
    receive_data_from_host(mtp_receive_buff_temp, 32);

    return rslt;

}

/*!
 *  @brief Send file (array) to MTP initiator (USB Host)
 */
void mtp_transmit_mem_file(uint8_t *file_data, uint32_t file_size)
{
    uint32_t initial_packet_size, transmit_parts = 0, remaining = 0;

    if (file_size + MTP_CONTAINER_HEADER_SIZE > MTP_BUFF_SIZE)
    {
        initial_packet_size = MTP_BUFF_SIZE;
        transmit_parts = (file_size + MTP_CONTAINER_HEADER_SIZE - MTP_BUFF_SIZE) / MTP_BUFF_SIZE;
        remaining = (file_size + MTP_CONTAINER_HEADER_SIZE) % MTP_BUFF_SIZE;
    }
    else
        initial_packet_size = file_size + MTP_CONTAINER_HEADER_SIZE;

    *(uint32_t *)&mtp_transmit_buff[MTP_CONTAINER_LENGTH_OFFSET] = file_size + MTP_CONTAINER_HEADER_SIZE ;
    *(uint16_t *)&mtp_transmit_buff[MTP_CONTAINER_TYPE_OFFSET] = MTP_CONTAINER_TYPE_DATA;
    *(uint16_t *)&mtp_transmit_buff[MTP_CONTAINER_CODE_OFFSET] = mtp_container_cmd.operation;
    *(uint32_t *)&mtp_transmit_buff[MTP_CONTAINER_TRANSACTION_ID_OFFSET] = mtp_container_cmd.transaction_id;

    memcpy(&mtp_transmit_buff[MTP_CONTAINER_HEADER_SIZE],file_data,MTP_BUFF_SIZE-MTP_CONTAINER_HEADER_SIZE);
    transmit_complete_status = false;
    send_data_to_host(mtp_transmit_buff, initial_packet_size);
    while (transmit_complete_status == false);

    for (uint32_t i = 1; i <= transmit_parts; i++)
    {
        transmit_complete_status = false;
        send_data_to_host(&file_data[i*MTP_BUFF_SIZE-MTP_CONTAINER_HEADER_SIZE],MTP_BUFF_SIZE);
        while(transmit_complete_status == false);
    }

    if (remaining)
    {
        transmit_complete_status = false;
        send_data_to_host(&file_data[(transmit_parts+1)*MTP_BUFF_SIZE-MTP_CONTAINER_HEADER_SIZE],remaining);
        while(transmit_complete_status == false);
    }
}

/*!
 *  @brief Call this API after forming the MTP packet
 */
void mtp_transmit_packet()
{

    *(uint32_t *)&mtp_transmit_buff[MTP_CONTAINER_LENGTH_OFFSET] = transmit_buffer_index;
    *(uint16_t *)&mtp_transmit_buff[MTP_CONTAINER_TYPE_OFFSET] = MTP_CONTAINER_TYPE_DATA;
    *(uint16_t *)&mtp_transmit_buff[MTP_CONTAINER_CODE_OFFSET] = mtp_container_cmd.operation;
    *(uint32_t *)&mtp_transmit_buff[MTP_CONTAINER_TRANSACTION_ID_OFFSET] = mtp_container_cmd.transaction_id;

    transmit_complete_status = false;
    send_data_to_host(mtp_transmit_buff,transmit_buffer_index);
    while(transmit_complete_status == false);
}

/*!
 *  @brief Read byte array from global variable - mtp_receive_buff
 */
void mtp_buff_get_byte_array(uint8_t *value, uint32_t length)
{
    if (value != NULL)
        memcpy(value, &mtp_receive_buff[receive_buffer_index], length);
    receive_buffer_index += length;
}

/*!
 *  @brief Read single uint64_t data from global variable - mtp_receive_buff
 */
uint64_t mtp_buff_get_u64()
{
    uint64_t value = 0;
    value = *(uint32_t *) (&mtp_receive_buff[receive_buffer_index]) ;
    receive_buffer_index += 4;
    value |= (uint64_t) (*(uint32_t *) (&mtp_receive_buff[receive_buffer_index])) << 32 ;
    transmit_buffer_index += 4;
    return value;
}

/*!
 *  @brief Read single uint32_t data from global variable - mtp_receive_buff
 */
uint32_t mtp_buff_get_u32()
{
    uint32_t value = 0;
    value = *(uint32_t *) (&mtp_receive_buff[receive_buffer_index]);
    receive_buffer_index += 4;
    return value;
}

/*!
 *  @brief Read single uint16_t data from global variable - mtp_receive_buff
 */
uint16_t mtp_buff_get_u16()
{
    uint16_t value = 0;
    value = *(uint16_t *) (&mtp_receive_buff[receive_buffer_index]);
    receive_buffer_index += 2;
    return value;
}

/*!
 *  @brief Read single uint8_t data from global variable - mtp_receive_buff
 */
uint8_t mtp_buff_get_u8()
{
    uint8_t value = 0;
    value = mtp_receive_buff[receive_buffer_index];
    transmit_buffer_index += 1;
    return value;
}

/*!
 *  @brief Read UTF-16 string from global variable - mtp_receive_buff
 */
void mtp_buff_get_string(char *string)
{
    int length = mtp_buff_get_u8();
    if (!string)
    {
        mtp_buff_get_byte_array(NULL, length * 2);
    }
    else
    {
        for (int i = 0; i < length; i++)
        {
            *(string++) = mtp_buff_get_u16() >> 8;
        }
    }
}


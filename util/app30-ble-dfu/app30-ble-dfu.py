# -*- coding: utf-8 -*-
"""
Copyright (C) 2020 Bosch Sensortec GmbH

SPDX-License-Identifier: BSD-3-Clause

Created on Tue Jan 28 13:56:48 2020

@author: Rinny Babu (RBEI/EAC) <rnb2cob> , Aswathy Rajan (RBEI/EAC) <rsw1cob>

PC based tool for performing BLE DFU on nRF5 chips having Adafruit / legacy (nRF SDK v11 or lesser) bootloaders
https://github.com/adafruit/Adafruit_nRF52_Bootloader
Works with latest Bluetooth v4.0 USB dongles and recent notebook PCs with Bluetooth.
Tested with CSR8510 dongle in Windows 10 (Build 16299 and above) and Ubuntu 18.04 LTS
"""

import argparse
import asyncio
import os
import struct
import sys

from bleak import BleakClient

DFU_SERVICE_UUID = "00001530-1212-efde-1523-785feabcd123"
DFU_CONTROL_POINT_UUID = "00001531-1212-efde-1523-785feabcd123"
DFU_PACKET_UUID = "00001532-1212-efde-1523-785feabcd123"
DFU_VERSION_UUID = "00001534-1212-efde-1523-785feabcd123"

BLE_GATT_WRITE_LEN = 20

INIT_DATA_PREFIX = bytearray([0x52, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0xfe, 0xff])
SIZE_PACKET_PREFIX = bytearray([0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])

PROGRESS_BAR_SIZE = 25

# Information from nRF5 SDK v11.0.0 documentation
# https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v11.0.0%2Fbledfu_transport_bleservice.html
START_DFU = 1
RECEIVE_INIT = 2
RECEIVE_FW = 3
VALIDATE = 4
ACTIVATE_N_RESET = 5
SYS_RESET = 6
IMAGE_SIZE_REQ = 7
PKT_RCPT_NOTIF_REQ = 8
RESPONSE = 16
PKT_RCPT_NOTIF = 17

RECEIVE_INIT_RDY = 0
RECEIVE_INIT_COMPLETE = 1

NO_IMAGE = 0
SOFTDEVICE = 1
BOOTLOADER = 2
SOFTDEVICE_BOOTLOADER = 3
APPLICATION = 4

SUCCESS = 1
INVALID_STATE = 2
NOT_SUPPORTED = 3
DATA_SIZE_LIMIT_EXCEEDED = 4
CRC_ERROR = 5
OPERATION_FAILED = 6


# callback for notification handler
def notification_DFUcontrolpoint(sender, data):
    global notification_flag, dfu_response_code
    notification_flag = True
    if data[0] is RESPONSE:
        dfu_response_code = data[2]


# CRC16 calculation
def crc16(data, offset, length):
    if data is None or offset < 0 or offset > len(data) - 1 and offset + length > len(data):
        return 0
    crc = 0xFFFF
    for i in range(0, length):
        crc ^= data[offset + i] << 8
        for j in range(0, 8):
            if (crc & 0x8000) > 0:
                crc = (crc << 1) ^ 0x1021
            else:
                crc = crc << 1
    return crc & 0xFFFF


async def run(address, loop, firmware_file):
    global notification_flag, dfu_response_code

    async with BleakClient(address, loop=loop) as client:
        dfu_service_found = False
        notification_flag = False
        dfu_response_code = SUCCESS

        connected = await client.is_connected()
        if connected is True:
            print("Connected to " + client.address)

        for service in client.services:
            if service.uuid == DFU_SERVICE_UUID:
                dfu_service_found = True

        # Read DFU version
        if dfu_service_found is True:
            value = await client.read_gatt_char(DFU_VERSION_UUID)
            print("nRF5 BLE DFU service found - v" + str(value[1]) + "." + str(value[0]) + "\n")
            await asyncio.sleep(.1, loop=loop)
        else:
            print("No DFU service ! Disconnecting ...")
            await client.disconnect()
            exit(-1)

        # Enable notifications
        await client.start_notify(DFU_CONTROL_POINT_UUID, notification_DFUcontrolpoint)

        # Start Application firmware DFU
        value = bytearray([START_DFU, APPLICATION])
        await client.write_gatt_char(DFU_CONTROL_POINT_UUID, value, response=True)
        await asyncio.sleep(.1, loop=loop)

        # Write image size to DFU_PACKET characteristic
        firmware_file_size = os.path.getsize(firmware_file)
        value = SIZE_PACKET_PREFIX + struct.pack('<L', firmware_file_size)
        await client.write_gatt_char(DFU_PACKET_UUID, value, response=True)
        await asyncio.sleep(5, loop=loop)

        if notification_flag is True and dfu_response_code is SUCCESS:
            notification_flag = False
            # DFU start initialization
            value = bytearray([RECEIVE_INIT, RECEIVE_INIT_RDY])
            await client.write_gatt_char(DFU_CONTROL_POINT_UUID, value, response=True)
            await asyncio.sleep(.2, loop=loop)

            # Send initializing parameters
            with open(firmware_file, 'rb') as fh:
                firmware_data = fh.read()

            crc16_info = crc16(firmware_data, 0, firmware_file_size)
            value = INIT_DATA_PREFIX + struct.pack('<H', crc16_info)
            await client.write_gatt_char(DFU_PACKET_UUID, value, response=True)
            await asyncio.sleep(.2, loop=loop)

            # Notify that `initialization of DFU parameters` is complete
            value = bytearray([RECEIVE_INIT, RECEIVE_INIT_COMPLETE])
            await client.write_gatt_char(DFU_CONTROL_POINT_UUID, value, response=True)
            await asyncio.sleep(.5, loop=loop)
        else:
            print("DFU Initialization error !")
            exit(-dfu_response_code)

        # Request for firmware image
        if notification_flag is True and dfu_response_code is SUCCESS:
            notification_flag = False
            value = bytearray([RECEIVE_FW])
            await client.write_gatt_char(DFU_CONTROL_POINT_UUID, value, response=True)
            await asyncio.sleep(.2, loop=loop)

            # Upload firmware
            fo = open(firmware_file, 'rb')
            tail_byte_count = firmware_file_size % BLE_GATT_WRITE_LEN
            if tail_byte_count is 0:
                num_of_blocks = int(firmware_file_size / BLE_GATT_WRITE_LEN)
            else:
                num_of_blocks = int((firmware_file_size / BLE_GATT_WRITE_LEN) + 1)

            for i in range(0, num_of_blocks):
                line = fo.read(BLE_GATT_WRITE_LEN)
                value = bytearray(line)
                await client.write_gatt_char(DFU_PACKET_UUID, value)
                if dfu_response_code is not SUCCESS:
                    print("\nDFU download error !")
                    exit(-dfu_response_code)

                t = int((i + 1) * PROGRESS_BAR_SIZE / num_of_blocks)
                no_of_symbols = 0
                if (t != no_of_symbols):
                    no_of_symbols = t
                    sys.stdout.write("Download  [")
                    sys.stdout.write('=' * no_of_symbols)
                    sys.stdout.write(' ' * (PROGRESS_BAR_SIZE - no_of_symbols))
                    sys.stdout.write("]  %d %% (%.2f kB/%.2f kB)" % ((i + 1) * 100 / num_of_blocks, (i * 20 + tail_byte_count) / 1024., firmware_file_size / 1024.))
                    sys.stdout.write('\r')
                    sys.stdout.flush()

                await asyncio.sleep(.025, loop=loop)

            print('\n')
            await asyncio.sleep(1, loop=loop)

        # Validate firmware
        if notification_flag is True and dfu_response_code is SUCCESS:
            notification_flag = False
            value = bytearray([VALIDATE])
            await client.write_gatt_char(DFU_CONTROL_POINT_UUID, value, response=True)
            await asyncio.sleep(.5, loop=loop)
        else:
            print("DFU download error !")
            exit(-dfu_response_code)

        # Activate and reset
        if notification_flag is True and dfu_response_code is SUCCESS:
            print("DFU Success !")
            notification_flag = False
            value = bytearray([ACTIVATE_N_RESET])
            await client.write_gatt_char(DFU_CONTROL_POINT_UUID, value, response=True)
            await asyncio.sleep(.5, loop=loop)
        else:
            print("Firmware integrity check failed !")
            exit(-dfu_response_code)


if __name__ == "__main__":
    print("Application Board 3.0 BLE DFU tool")
    print("Bosch Sensortec GmbH (C) 2020")
    print("")
    cmdline_parser = argparse.ArgumentParser()
    required_args = cmdline_parser.add_argument_group('required arguments')
    required_args.add_argument('-d', dest='device_mac_addr', help='Specify device MAC address', required=True)
    required_args.add_argument('-f', dest='firmware_bin_file', help='Specify firmware binary', required=True)
    required_args = cmdline_parser.parse_args()
    devices = (required_args.device_mac_addr)
    firmware = required_args.firmware_bin_file

    loop = asyncio.get_event_loop()
    loop.run_until_complete(run(devices, loop, firmware))

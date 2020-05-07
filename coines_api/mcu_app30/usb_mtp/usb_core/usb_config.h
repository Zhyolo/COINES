/**
 * Copyright (c) 2016 - 2018, Nordic Semiconductor ASA
 * Copyright (c) 2019, Bosch Sensortec GmbH
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef USB_CONFIG_H_
#define USB_CONFIG_H_

/** Device descriptor */
#include "mtp.h"
#define USBD_DEVICE_DESCRIPTOR \
    0x12,                        /* bLength | size of descriptor                                                  */\
    0x01,                        /* bDescriptorType | descriptor type                                             */\
    0x00, 0x02,                  /* bcdUSB | USB spec release (ver 2.0)                                           */\
    0x00,                        /* bDeviceClass ¦ class code (each interface specifies class information)        */\
    0x00,                        /* bDeviceSubClass ¦ device sub-class (must be set to 0 because class code is 0) */\
    0x00,                        /* bDeviceProtocol | device protocol (no class specific protocol)                */\
    64,                          /* bMaxPacketSize0 | maximum packet size (64 bytes)                              */\
    0x8C, 0x10,                  /* vendor ID  (0x108C Robert Bosch GmbH)                                         */\
    0x3F, 0xAB,                  /* product ID (0xAB3F APP3.0  Board [MTP])                      */\
    0x00, 0x01,                  /* bcdDevice | final device release number in BCD Format                         */\
    USBD_STRING_MANUFACTURER_IX, /* iManufacturer | index of manufacturer string                                  */\
    USBD_STRING_PRODUCT_IX,      /* iProduct | index of product string                                            */\
    USBD_STRING_SERIAL_IX,       /* iSerialNumber | Serial Number string                                          */\
    0x01                         /* bNumConfigurations | number of configurations                                 */


#define USBD_CONFIG_DESCRIPTOR_SIZE   9
#define USBD_CONFIG_DESCRIPTOR_FULL_SIZE   39
#define USBD_CONFIG_DESCRIPTOR  \
    0x09,         /* bLength | length of descriptor                                             */\
    0x02,         /* bDescriptorType | descriptor type (CONFIGURATION)                          */\
    USBD_CONFIG_DESCRIPTOR_FULL_SIZE, 0x00,    /* wTotalLength | total length of descriptor(s)  */\
    0x01,         /* bNumInterfaces                                                             */\
    0x01,         /* bConfigurationValue                                                        */\
    0x00,         /* index of string Configuration | configuration string index (not supported) */\
    0x80,         /* bmAttributes    */\
    50            /* maximum power in steps of 2mA (100mA)                                       */

#define USBD_INTERFACE0_DESCRIPTOR  \
    0x09,         /* bLength                                                                     */\
    0x04,         /* bDescriptorType | descriptor type (INTERFACE)                               */\
    0x00,         /* bInterfaceNumber                                                            */\
    0x00,         /* bAlternateSetting                                                           */\
    0x03,         /* bNumEndpoints | number of endpoints (1)                                     */\
    0xFF,         /* bInterfaceClass | Application-Specific                                      */\
    0xFF,         /* bInterfaceSubClass | Device Firmware Upgrade                                */\
    0x00,         /* bInterfaceProtocol | DFU Mode                                               */\
    0x04          /* interface string index                                                      */

#define USBD_ENDPOINT1_DESCRIPTOR \
  0x07,         /* bLength                                                             */\
  0x05,         /* bDescriptorType | descriptor type  (Endpoint)                       */\
  0x81,         /* bEndpointAddress                                                    */\
  0x02,         /* bmAttributes                                                        */\
  0x40,0x00,    /* wMaxPacketSize                                                      */\
  0x00          /* bInterval                                                           */\

#define USBD_ENDPOINT2_DESCRIPTOR \
  0x07,         /* bLength                                                             */\
  0x05,         /* bDescriptorType | descriptor type  (Endpoint)                       */\
  0x02,         /* bEndpointAddress                                                    */\
  0x02,         /* bmAttributes                                                        */\
  0x40,0x00,    /* wMaxPacketSize                                                      */\
  0x00          /* bInterval                                                           */\

#define USBD_ENDPOINT3_DESCRIPTOR \
  0x07,         /* bLength                                                             */\
  0x05,         /* bDescriptorType | descriptor type  (Endpoint)                       */\
  0x83,         /* bEndpointAddress                                                    */\
  0x03,         /* bmAttributes                                                        */\
  0x1C,0x00,    /* wMaxPacketSize                                                      */\
  0x06          /* bInterval                                                           */\

/**
 * String config descriptor
 */
#define USBD_STRING_LANG_IX  0x00
#define USBD_STRING_LANG \
    0x04,         /* length of descriptor                   */\
    0x03,         /* descriptor type                        */\
    0x09,         /*                                        */\
    0x04          /* Supported LangID = 0x0409 (US-English) */

#define USBD_STRING_MANUFACTURER_IX  0x01
#define USBD_STRING_MANUFACTURER \
    42,           /* length of descriptor (? bytes)   */\
    0x03,         /* descriptor type                  */\
    'B', 0x00,    /* Define Unicode String "Nordic Semiconductor  */\
    'o', 0x00, \
    's', 0x00, \
    'c', 0x00, \
    'h', 0x00, \
    ' ', 0x00, \
    'S', 0x00, \
    'e', 0x00, \
    'n', 0x00, \
    's', 0x00, \
    'o', 0x00, \
    'r', 0x00, \
    't', 0x00, \
    'e', 0x00, \
    'c', 0x00, \
    ' ', 0x00, \
    'G', 0x00, \
    'm', 0x00, \
    'b', 0x00, \
    'H', 0x00

#define USBD_STRING_PRODUCT_IX  0x02
#define USBD_STRING_PRODUCT \
    36,           /* length of descriptor (? bytes)         */\
    0x03,         /* descriptor type                        */\
    'A', 0x00,    /* generic unicode string for all devices */\
    'P', 0x00, \
    'P', 0x00, \
    '3', 0x00, \
    '.', 0x00, \
    '0', 0x00, \
    ' ', 0x00, \
    'B', 0x00, \
    'o', 0x00, \
    'a', 0x00, \
    'r', 0x00, \
    'd', 0x00, \
    '(', 0x00, \
    'M', 0x00, \
    'T', 0x00, \
    'P', 0x00, \
    ')', 0x00, \

#define USBD_STRING_SERIAL_IX  0x03
#define USBD_STRING_SERIAL \
22,       \
0x03,     \
'A',0x00, \
'P',0x00, \
'P',0x00, \
'3',0x00, \
'.',0x00, \
'0',0x00, \
'-',0x00, \
'M',0x00, \
'T',0x00, \
'P',0x00  \

#define USBD_STRING_INTERFACE_SETTING_0_IX  0x04
#define USBD_STRING_INTERFACE_SETTING_0 \
8,        \
0x03,     \
'M',0x00, \
'T',0x00, \
'P',0x00

#define GET_CONFIG_DESC_SIZE    sizeof(get_descriptor_configuration)
#define GET_INTERFACE_DESC_SIZE 9
#define GET_ENDPOINT_DESC_SIZE  7

#define get_descriptor_interface_0 \
    &get_descriptor_configuration[9]

#define MSFT_STR \
18, \
0x03, \
'M', 0x00, \
'S', 0x00, \
'F', 0x00, \
'T', 0x00, \
'1', 0x00, \
'0', 0x00, \
'0', 0x00, \
0xEE,0x00

#endif

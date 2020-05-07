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

#include "usb_core.h"
#include "usb_config.h"

static const uint8_t get_descriptor_device[] = {
    USBD_DEVICE_DESCRIPTOR
};

static const uint8_t get_descriptor_configuration[] = {
    USBD_CONFIG_DESCRIPTOR,
    USBD_INTERFACE0_DESCRIPTOR,
	USBD_ENDPOINT1_DESCRIPTOR,
	USBD_ENDPOINT2_DESCRIPTOR,
	USBD_ENDPOINT3_DESCRIPTOR,
};
static const uint8_t get_descriptor_string_lang[] = {
    USBD_STRING_LANG
};
static const uint8_t get_descriptor_string_manuf[] = {
    USBD_STRING_MANUFACTURER
};
static const uint8_t get_descriptor_string_prod[] = {
    USBD_STRING_PRODUCT
};

static const uint8_t get_descriptor_string_serial[] = {
    USBD_STRING_SERIAL
};

static const uint8_t get_descriptor_string_interface_setting_0[] = {
    USBD_STRING_INTERFACE_SETTING_0
};

static const uint8_t msft_str[]={
	MSFT_STR
};

const MicrosoftCompatibleDescriptor msft_compatible = {
    .dwLength = sizeof(MicrosoftCompatibleDescriptor) + sizeof(MicrosoftCompatibleDescriptor_Interface),
    .bcdVersion = 0x0100,
    .wIndex = 0x0004,
    .bCount = 1,
    .reserved = {0, 0, 0, 0, 0, 0, 0},
    .interfaces = {
        {
            .bFirstInterfaceNumber = 0,
            .reserved1 = 0,
            .compatibleID = "MTP",
            .subCompatibleID = {0, 0, 0, 0, 0, 0, 0, 0},
            .reserved2 = {0, 0, 0, 0, 0, 0},
        }
    }
};

static volatile bool m_usbd_configured = false;
static volatile bool m_usbd_suspend_state_req = false;
static volatile bool m_system_off_req = false;

static const uint8_t get_status_device_resp[] = {0, 0};
static const uint8_t get_status_interface_resp[] = {0, 0};
static const uint8_t get_status_ep_active_resp[] = {0, 0};

static const uint8_t get_config_resp_configured[]   = {1};
static const uint8_t get_config_resp_unconfigured[] = {0};

static void usbd_setup_GetStatus(nrf_drv_usbd_setup_t const * const p_setup)
{
    switch (p_setup->bmRequestType)
    {
    case 0x80: // Device
        if (((p_setup->wIndex) & 0xff) == 0)
        {
            respond_setup_data(
                p_setup,
				get_status_device_resp,
                sizeof(get_status_device_resp));
            return;
        }
        break;
    case 0x81: // Interface
        if (m_usbd_configured) // Respond only if configured
        {
            if (((p_setup->wIndex) & 0xff) == 0) // Only interface 0 supported
            {
                respond_setup_data(
                    p_setup,
                    get_status_interface_resp,
                    sizeof(get_status_interface_resp));
                return;
            }

        }
        break;
    case 0x82: // Endpoint
		respond_setup_data(p_setup, get_status_ep_active_resp,
				sizeof(get_status_ep_active_resp));
		return;
        break;
    default:
        break; // Just go to stall
    }
    nrf_drv_usbd_setup_stall();
}

static void usbd_setup_ClearFeature(nrf_drv_usbd_setup_t const * const p_setup)
{
    if ((p_setup->bmRequestType) == 0x02) // standard request, recipient=endpoint
    {
        if ((p_setup->wValue) == 0)
        {
                nrf_drv_usbd_setup_clear();
                return;
        }
    }
    else if ((p_setup->bmRequestType) ==  0x0) // standard request, recipient=device
    {
                nrf_drv_usbd_setup_clear();
                return;
    }
    nrf_drv_usbd_setup_stall();
}

static void usbd_setup_SetFeature(nrf_drv_usbd_setup_t const * const p_setup)
{
    if ((p_setup->bmRequestType) == 0x02) // standard request, recipient=endpoint
    {
        if ((p_setup->wValue) == 0) // Feature HALT
        {
                nrf_drv_usbd_setup_clear();
                return;
        }
    }
    else if ((p_setup->bmRequestType) ==  0x0) // standard request, recipient=device
    {

                nrf_drv_usbd_setup_clear();
                return;
    }
    nrf_drv_usbd_setup_stall();
}

static void usbd_setup_GetDescriptor(nrf_drv_usbd_setup_t const * const p_setup)
{
    //determine which descriptor has been asked for
    switch ((p_setup->wValue) >> 8)
    {
    case 1: // Device
        if ((p_setup->bmRequestType) == 0x80)
        {
            respond_setup_data(
                p_setup,
                get_descriptor_device,
                sizeof(get_descriptor_device));
            return;
        }
        break;
    case 2: // Configuration
        if ((p_setup->bmRequestType) == 0x80)
        {
            respond_setup_data(
                p_setup,
                get_descriptor_configuration,
                GET_CONFIG_DESC_SIZE);
            return;
        }
        break;
    case 3: // String
        if ((p_setup->bmRequestType) == 0x80)
        {
            // Select the string
            switch ((p_setup->wValue) & 0xFF)
            {
            case USBD_STRING_LANG_IX:
                respond_setup_data(
                    p_setup,
                    get_descriptor_string_lang,
                    sizeof(get_descriptor_string_lang));
                return;
            case USBD_STRING_MANUFACTURER_IX:
                respond_setup_data(
                    p_setup,
                    get_descriptor_string_manuf,
                    sizeof(get_descriptor_string_manuf));
                return;
            case USBD_STRING_PRODUCT_IX:
                respond_setup_data(p_setup,
                    get_descriptor_string_prod,
                    sizeof(get_descriptor_string_prod));
                return;
            case USBD_STRING_SERIAL_IX:
                respond_setup_data(p_setup,
                    get_descriptor_string_serial,
                    sizeof(get_descriptor_string_serial));
                return;
            case USBD_STRING_INTERFACE_SETTING_0_IX:
                respond_setup_data(p_setup,
                		get_descriptor_string_interface_setting_0,
                    sizeof(get_descriptor_string_interface_setting_0));
                return;
            case 0xEE:
            	respond_setup_data(p_setup,msft_str,sizeof(msft_str));
            	return;
            default:
                break;
            }
        }
        break;
    case 4: // Interface
        if ((p_setup->bmRequestType) == 0x80)
        {
            // Which interface?
            if ((((p_setup->wValue) & 0xFF) == 0))
            {
                respond_setup_data(
                    p_setup,
                    get_descriptor_interface_0,
                    GET_INTERFACE_DESC_SIZE);
                return;
            }
        }
        break;
    case 5: // Endpoint
        if ((p_setup->bmRequestType) == 0x80)
        {
            // Which endpoint?
            if (((p_setup->wValue) & 0xFF) == 1)
            {
                return;
            }
        }
        break;

    case 6: // Device Qualifier
        if ((p_setup->bmRequestType) == 0x80)
        {
            nrf_drv_usbd_setup_clear();
        }
        break;

    case 0x0a: // Debug
        if ((p_setup->bmRequestType) == 0x80)
        {
            nrf_drv_usbd_setup_clear();
        }
        break;


    default:
        break; // Not supported - go to stall
    }
}

static void usbd_setup_GetConfig(nrf_drv_usbd_setup_t const * const p_setup)
{
    if (m_usbd_configured)
    {
        respond_setup_data(
            p_setup,
            get_config_resp_configured,
            sizeof(get_config_resp_configured));
    }
    else
    {
        respond_setup_data(
            p_setup,
            get_config_resp_unconfigured,
            sizeof(get_config_resp_unconfigured));
    }
}

static void usbd_setup_SetConfig(nrf_drv_usbd_setup_t const * const p_setup)
{
    if ((p_setup->bmRequestType) == 0x00)
    {
        // accept only 0 and 1
        if (((p_setup->wIndex) == 0) && ((p_setup->wLength) == 0) &&
            ((p_setup->wValue) <= UINT8_MAX))
        {
            nrf_drv_usbd_ep_dtoggle_clear(NRF_DRV_USBD_EPIN1);
            nrf_drv_usbd_ep_stall_clear(NRF_DRV_USBD_EPIN1);
            nrf_drv_usbd_ep_enable(NRF_DRV_USBD_EPIN1);

            nrf_drv_usbd_ep_dtoggle_clear(NRF_DRV_USBD_EPOUT2);
            nrf_drv_usbd_ep_stall_clear(NRF_DRV_USBD_EPOUT2);
            nrf_drv_usbd_ep_enable(NRF_DRV_USBD_EPOUT2);

            nrf_drv_usbd_ep_dtoggle_clear(NRF_DRV_USBD_EPIN3);
            nrf_drv_usbd_ep_stall_clear(NRF_DRV_USBD_EPIN3);
            nrf_drv_usbd_ep_enable(NRF_DRV_USBD_EPIN3);
            mtp_init();

            nrf_drv_usbd_setup_clear();
            return;
        }
    }
}

static void usbd_setup_SetIdle(nrf_drv_usbd_setup_t const * const p_setup)
{
    if (p_setup->bmRequestType == 0x21)
    {
        //accept any value
        nrf_drv_usbd_setup_clear();
        return;
    }
    nrf_drv_usbd_setup_stall();
}

static void usbd_setup_SetInterface(
    nrf_drv_usbd_setup_t const * const p_setup)
{
	nrf_drv_usbd_setup_clear();
}

static void usbd_event_handler(nrf_drv_usbd_evt_t const * const p_event)
{
	 switch (p_event->type)
	    {
	    case NRF_DRV_USBD_EVT_SUSPEND:
	        m_usbd_suspend_state_req = true;
	        break;
	    case NRF_DRV_USBD_EVT_RESUME:
	        m_usbd_suspend_state_req = false;
	        break;
	    case NRF_DRV_USBD_EVT_WUREQ:
	        m_usbd_suspend_state_req = false;
	        break;
	    case NRF_DRV_USBD_EVT_RESET:
	        {
	            m_usbd_suspend_state_req = false;
	            break;
	        }
	    case NRF_DRV_USBD_EVT_SOF:
	        {
	            static uint32_t cycle = 0;
	            ++cycle;
	            if ((cycle % (m_usbd_configured ? 500 : 100)) == 0)
	            {
	            }
	            break;
	        }
	    case NRF_DRV_USBD_EVT_EPTRANSFER:
	        if(NRF_DRV_USBD_EPIN1 == p_event->data.eptransfer.ep)
	        {
	            usb_transmit_complete_cb();
	            break;
	        }
	        if(NRF_DRV_USBD_EPOUT2 == p_event->data.eptransfer.ep)
	        {
	            usb_receive_complete_cb();
	            break;
	        }

            if (p_event->data.eptransfer.status == NRF_USBD_EP_ABORTED)
            {
                /* Just ignore aborting */
                break;
            }

	        if (NRF_DRV_USBD_EPIN0 == p_event->data.eptransfer.ep)
	        {
	                if (!nrf_drv_usbd_errata_154())
	                {
	                    /* Transfer ok - allow status stage */
	                    nrf_drv_usbd_setup_clear();
	                }
	        }
	        else
	        if (NRF_DRV_USBD_EPOUT0 == p_event->data.eptransfer.ep)
	        {
	                /* NOTE: Data values or size may be tested here to decide if clear or stall.
	                 * If errata 154 is present the data transfer is acknowledged by the hardware. */
	                if (!nrf_drv_usbd_errata_154())
	                {
	                    /* Transfer ok - allow status stage */
	                    nrf_drv_usbd_setup_clear();
	                }
	        }
	        break;
	    case NRF_DRV_USBD_EVT_SETUP:
	        {
	            nrf_drv_usbd_setup_t setup;
	            nrf_drv_usbd_setup_get(&setup);

	        	if (setup.wIndex == 0x0004 && setup.bmRequest == 0xEE)
	        	{
	        		respond_setup_data(&setup,(uint8_t *)&msft_compatible, setup.wLength);
	        		return;
	        	}

	            switch (setup.bmRequest)
	            {
	            case 0x00: // GetStatus
	                usbd_setup_GetStatus(&setup);
	                break;
	            case 0x01: // CleartFeature
	                usbd_setup_ClearFeature(&setup);
	                break;
	            case 0x03: // SetFeature
	                usbd_setup_SetFeature(&setup);
	                break;
	            case 0x05: // SetAddress
	                //nothing to do, handled by hardware; but don't STALL
	                break;
	            case 0x06: // GetDescriptor
	                usbd_setup_GetDescriptor(&setup);
	                break;
	            case 0x08: // GetConfig
	                usbd_setup_GetConfig(&setup);
	                break;
	            case 0x09: // SetConfig
	                usbd_setup_SetConfig(&setup);
	                break;
	            case 0x0A: // SetIdle
	                usbd_setup_SetIdle(&setup);
	                break;
	            case 0x0B: // SetProtocol or SetInterface
	                if (setup.bmRequestType == 0x01) // standard request, recipient=interface
	                {
	                    usbd_setup_SetInterface(&setup);
	                }
	                else
	                {
	                    nrf_drv_usbd_setup_stall();
	                }
	                break;
	            default:
	                nrf_drv_usbd_setup_stall();
	                return;
	            }
	            break;
	        }
	    default:
	        break;
	    }
}

static void power_usb_event_handler(nrf_drv_power_usb_evt_t event)
{
    switch (event)
    {
    case NRF_DRV_POWER_USB_EVT_DETECTED:
        if (!nrf_drv_usbd_is_enabled())
        {
            nrf_drv_usbd_enable();
        }
        break;
    case NRF_DRV_POWER_USB_EVT_REMOVED:
        if (nrf_drv_usbd_is_started())
        {
            nrf_drv_usbd_stop();
        }
        if (nrf_drv_usbd_is_enabled())
        {
            nrf_drv_usbd_disable();
        }
        break;
    case NRF_DRV_POWER_USB_EVT_READY:
        if (!nrf_drv_usbd_is_started())
        {
            nrf_drv_usbd_start(true);
        }
        break;
    }
}

void init_usb(void)
{
    nrf_drv_usbd_init(usbd_event_handler);

    static const nrf_drv_power_usbevt_config_t config =
    {
        .handler = power_usb_event_handler
    };
    nrf_drv_power_usbevt_init(&config);
}


void respond_setup_data(nrf_drv_usbd_setup_t const * const p_setup, void const * p_data, size_t size)
{
	/* Check the size against required response size */
	if (size > p_setup->wLength)
	{
		size = p_setup->wLength;
	}

	NRF_DRV_USBD_TRANSFER_IN(transfer, p_data, size);
	nrf_drv_usbd_ep_transfer(NRF_DRV_USBD_EPIN0, &transfer);

}

void initiate_setup_data(nrf_drv_usbd_setup_t const * const p_setup, void * p_data, size_t size)
{
	NRF_DRV_USBD_TRANSFER_OUT(transfer, p_data, size);
	nrf_drv_usbd_ep_transfer(NRF_DRV_USBD_EPOUT0, &transfer);
	nrf_drv_usbd_setup_data_clear();

	if (p_setup->wLength == 0)
	{
		nrf_drv_usbd_setup_clear();
	}
}

void send_data_to_host(void * p_data, size_t size)
{
	NRF_DRV_USBD_TRANSFER_IN(transfer, p_data, size);
	nrf_drv_usbd_ep_transfer(NRF_DRV_USBD_EPIN1, &transfer);
}

void receive_data_from_host(void * p_data, size_t size)
{
	NRF_DRV_USBD_TRANSFER_OUT(transfer, p_data, size);
	nrf_drv_usbd_ep_transfer(NRF_DRV_USBD_EPOUT2, &transfer);
}

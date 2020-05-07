ifeq ($(OS),Windows_NT)
    PLATFORM = PLATFORM_WINDOWS
    DRIVER ?= LEGACY_USB_DRIVER
else
    PLATFORM = PLATFORM_LINUX
    DRIVER = LIBUSB_DRIVER
endif

# 0 - False , 1 - True
ZEUS_QUIRK ?= 0

CC     = gcc

CFLAGS ?= -D$(PLATFORM) -c -g -Wall -D$(DRIVER)

C_SRCS_COINES += \
coines.c \
comm_intf/comm_intf.c \
comm_intf/comm_ringbuffer.c \
comm_driver/usb.c \

INCLUDEPATHS_COINES += \
. \
coines_api \
comm_intf \
comm_driver \

ifeq ($(DRIVER),LEGACY_USB_DRIVER)
C_SRCS_COINES += comm_driver/legacy_usb/legacy_usb_support.c
INCLUDEPATHS_COINES += comm_driver/legacy_usb
endif

ifeq ($(DRIVER),LIBUSB_DRIVER)
INCLUDEPATHS_COINES += comm_driver/libusb-1.0
endif

ifneq ($(ZEUS_QUIRK),0)
C_SRCS_COINES += quirks/zeus.c
INCLUDEPATHS_COINES += quirks
CFLAGS += -D ZEUS_QUIRK
endif
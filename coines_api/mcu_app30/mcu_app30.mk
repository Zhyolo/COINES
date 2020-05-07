CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar

CFLAGS += -std=c99 -DNRF52840_XXAA -D__HEAP_SIZE=8192 -D__STACK_SIZE=8192 -DSWI_DISABLE0 -DUSE_APP_CONFIG \
-mthumb -mabi=aapcs -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -c -g -Wall

nRF5_SDK_DIR = nRF5_SDK_15.2.0

ASM_SRCS_COINES = $(nRF5_SDK_DIR)/modules/nrfx/mdk/gcc_startup_nrf52840.S \

C_SRCS_COINES += \
mcu_app30.c \
support/ds28e05/ds28e05.c \
support/eeprom/app30_eeprom.c \
support/FLogFs/src/flogfs.c \
support/w25n01gwtbig/src/w25n01gwtbig.c \
$(nRF5_SDK_DIR)/modules/nrfx/mdk/system_nrf52840.c \
$(nRF5_SDK_DIR)/integration/nrfx/legacy/nrf_drv_clock.c \
$(nRF5_SDK_DIR)/integration/nrfx/legacy/nrf_drv_power.c \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/src/nrfx_clock.c  \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/src/nrfx_gpiote.c \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/src/nrfx_power.c  \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/src/nrfx_power_clock.c \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/src/nrfx_spim.c  \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/src/nrfx_systick.c \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/src/nrfx_timer.c \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/src/nrfx_twim.c  \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/src/prs/nrfx_prs.c \
$(nRF5_SDK_DIR)/components/drivers_nrf/usbd/nrf_drv_usbd.c \
$(nRF5_SDK_DIR)/components/libraries/usbd/app_usbd.c \
$(nRF5_SDK_DIR)/components/libraries/usbd/class/cdc/acm/app_usbd_cdc_acm.c \
$(nRF5_SDK_DIR)/components/libraries/usbd/app_usbd_core.c \
$(nRF5_SDK_DIR)/components/libraries/usbd/app_usbd_serial_num.c \
$(nRF5_SDK_DIR)/components/libraries/usbd/app_usbd_string_desc.c \
$(nRF5_SDK_DIR)/components/libraries/util/app_util_platform.c \
$(nRF5_SDK_DIR)/components/libraries/util/app_error.c \
$(nRF5_SDK_DIR)/components/libraries/util/app_error_handler_gcc.c \
$(nRF5_SDK_DIR)/components/libraries/util/app_error_weak.c \
$(nRF5_SDK_DIR)/components/libraries/hardfault/nrf52/handler/hardfault_handler_gcc.c \
$(nRF5_SDK_DIR)/components/libraries/hardfault/hardfault_implementation.c \
$(nRF5_SDK_DIR)/components/libraries/atomic_fifo/nrf_atfifo.c \
$(nRF5_SDK_DIR)/components/libraries/atomic/nrf_atomic.c \
$(nRF5_SDK_DIR)/components/drivers_nrf/nrf_soc_nosd/nrf_nvic.c \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/src/nrfx_spi.c \
$(nRF5_SDK_DIR)/integration/nrfx/legacy/nrf_drv_spi.c

INCLUDEPATHS_COINES += \
.. \
. \
conf \
support/ds28e05 \
support/eeprom \
support/FLogFs/inc \
support/w25n01gwtbig/inc \
$(nRF5_SDK_DIR)/modules/nrfx \
$(nRF5_SDK_DIR)/modules/nrfx/mdk \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/include \
$(nRF5_SDK_DIR)/modules/nrfx/hal \
$(nRF5_SDK_DIR)/components \
$(nRF5_SDK_DIR)/components/libraries/scheduler \
$(nRF5_SDK_DIR)/components/libraries/queue \
$(nRF5_SDK_DIR)/components/libraries/pwr_mgmt \
$(nRF5_SDK_DIR)/components/libraries/fifo \
$(nRF5_SDK_DIR)/components/libraries/strerror \
$(nRF5_SDK_DIR)/components/toolchain/cmsis/include \
$(nRF5_SDK_DIR)/components/libraries/timer \
$(nRF5_SDK_DIR)/components/libraries/util \
$(nRF5_SDK_DIR)/components/libraries/usbd/class/cdc \
$(nRF5_SDK_DIR)/components/drivers_nrf/usbd \
$(nRF5_SDK_DIR)/components/libraries/ringbuf \
$(nRF5_SDK_DIR)/components/libraries/hardfault/nrf52 \
$(nRF5_SDK_DIR)/components/libraries/hardfault \
$(nRF5_SDK_DIR)/components/libraries/log \
$(nRF5_SDK_DIR)/components/libraries/log/src \
$(nRF5_SDK_DIR)/components/libraries/experimental_section_vars \
$(nRF5_SDK_DIR)/components/libraries/usbd \
$(nRF5_SDK_DIR)/components/libraries/usbd/class/cdc/acm \
$(nRF5_SDK_DIR)/components/libraries/mutex \
$(nRF5_SDK_DIR)/components/libraries/delay \
$(nRF5_SDK_DIR)/components/libraries/atomic_fifo \
$(nRF5_SDK_DIR)/components/drivers_nrf/nrf_soc_nosd \
$(nRF5_SDK_DIR)/components/libraries/atomic \
$(nRF5_SDK_DIR)/components/boards \
$(nRF5_SDK_DIR)/integration/nrfx \
$(nRF5_SDK_DIR)/integration/nrfx/legacy \
$(nRF5_SDK_DIR)/external/fnmatch \
$(nRF5_SDK_DIR)/external/utf_converter \



OBJ_DIR = build

CROSS_COMPILE = arm-none-eabi-
GCC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
OBJCOPY = $(CROSS_COMPILE)objcopy
LD = $(CROSS_COMPILE)ld

UTIL_DIR = ../../../util

FS ?= FLOGFS
LOCATION ?= SPECIAL

ifeq ($(OS),Windows_NT)
    IS_GCC_FOUND = $(shell where $(GCC))
    JLINK = JLink
    $(info Platform: Windows)
    ifeq ($(notdir $(MAKE)),mingw32-make)
    RM  = cmd /c del /s /q
    else
    RM = rm -rf
    endif
else
    $(info Platform: Linux)
    IS_GCC_FOUND = $(shell which $(GCC))
    JLINK = JLinkExe
    EXT =
    RM = rm -rf
endif

ifeq ($(LOCATION),SPECIAL)
    LD_SCRIPT = usb_mtp.ld
else ifeq ($(LOCATION),FLASH)
    LD_SCRIPT =  ../linker_scripts/mcu_app30_flash.ld
else ifeq ($(LOCATION),RAM)
    LD_SCRIPT =  ../linker_scripts/mcu_app30_ram.ld
endif

$(info LOCATION : $(LOCATION))

CFLAGS += -std=c99 -DNRF52840_XXAA -D__HEAP_SIZE=8192 -D__STACK_SIZE=8192 
CFLAGS += -mthumb -mabi=aapcs -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -c -Os -g -Wall

LDFLAGS += -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -flto -pipe -specs=nano.specs -T $(LD_SCRIPT) -Xlinker -Map=$(OBJ_DIR)/build.map

nRF5_SDK_DIR = ../nRF5_SDK_15.2.0

ASM_SRCS = $(nRF5_SDK_DIR)/modules/nrfx/mdk/gcc_startup_nrf52840.S

C_SRCS += \
usb_mtp.c \
mtp/mtp.c \
usb_core/usb_core.c \
$(nRF5_SDK_DIR)/modules/nrfx/mdk/system_nrf52840.c \
$(nRF5_SDK_DIR)/integration/nrfx/legacy/nrf_drv_clock.c \
$(nRF5_SDK_DIR)/integration/nrfx/legacy/nrf_drv_power.c \
$(nRF5_SDK_DIR)/components/drivers_nrf/usbd/nrf_drv_usbd.c \
$(nRF5_SDK_DIR)/components/drivers_nrf/nrf_soc_nosd/nrf_nvic.c \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/src/nrfx_clock.c \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/src/nrfx_gpiote.c \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/src/nrfx_power.c \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/src/nrfx_power_clock.c \
$(nRF5_SDK_DIR)/components/libraries/util/app_util_platform.c \
$(nRF5_SDK_DIR)/components/libraries/atomic/nrf_atomic.c \

INCLUDEPATHS += \
. \
conf \
mtp \
usb_core \
$(nRF5_SDK_DIR)/modules/nrfx \
$(nRF5_SDK_DIR)/modules/nrfx/mdk \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/include \
$(nRF5_SDK_DIR)/modules/nrfx/hal \
$(nRF5_SDK_DIR)/components \
$(nRF5_SDK_DIR)/components/toolchain/cmsis/include \
$(nRF5_SDK_DIR)/components/libraries/util \
$(nRF5_SDK_DIR)/components/drivers_nrf/usbd \
$(nRF5_SDK_DIR)/components/libraries/ringbuf \
$(nRF5_SDK_DIR)/components/libraries/hardfault/nrf52 \
$(nRF5_SDK_DIR)/components/libraries/hardfault \
$(nRF5_SDK_DIR)/components/libraries/log \
$(nRF5_SDK_DIR)/components/libraries/log/src \
$(nRF5_SDK_DIR)/components/libraries/experimental_section_vars \
$(nRF5_SDK_DIR)/components/libraries/delay \
$(nRF5_SDK_DIR)/components/libraries/atomic_fifo \
$(nRF5_SDK_DIR)/components/libraries/atomic \
$(nRF5_SDK_DIR)/components/softdevice/s140/headers \
$(nRF5_SDK_DIR)/components/softdevice/s140/headers/nrf52 \
$(nRF5_SDK_DIR)/components/boards \
$(nRF5_SDK_DIR)/integration/nrfx \
$(nRF5_SDK_DIR)/integration/nrfx/legacy \
$(nRF5_SDK_DIR)/external/fnmatch \

ifeq ($(FS),DUMMYFS)
C_SRCS += \
storage/dummyfs/dummyfs.c

INCLUDEPATHS += \
storage/dummyfs

BINARY_BLOBS +=\
storage/dummyfs/content/README.txt \
storage/dummyfs/content/ReleaseNotes_COINES.pdf \

CFLAGS += -DFS_START=dummyfs_start
endif

ifeq ($(FS),FLOGFS)
C_SRCS += \
../support/FLogFs/src/flogfs.c \
../support/w25n01gwtbig/src/w25n01gwtbig.c \
storage/flogfs_w25m02/flogfs_glue.c \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/src/nrfx_spim.c \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/src/nrfx_spi.c \
$(nRF5_SDK_DIR)/integration/nrfx/legacy/nrf_drv_spi.c \
$(nRF5_SDK_DIR)/modules/nrfx/drivers/src/prs/nrfx_prs.c \

INCLUDEPATHS += \
../support/FLogFs/inc \
../support/w25n01gwtbig/inc \
storage/flogfs_w25m02

BINARY_BLOBS +=\
storage/flogfs_w25m02/README.TXT

CFLAGS += -DFS_START=flogfs_glue_start
endif

ASM_FILES = $(notdir $(ASM_SRCS))
ASM_OBJS = $(addprefix $(OBJ_DIR)/, $(ASM_FILES:.S=.o))
ASM_PATHS = $(sort $(dir $(ASM_SRCS)))

C_FILES = $(notdir $(C_SRCS))
C_OBJS = $(addprefix $(OBJ_DIR)/, $(C_FILES:.c=.o))
C_PATHS = $(sort $(dir $(C_SRCS)))

BINARY_BLOB_FILES = $(notdir $(BINARY_BLOBS))
BINARY_BLOB_OBJS = $(addprefix $(OBJ_DIR)/, $(addsuffix .blob,$(BINARY_BLOB_FILES)))
BINARY_BLOB_PATHS = $(sort $(dir $(BINARY_BLOBS)))

DEP = $(C_OBJS:%.o=%.d)
vpath %.c $(C_PATHS)
vpath %.S $(ASM_PATHS)
vpath % $(BINARY_BLOB_PATHS)

APP = usb_mtp

BIN = $(APP).bin
ELF = $(APP).elf
HEX = $(APP).hex

REVISION = $(shell git describe  --tags)
ifneq ($(REVISION),)
CFLAGS += -DUSB_MTP_FW_VERSION=\"$(REVISION)\"
endif
ARTIFACTS = $(BIN) $(ELF) $(HEX)

####################################################################
# Make Targets                                                     #
####################################################################
all: $(ARTIFACTS)

$(OBJ_DIR):
	@echo [ MKDIR ] $@
	@mkdir "$@"
	
$(BIN): $(ELF)
	@$(OBJCOPY) -O binary $< $@
	
$(HEX): $(ELF)
	@$(OBJCOPY) -O ihex $< $@
	
$(ELF): $(OBJ_DIR) $(C_OBJS) $(ASM_OBJS) $(BINARY_BLOB_OBJS)
	@echo [ LD ] $@
	@$(GCC) $(LDFLAGS) -o "$@" $(C_OBJS) $(ASM_OBJS) $(BINARY_BLOB_OBJS)
	
-include $(DEP)

$(OBJ_DIR)/%.o: %.S
	@echo [ AS ] $<
	@$(GCC) $(CFLAGS) -o "$@" "$<"
	
$(OBJ_DIR)/%.o: %.c
	@echo [ CC ] $< 
	@$(GCC) $(CFLAGS) -MMD $(addprefix -I,$(INCLUDEPATHS)) -o "$@" "$<"
	
$(OBJ_DIR)/%.blob: %
	@echo [ BLOB ] $< 
	@$(LD) -r -b binary -o "$@" "$<"

flash: $(BIN)
ifeq ($(LOCATION),SPECIAL)
	$(JLINK) -device NRF52840_XXAA -if SWD -speed 4000 -autoconnect 1 -CommanderScript flash.jlink
else
	$(UTIL_DIR)/app_switch usb_dfu_bl
	$(UTIL_DIR)/usb-dfu/dfu-util -a $(LOCATION) -D $(BIN) -R
endif

clean:
	@echo "Cleaning..."
	@$(RM) $(ARTIFACTS) $(OBJ_DIR)

.PHONY: all clean flash $(ARTIFACTS)

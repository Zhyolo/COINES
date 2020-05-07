OBJ_DIR = build

CROSS_COMPILE = arm-none-eabi-
GCC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
OBJCOPY = $(CROSS_COMPILE)objcopy


ifeq ($(OS),Windows_NT)
    IS_GCC_FOUND = $(shell where $(GCC))
    JLINK = JLink
    $(info Platform: Windows)
    ifeq ($(notdir $(MAKE)),mingw32-make)
        SHELL = cmd
        RM  = del /s /q
    else
        RM = rm -rf
    endif
else
    $(info Platform: Linux)
    IS_GCC_FOUND = $(shell which $(GCC))
    JLINK = JLinkExe
    RM = rm -rf
endif

LD_SCRIPT = usb_dfu_bootloader.ld

CFLAGS += -DNRF52840_XXAA -D__HEAP_SIZE=8192 -D__STACK_SIZE=8192 
CFLAGS += -std=c99 -mthumb -mabi=aapcs -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -c -Os -g -Wall

LDFLAGS += -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -flto -pipe -specs=nano.specs -T $(LD_SCRIPT) -Xlinker -Map=$(OBJ_DIR)/build.map

nRF5_SDK_DIR = ../nRF5_SDK_15.2.0

ASM_SRCS = $(nRF5_SDK_DIR)/modules/nrfx/mdk/gcc_startup_nrf52840.S

C_SRCS += \
usb_dfu_bootloader.c \
usb_dfu/usb_dfu.c \
usb_core/usb_core.c \
$(nRF5_SDK_DIR)/modules/nrfx/mdk/system_nrf52840.c \
$(nRF5_SDK_DIR)/modules/nrfx/hal/nrf_nvmc.c \
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
usb_dfu \
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

ASM_FILES = $(notdir $(ASM_SRCS))
ASM_OBJS = $(addprefix $(OBJ_DIR)/, $(ASM_FILES:.S=.o))
ASM_PATHS = $(sort $(dir $(ASM_SRCS)))

C_FILES = $(notdir $(C_SRCS))
C_OBJS = $(addprefix $(OBJ_DIR)/, $(C_FILES:.c=.o))
C_PATHS = $(sort $(dir $(C_SRCS)))
DEP = $(C_OBJS:%.o=%.d)
vpath %.c $(C_PATHS)
vpath %.S $(ASM_PATHS)

APP = usb_dfu_bootloader

BIN = $(APP).bin
ELF = $(APP).elf
HEX = $(APP).hex

REVISION = $(shell git describe  --tags)
ifneq ($(REVISION),)
CFLAGS += -DBOOTLOADER_VERSION=\"$(REVISION)\"
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
	
$(ELF): $(OBJ_DIR) $(C_OBJS) $(ASM_OBJS)
	@echo [ LD ] $@
	@$(GCC) $(LDFLAGS) -o "$@" $(C_OBJS) $(ASM_OBJS)
	
-include $(DEP)

$(OBJ_DIR)/%.o: %.S
	@echo [ AS ] $<
	@$(GCC) $(CFLAGS) -o "$@" "$<"
	
$(OBJ_DIR)/%.o: %.c
	@echo [ CC ] $< 
	@$(GCC) $(CFLAGS) -MMD $(addprefix -I,$(INCLUDEPATHS)) -o "$@" "$<"

flash: $(HEX)
	$(JLINK) -device NRF52840_XXAA -if SWD -speed 4000 -autoconnect 1 -CommanderScript flash.jlink

clean:
	@echo "Cleaning..."
	@$(RM) $(ARTIFACTS) $(OBJ_DIR)

.PHONY: all clean flash $(ARTIFACTS)

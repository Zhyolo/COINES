TARGET ?= PC

OBJ_DIR = build/$(TARGET)

ifneq ($(TARGET),$(filter $(TARGET),PC MCU_APP20 MCU_APP30))
    $(info Unsupported 'TARGET' : $(TARGET))
    $(info Supported 'TARGET's : PC, MCU_APP20, MCU_APP30)
    $(error Exit)
endif

ifneq ($(LOCATION),$(filter $(LOCATION),RAM FLASH))
    $(info Unsupported 'LOCATION' : $(LOCATION))
    $(info Supported 'LOCATION's : RAM, FLASH)
    $(error Exit)
endif

# Compiler optimization level
OPT ?= -Os

# Debug flags, Set to 0 (disable) or 1 (enable)
DEBUG ?= 0
ifeq ($(DEBUG),0)
CFLAGS += -U DEBUG -D NDEBUG 
else
CFLAGS += -D DEBUG -U NDEBUG
endif

################################ MCU Target common - APP2.0,APP3.0 ############################
ifeq ($(TARGET),$(filter $(TARGET),MCU_APP20 MCU_APP30))
    CFLAGS += -std=c99 -mthumb -mabi=aapcs -mcpu=cortex-m4 -c $(OPT) -g3 -Wall \
	          -Dprintf=iprintf -Dscanf=iscanf -D$(TARGET)
    CPPFLAGS += -mthumb -mabi=aapcs -mcpu=cortex-m4 -c $(OPT) -g3 -Wall -D$(TARGET)

    LDFLAGS += -mthumb -mcpu=cortex-m4 -specs=nano.specs -Wl,--cref -Wl,--check-sections \
               -Wl,--gc-sections -Wl,--entry=Reset_Handler -Wl,--unresolved-symbols=report-all \
               -Xlinker -Map=$(OBJ_DIR)/build.map -Wl,--start-group -u _printf_float -u _exit -Wl,--end-group

    CROSS_COMPILE = arm-none-eabi-
    # LOCATION ?= FLASH
    LOCATION ?= RAM
    EXT = .elf
endif
#############################################################################################

ifeq ($(TARGET),PC)
CFLAGS += -std=gnu99 -c -g3 $(OPT) -D$(TARGET) -Wall
CPPFLAGS += -c -g3 $(OPT) -D$(TARGET) -Wall
CROSS_COMPILE =
endif

AS = $(CROSS_COMPILE)as
CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar
OBJCOPY = $(CROSS_COMPILE)objcopy

ifeq ($(OS),Windows_NT)
    IS_CC_FOUND = $(shell where $(CC))
    $(info Platform: Windows)
    PLATFORM = PLATFORM_WINDOWS
    LIB_PATH ?= $(COINES_INSTALL_PATH)/coines_api/pc/comm_driver/libusb-1.0/mingw_lib
    DRIVER ?= LEGACY_USB_DRIVER
    ifneq ($(IS_CC_FOUND),)
      GCC_TARGET = $(shell gcc -dumpmachine)
    endif

    ifneq (,$(findstring x86_64,$(GCC_TARGET)))
        LIB_PATH_ARCH ?= $(LIB_PATH)/x64
    else
        LIB_PATH_ARCH ?= $(LIB_PATH)/x86
        EXTRA_LIBS += -lpatch 
    endif
    ifeq ($(notdir $(MAKE)),mingw32-make)
        SHELL = cmd
        CP  = copy
        RM  = del /s /q
        MKDIR = mkdir
        syspath = $(subst /,\,$(1))
    else
        CP = cp
        RM = rm -rf
        MKDIR = mkdir -p
        syspath = $(subst /,/,$(1))
    endif
    DFU = $(COINES_INSTALL_PATH)/util/usb-dfu/dfu-util
else
    IS_CC_FOUND = $(shell which $(CC))
    $(info Platform: Linux / macOS)
    PLATFORM = PLATFORM_LINUX
    DRIVER = LIBUSB_DRIVER
    RM = rm -rf
    DFU = dfu-util
    MKDIR = mkdir -p
    syspath = $(subst /,/,$(1))
endif

APP_SWITCH = $(COINES_INSTALL_PATH)/util/app_switch

#################################  Common - PC and MCU  #####################################

ifeq ($(IS_CC_FOUND),)
    $(error cc: $(CC) not found / Add $(CC) Folder to PATH)
else
    $(info cc:  "$(IS_CC_FOUND)".)
endif

ifeq ($(suffix $(EXAMPLE_FILE)),.S)
ASM_SRCS += $(EXAMPLE_FILE)
endif

ifeq ($(suffix $(EXAMPLE_FILE)),.c)
C_SRCS += $(EXAMPLE_FILE)
endif

ifeq ($(suffix $(EXAMPLE_FILE)),.cpp)
CPP_SRCS += $(EXAMPLE_FILE)
endif

PROJ_NAME = $(basename $(EXAMPLE_FILE))
EXE =  $(PROJ_NAME)$(EXT)
BIN =  $(PROJ_NAME).bin

SENSOR = $(SHUTTLE_BOARD)
ifneq ($(SENSOR),)
include $(COINES_INSTALL_PATH)/sensorAPI/sensors.mk
endif

INCLUDEPATHS += $(COINES_INSTALL_PATH)/coines_api

LIBPATHS += \
$(LIB_PATH_ARCH) \
$(COINES_INSTALL_PATH)/coines_api \

################################ MCU Target - APP2.0 specific ###############################

ifeq ($(TARGET),MCU_APP20)
    DEVICE = APP2.0-DFU

        ifeq ($(LOCATION),RAM)
            LD_SCRIPT = $(COINES_INSTALL_PATH)/coines_api/mcu_app20/mcu_app20_ram.ld
        else
            LD_SCRIPT = $(COINES_INSTALL_PATH)/coines_api/mcu_app20/mcu_app20_flash.ld
        endif

    LDFLAGS +=  -T $(LD_SCRIPT)
    LIBS += coines-mcu_app20
    ARTIFACTS = $(EXE) $(BIN)

endif

################################ MCU Target - APP3.0 specific #################################

ifeq ($(TARGET),MCU_APP30)
    DEVICE = APP3.0-DFU
        ifeq ($(LOCATION),RAM)
            LD_SCRIPT = $(COINES_INSTALL_PATH)/coines_api/mcu_app30/linker_scripts/mcu_app30_ram.ld
        else
            LD_SCRIPT = $(COINES_INSTALL_PATH)/coines_api/mcu_app30/linker_scripts/mcu_app30_flash.ld
        endif
    CFLAGS +=  -mfloat-abi=hard -mfpu=fpv4-sp-d16
    CPPFLAGS +=  -mfloat-abi=hard -mfpu=fpv4-sp-d16
    LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16 -T $(LD_SCRIPT)

    # Doesn't work for some reason !
    # LIBS += coines-mcu_app30
    LDFLAGS += -Wl,--whole-archive -L $(COINES_INSTALL_PATH) -lcoines-mcu_app30 -Wl,--no-whole-archive
    ARTIFACTS = $(EXE) $(BIN)

endif

################################ PC Target - Windows,Linux/macOS ############################

ifeq ($(TARGET),PC)
    CFLAGS += -D$(PLATFORM)
    CPPFLAGS += -D$(PLATFORM)
    LIBS += coines-pc

    ifeq ($(PLATFORM),PLATFORM_LINUX)
        LIBS += pthread
    endif

    ifeq ($(DRIVER),LEGACY_USB_DRIVER)
        LIBS += setupapi
    endif

    ifeq ($(DRIVER),LIBUSB_DRIVER)
        LIBS += usb-1.0
    endif

    ARTIFACTS = $(EXE)
endif

#############################################################################################

LIBS += m

ASM_FILES = $(notdir $(ASM_SRCS))
ASM_OBJS += $(addprefix $(OBJ_DIR)/, $(ASM_FILES:.S=.S.o))
ASM_PATHS = $(sort $(dir $(ASM_SRCS)))
vpath %.S $(ASM_PATHS)

C_FILES = $(notdir $(C_SRCS))
C_OBJS += $(addprefix $(OBJ_DIR)/, $(C_FILES:.c=.c.o))
C_PATHS = $(sort $(dir $(C_SRCS)))
DEP = $(C_OBJS:%.o=%.d)
vpath %.c $(C_PATHS)

CPP_FILES = $(notdir $(CPP_SRCS))
CPP_OBJS += $(addprefix $(OBJ_DIR)/, $(CPP_FILES:.cpp=.cpp.o))
CPP_PATHS = $(sort $(dir $(CPP_SRCS)))
DEP = $(CPP_OBJS:%.o=%.d)
vpath %.cpp $(CPP_PATHS)

#Load contents of cflags.save file.
#Populates the the CFLAGS_SAVE variable
-include $(OBJ_DIR)/cflags.save


#Compare  CFLAGS_SAVE with CFLAGS,if they differ perform a clean build
#If CFLAGS_SAVE is empty, don't do anything
ifneq ($(CFLAGS_SAVE),)
ifneq ($(strip $(CFLAGS)),$(strip $(CFLAGS_SAVE)))
ifneq (,$(shell $(RM) $(OBJ_DIR)))
$(info Cleaning...)
endif
endif
endif

####################################################################
# Make Targets                                                     #
####################################################################
all: $(ARTIFACTS)
	@echo CFLAGS_SAVE = $(CFLAGS) > $(OBJ_DIR)/cflags.save
	$(call syspath,$(POSTBUILD_CMD))

$(OBJ_DIR):
	@echo [ MKDIR ] $@
	@$(MKDIR) $(call syspath,$@)

$(BIN): $(EXE)
	@echo [ BIN ] $@
	@$(OBJCOPY) -O binary $< $@

$(EXE): $(OBJ_DIR) $(C_OBJS) $(CPP_OBJS) $(ASM_OBJS)
	@echo [ MAKE ] coines_api
	@$(MAKE) -s -C  $(COINES_INSTALL_PATH)/coines_api TARGET=$(TARGET) OPT=$(OPT) DEBUG=$(DEBUG) ZEUS_QUIRK=$(ZEUS_QUIRK)
	@echo [ LD ] $@
	@$(CC) $(LDFLAGS) -o "$@" $(C_OBJS) $(CPP_OBJS) $(ASM_OBJS) $(addprefix -L,$(LIBPATHS)) $(addprefix -l,$(LIBS)) $(EXTRA_LIBS)

-include $(DEP)

$(OBJ_DIR)/%.S.o: %.S
	@echo [ AS ] $<
	@$(CC) $(CFLAGS) -o "$@" "$<"

$(OBJ_DIR)/%.c.o: %.c
	@echo [ CC ] $<
	@$(CC) $(CFLAGS) -MMD $(addprefix -I,$(INCLUDEPATHS)) -o "$@" "$<"

$(OBJ_DIR)/%.cpp.o: %.cpp
	@echo [ CXX ] $<
	@$(CXX) $(CPPFLAGS) -MMD $(addprefix -I,$(INCLUDEPATHS)) -o "$@" "$<"

ifeq ($(TARGET),$(filter $(TARGET),MCU_APP20 MCU_APP30))
download: $(BIN)
	@$(APP_SWITCH) usb_dfu_bl
	@echo [ DFU ] $<
	@$(DFU) --serial $(DEVICE) -a $(LOCATION) -D $(BIN) -R
endif

run:
	@$(APP_SWITCH) example

clean:
	@echo "Cleaning..."
	@$(RM) $(ARTIFACTS) $(PROJ_NAME).exe $(call syspath,$(OBJ_DIR))

clean-all: clean
	@$(RM) build $(PROJ_NAME) $(PROJ_NAME).elf $(PROJ_NAME).exe $(PROJ_NAME).bin
	@$(MAKE) -s -C  $(COINES_INSTALL_PATH)/coines_api clean

.PHONY: all clean clean-all download $(ARTIFACTS)
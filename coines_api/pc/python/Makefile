COMPILER_PREFIX ?=
CC = $(COMPILER_PREFIX)gcc
LD = $(COMPILER_PREFIX)gcc
TARGET = PC

ARCH = $(shell $(CC) -dumpmachine)

ifneq (,$(findstring x86_64,$(ARCH)))
     SUFFIX = 64
else
     SUFFIX = 32
endif

ifeq ($(OS),Windows_NT)
    C_SRCS_COINES += comm_driver/legacy_usb/legacy_usb_support.c
    INCLUDEPATHS_COINES += comm_driver/legacy_usb
    CFLAGS += -D PLATFORM_WINDOWS -D LEGACY_USB_DRIVER
    VARIANT = win$(SUFFIX)
    EXT = dll
    USB_LIB += -lsetupapi
    ifeq ($(notdir $(MAKE)),mingw32-make)
        SHELL = cmd
        RM  = del /s /q
        MKDIR = mkdir
        syspath = $(subst /,\,$(1))
    else
        RM = rm -rf
        MKDIR = mkdir -p
        syspath = $(subst /,/,$(1))
    endif
else
    INCLUDEPATHS_COINES += comm_driver/libusb-1.0
    MKDIR = mkdir -p
    RM  = rm -rf
    syspath = $(subst /,/,$(1))
    CFLAGS += -D PLATFORM_LINUX -D LIBUSB_DRIVER -fPIC
    EXT = so
    VARIANT = linux$(SUFFIX)
    USB_LIB += -lusb-1.0
endif

OBJ_DIR = build/$(VARIANT)

SRC_DIR = ..

C_SRCS_COINES += \
coines.c \
comm_intf/comm_intf.c \
comm_intf/comm_ringbuffer.c \
comm_driver/usb.c \

INCLUDEPATHS_COINES += \
. \
.. \
coines_api \
comm_intf \
comm_driver \

C_SRCS = $(addprefix $(SRC_DIR)/,$(C_SRCS_COINES))
INCLUDEPATHS = $(addprefix $(SRC_DIR)/,$(INCLUDEPATHS_COINES))

C_FILES = $(notdir $(C_SRCS))
C_OBJS = $(addprefix $(OBJ_DIR)/, $(C_FILES:.c=.c.o))
C_PATHS = $(sort $(dir $(C_SRCS)))
DEP = $(C_OBJS:%.o=%.d)

vpath %.c $(C_PATHS)

CFLAGS += -c -D $(TARGET)
LDFLAGS += -shared

SHARED_LIB += coinespy/libcoines_$(SUFFIX).$(EXT)

all: $(SHARED_LIB)
	@echo CFLAGS_SAVE = $(CFLAGS) > $(OBJ_DIR)/cflags.save

$(OBJ_DIR):
	@echo [ MKDIR ] $@
	@$(MKDIR) $(call syspath,$@)
	
$(SHARED_LIB): $(OBJ_DIR) $(C_OBJS)
	@echo [ LD ] $@
	@$(CC) $(LDFLAGS) -o "$@" $(C_OBJS) $(USB_LIB)

-include $(DEP)

$(OBJ_DIR)/%.c.o: %.c
	@echo [ CC ] $< 
	@$(CC) $(CFLAGS) -MMD -I . $(addprefix -I,$(INCLUDEPATHS)) -o "$@" "$<"

clean:
	@echo "Cleaning..."
	@$(RM) $(call syspath,$(SHARED_LIB)) build
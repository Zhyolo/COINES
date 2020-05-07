# -*- coding: utf-8 -*-
"""
(c) Bosch Sensortec GmbH, Reutlingen, Germany

@author: kro1rt

pylint3 --disable=missing-docstring,too-few-public-methods,invalid-name,\
attribute-defined-outside-init,redefined-outer-name __init__.py --> 9.13/10
"""
from __future__ import print_function
import ctypes as ct
import math
import os
import os.path
import platform
import sys

def my_os_arch():
    os_arch = platform.architecture()[0]
    return os_arch

def load_coines_lib():
    path_list = []
    coinespy_path = os.path.dirname(__file__)
    if platform.system() == 'Windows':
        path_list.append('libcoines.dll')
        if my_os_arch() == '64bit':
            path_list.append(os.path.join(coinespy_path, 'libcoines_64.dll'))
        else:
            path_list.append(os.path.join(coinespy_path, 'libcoines_32.dll'))

    elif platform.system() == 'Linux':
        path_list.append('libcoines.so')
        if 'armv7l' in platform.uname():
            path_list.append(os.path.join(coinespy_path, 'libcoines_armv7_32.so'))
        elif my_os_arch() == '64bit':
            path_list.append(os.path.join(coinespy_path, 'libcoines_64.so'))
        else:   # 32bit
            path_list.append(os.path.join(coinespy_path, 'libcoines_32.so'))
    else: # macOS library loading
        path_list.append('libcoines.dylib')
        path_list.append(os.path.join(coinespy_path, 'libcoines.dylib'))

    ret = -1
    for path in path_list:
        try:
            loadedlib = ct.cdll.LoadLibrary(path)
            ret = 0
        except OSError:
            ret = -1
        if ret == 0:
            break

    if ret == -1:
        return None
    return loadedlib

class EONOFF:
    OFF = 0
    ON = 1

class PINMODE:
    INPUT = 0       # COINES_PIN_DIRECTION_IN = 0
    OUTPUT = 1

class PINLEVEL:
    LOW = 0         # COINES_PIN_VALUE_LOW = 0
    HIGH = 1

class PCINTERFACE:
    USB = 0     # COINES_COMM_INTF_USB
    SERIAL = 1  # COINES_COMM_INTF_VCOM

class I2CSPEED:
    STANDARDMODE = 0    # Standard mode - 100kHz
    FASTMODE = 1        # Fast mode - 400kHz
    HSMODE = 2          # High Speed mode - 3.4 MHz
    HSMODE2 = 3         # High Speed mode 2 - 1.7 MHz

class SPISPEED:
    SPI250KBIT = 240
    SPI300KBIT = 200
    SPI400KBIT = 150
    SPI500KBIT = 120
    SPI600KBIT = 100
    SPI750KBIT = 80
    SPI1000KBIT = 60
    SPI1200KBIT = 50
    SPI1250KBIT = 48
    SPI1500KBIT = 40
    SPI2000KBIT = 30
    SPI2500KBIT = 24
    SPI3000KBIT = 20
    SPI3750KBIT = 16
    SPI5000KBIT = 12
    SPI6000KBIT = 10
    SPI7500KBIT = 8
    SPI10000KBIT = 6

class SPIBITS:
    SPI8BIT = 8 # 8 bit register read/write
    SPI16BIT = 16   # 16 bit register read/write

class SPIMODE:
    MODE0 = 0       # SPI Mode 0: CPOL=0; CPHA=0
    MODE1 = 1       # SPI Mode 1: CPOL=0; CPHA=1
    MODE2 = 2       # SPI Mode 2: CPOL=1; CPHA=0
    MODE3 = 3       # SPI Mode 3: CPOL=1; CPHA=1

# Replication of coines_multi_io_pin from coines.h
class ShuttleBoardPin:
    COINES_SHUTTLE_PIN_7 = 9    # CS pin
    COINES_SHUTTLE_PIN_8 = 5    # Multi-IO 5
    COINES_SHUTTLE_PIN_9 = 0    # Multi-IO 0
    COINES_SHUTTLE_PIN_14 = 1   # Multi-IO 1
    COINES_SHUTTLE_PIN_15 = 2   # Multi-IO 2
    COINES_SHUTTLE_PIN_16 = 3   # Multi-IO 3
    COINES_SHUTTLE_PIN_19 = 8   # Multi-IO 8
    COINES_SHUTTLE_PIN_20 = 6   # Multi-IO 6
    COINES_SHUTTLE_PIN_21 = 7   # Multi-IO 7
    COINES_SHUTTLE_PIN_22 = 4   # Multi-IO 4

    # APP3.0 pins
    COINES_MINI_SHUTTLE_PIN_1_4 = 0x10  # GPIO0
    COINES_MINI_SHUTTLE_PIN_1_5 = 0x11  # GPIO1
    COINES_MINI_SHUTTLE_PIN_1_6 = 0x12  # GPIO2/INT1
    COINES_MINI_SHUTTLE_PIN_1_7 = 0x13  # GPIO3/INT2
    COINES_MINI_SHUTTLE_PIN_2_5 = 0x14  # GPIO4
    COINES_MINI_SHUTTLE_PIN_2_6 = 0x15  # GPIO5
    COINES_MINI_SHUTTLE_PIN_2_1 = 0x16  # CS
    COINES_MINI_SHUTTLE_PIN_2_3 = 0x17  # SDO

# For backward compatibility
class MULTIIO:
    MULTIIO_0 = 0
    MULTIIO_1 = 1
    MULTIIO_2 = 2
    MULTIIO_3 = 3
    MULTIIO_4 = 4
    MULTIIO_5 = 5
    MULTIIO_6 = 6
    MULTIIO_7 = 7
    MULTIIO_8 = 8

# Old style naming for APP3.0 pins
class GPIO:
    GPIO_0 = 0x10
    GPIO_1 = 0x11
    GPIO_2 = 0x12
    GPIO_3 = 0x13
    GPIO_4 = 0x14
    GPIO_5 = 0x15

# Assign the field values inside the corresponding streaming setting function
class PinConfigInfo(ct.Structure):
    _fields_ = [('direction', ct.c_uint16),
                ('switchState', ct.c_uint16),
                ('level', ct.c_uint16)]

class BoardInfo(ct.Structure):
    _fields_ = [('HardwareId', ct.c_uint16),   #variable for storing the hardware ID.
                ('SoftwareId', ct.c_uint16),   #variable for storing the software ID.
                ('Board', ct.c_uint8),         #variable for storing the board information.
                ('ShuttleID', ct.c_uint16)]    #variable to store the shuttle ID.

class UserApplicationBoard(object):
    def __init__(self, path_to_coineslib=None):
        self.LIB = None
        self.PCinterface = -1
        self.ERRORCODE = 0
        self.currentVDD = 0
        self.currentVDDIO = 0
        self.sensorInterfaceDetail = -1
        self.sensorInterface = 'i2c'
        self.spi16bitEnable = False

        if path_to_coineslib != None:
            try:
                self.LIB = ct.cdll.LoadLibrary(path_to_coineslib)
            except OSError:
                print('Cound not load coineslib shared library: %s' % path_to_coineslib)
                sys.exit(-1)

        if self.LIB is None:
            self.LIB = load_coines_lib()

        if self.LIB is None:
            print(r"""
            Could not load coineslib shared library. Please intialize the
            UserApplicationBoard object with the path to the library, e.g.
            board = BST.UserApplicationBoard(r'libcoines.dll')
            """)
            sys.exit(-1)

    def wrap_function(self, funcname, restype, argtypes):
        func = self.LIB.__getattr__(funcname)
        func.restype = restype
        func.argtypes = argtypes
        return func

    def PCInterfaceConfig(self, interf):
        self.PCinterface = interf
        ret = self.LIB.coines_open_comm_intf(interf)
        if ret != 0:
            self.ERRORCODE = -1
        else:
            self.ERRORCODE = 0

    def ClosePCInterface(self):
        ret = self.LIB.coines_close_comm_intf(self.PCinterface)
        if ret != 0:
            self.ERRORCODE = -1
        else:
            self.ERRORCODE = 0

    def GetBoardInfo(self):
        brd_info = BoardInfo()
        ret = self.LIB.coines_get_board_info(ct.pointer(brd_info))
        if ret != 0:
            self.ERRORCODE = ret
        else:
            self.ERRORCODE = 0
        return brd_info

    def PinConfig(self, pinNumber, switchState, direction, outputState):
        pinconf_func = self.wrap_function('coines_set_pin_config', ct.c_uint8,\
                                        [ct.c_uint8, ct.c_uint8, ct.c_uint8])
        if switchState == EONOFF.OFF:
            ret = pinconf_func(pinNumber, PINMODE.INPUT, PINLEVEL.LOW)
        else:
            ret = pinconf_func(pinNumber, direction, outputState)
        if ret != 0:
            print('PinConfig FAIL')
            self.ERRORCODE = -1
        else:
            self.ERRORCODE = 0

    def GetPinConfig(self, pinNumber):
        pin_info = PinConfigInfo()
        pt_direction = ct.c_uint16()
        pt_level = ct.c_uint16()
        getpin_func = self.wrap_function('coines_get_pin_config', ct.c_uint8,\
                    [ct.c_int, ct.POINTER(ct.c_uint16), ct.POINTER(ct.c_uint16)])
        ret = getpin_func(pinNumber, ct.byref(pt_direction), ct.byref(pt_level))
        if ret != 0:
            self.ERRORCODE = -1
        else:
            self.ERRORCODE = 0
            pin_info.level = pt_level.value
            pin_info.direction = pt_direction.value
            if pin_info.level == PINLEVEL.LOW and pin_info.direction == PINMODE.INPUT:
                pin_info.switchState = EONOFF.OFF
            else:
                pin_info.switchState = EONOFF.ON
        return pin_info

    def SetVDD(self, value):
        self.currentVDD = ct.c_ushort(int(value * 1000))
        ret = self.LIB.coines_set_shuttleboard_vdd_vddio_config(self.currentVDD, self.currentVDDIO)
        if ret != 0:
            self.ERRORCODE = -1
        else:
            self.ERRORCODE = 0

    def SetVDDIO(self, value):
        self.currentVDDIO = ct.c_ushort(int(value * 1000))
        ret = self.LIB.coines_set_shuttleboard_vdd_vddio_config(self.currentVDD, self.currentVDDIO)
        if ret != 0:
            self.ERRORCODE = -1
        else:
            self.ERRORCODE = 0

    def SensorI2CConfig(self, i2cAddress, speed):
        self.sensorInterfaceDetail = i2cAddress
        self.sensorInterface = 'i2c'
        self.spi16bitEnable = False
        self._intern_config_i2c_bus(speed)

    def _intern_config_i2c_bus(self, speed):
        ret = self.LIB.coines_config_i2c_bus(0, speed) # Always use bus COINES_I2C_BUS_0,
                                                  # more are not available on APP2.0
        if ret != 0:
            self.ERRORCODE = -1
        else:
            self.ERRORCODE = 0

    def SensorSPIConfig(self, chipSelectPin, spiSpeed=60, spiMode=SPIMODE.MODE0):
        self.sensorInterfaceDetail = chipSelectPin
        self.sensorInterface = 'spi'
        self.spi16bitEnable = False
        self._intern_config_spi_bus(spiSpeed, spiMode)

    def _intern_config_spi_bus(self, spiSpeed, spiMode):
        ret = self.LIB.coines_config_spi_bus(0, spiSpeed, spiMode)
        if ret != 0:
            self.ERRORCODE = -1
        else:
            self.ERRORCODE = 0

    def Sensor16bitSPIConfig(self, chipSelectPin, spiSpeed=60, spiMode=SPIMODE.MODE0, \
                             spiBits=SPIBITS.SPI16BIT):
        self.sensorInterfaceDetail = chipSelectPin
        self.sensorInterface = 'spi'
        self.spi16bitEnable = True
        self._intern_config_16bitspi_bus(spiSpeed, spiMode, spiBits)

    def _intern_config_16bitspi_bus(self, spiSpeed, spiMode, spiBits):
        ret = self.LIB.coines_config_word_spi_bus(0, spiSpeed, spiMode, spiBits)
        if ret != 0:
            self.ERRORCODE = -1
        else:
            self.ERRORCODE = 0

    def CustomSPIConfig(self, chipSelectPin, spiSpeed, spiMode=SPIMODE.MODE0):
        self.sensorInterfaceDetail = chipSelectPin
        customSpiSpeed = math.ceil(60 * (10**6) / spiSpeed)
        self._intern_config_spi_bus(customSpiSpeed, spiMode)

    def Read(self, registerAddress, numberofReads=1, sensorInterfaceDetail=None):
        if sensorInterfaceDetail != None:
            interface = sensorInterfaceDetail
        else:
            interface = self.sensorInterfaceDetail
        if self.spi16bitEnable is False:
            reg_data = ct.create_string_buffer(numberofReads)
        else:
            reg_data = (ct.c_int16 * numberofReads)()

        if self.sensorInterface == 'i2c':      # I2C bus configured
            ret = self.LIB.coines_read_i2c(interface, int(registerAddress),\
                                                ct.byref(reg_data), int(numberofReads))
        elif self.sensorInterface == 'spi':                       # SPI bus configured
            if self.spi16bitEnable is False:
                ret = self.LIB.coines_read_spi(interface, int(registerAddress),\
                                                ct.byref(reg_data), int(numberofReads))
            else:
                ret = self.LIB.coines_read_16bit_spi(interface, registerAddress, \
                        ct.cast(reg_data, ct.POINTER(ct.c_int16)), int(numberofReads))
        else:
            raise 'No bus configured'

        if ret != 0:
            self.ERRORCODE = -1
        else:
            self.ERRORCODE = 0
        if self.spi16bitEnable is False:
            reg_data = [int.from_bytes(i, byteorder='big') for i in reg_data]

        return reg_data

    def Read16bitSPI(self, registerAddress, numberofReads=2, sensorInterfaceDetail=None):

        if sensorInterfaceDetail != None:
            interface = sensorInterfaceDetail
        else:
            interface = self.sensorInterfaceDetail
        #reg_data = ct.create_string_buffer(numberofReads)

        array_type = (ct.c_int16 * numberofReads)()

        self.LIB.coines_read_16bit_spi.argtypes = \
                        [ct.c_char, ct.c_int16, ct.POINTER(ct.c_int16), ct.c_int16]

        if self.sensorInterface == 'spi': # SPI bus configured
            ret = self.LIB.coines_read_16bit_spi(interface, int(registerAddress),\
                    ct.cast(array_type, ct.POINTER(ct.c_int16)), int(numberofReads))
        else:
            raise 'No bus configured'

        if ret != 0:
            self.ERRORCODE = -1
        else:
            self.ERRORCODE = 0

        return array_type

    def Write(self, registerAddress, registerValue, sensorInterfaceDetail=None):
        """
            Backward compatibility with GenericAPI: allow registerValue to be
            either a list or an integer. However, the user should prefer to write one
            byte at a time or consider a long delay after the write and check for
            success by reading back.
        """
        if isinstance(registerValue, int):
            reg_length = 1
            if self.spi16bitEnable is False:
                write_data = ct.create_string_buffer(reg_length)
            else:
                write_data = (ct.c_int16 * 1)()
            write_data[0] = registerValue

        elif isinstance(registerValue, list):
            reg_length = len(registerValue)
            if self.spi16bitEnable is False:
                write_data = ct.create_string_buffer(reg_length)
            else:
                write_data = (ct.c_int16 * reg_length)()
            for i in range(reg_length):
                write_data[i] = registerValue[i]
        else:
            raise ValueError("Unkown format: registerValue")

        if sensorInterfaceDetail != None:
            interface = sensorInterfaceDetail
        else:
            interface = self.sensorInterfaceDetail

        if self.sensorInterface == 'i2c':      # I2C bus configured
            ret = self.LIB.coines_write_i2c(interface, registerAddress, write_data, reg_length)
        elif self.sensorInterface == 'spi':  # SPI bus configured
            if self.spi16bitEnable is False:
                ret = self.LIB.coines_write_spi(interface, registerAddress, write_data, reg_length)
            else:
                ret = self.LIB.coines_write_16bit_spi(interface, registerAddress,\
                        ct.cast(write_data, ct.POINTER(ct.c_int16)), reg_length)
        else:
            raise 'No bus configured'

        if ret != 0:
            self.ERRORCODE = -1
        else:
            self.ERRORCODE = 0

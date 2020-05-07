USB MTP Firmware : v1.0-beta
Date : 31-07-2019
Filesystem : FlogFS
Board : APP3.0
Manufacturer : Bosch Sensortec GmbH (www.bosch-sensortec.com)

Open source software information
--------------------------------
1.FlogFS
    Link   - https://github.com/conservify/FLogFS
    Author - Ben Nahill <bnahill@gmail.com>

2.Android Open Source Project (AOSP)
    Link - https://android.googlesource.com/platform/frameworks/av/+/android-5.1.1_r8/media/mtp/mtp.h

Known limitations
---------------------------------
  W25M02 NAND driver
        * Memory access is limited to 128MB instead of 256MB
        * Hardware ECC is currently disabled
  MTP stack and FlogFS filesystem
        * Maximum of 128 files is accessible in a session
        * Renaming files not supported
        * File name is limited to 32 characters
        * No support for directories, file timestamps


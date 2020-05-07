This is a proof of concept that MTP can run on nRF52840 microcontrollers !

The below links helped a lot to write the MTP stack
	* https://github.com/yoonghm/MTP
	* https://android.googlesource.com/platform/frameworks/av/+/android-5.1.1_r8/media/mtp/mtp.h
	* https://www.usb.org/document-library/media-transfer-protocol-v11-spec-and-mtp-v11-adopters-agreement

This demo firmware contains 4 other files

Opening/Copying the below files reads the specified flash memory region
 * flash_mem_backup.bin   | 0x30000 - 0xF0000 
 * nrf52840_ble_sd.bin    | 0x00000 - 0x30000  (BLE Softdevice and MBR)
 * usb_dfu_bootloader.bin | 0xF0000 - 0xF4000  (You can retreive the bootloader code !)
 
   Check the version of bootloader using the below command in Git Bash
   $ strings usb_dfu_bootloader.bin | grep info

 * ReleaseNotes_COINES.pdf ( PDF file for testing ) 

~ Jabez Winston Christopher (RBEI/EAC3) 
  <christopher.jabezwinston@in.bosch.com>
  01/07/2019
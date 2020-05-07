/*
Copyright (c) 2013, Ben Nahill <bnahill@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FLogFS Project.
*/

/*!
 * @file flogfs_conf.cpp
 * @author Ben Nahill <bnahill@gmail.com>
 * @ingroup FLogFS
 *
 * @brief Platform-specific interface details
 */

#include "flogfs.h"
#include "w25n01gwtbig.h"
#include <stdlib.h>


typedef uint8_t flash_spare_t[59];

typedef uint8_t fs_lock_t;
#define nullptr NULL


static inline void fs_lock_initialize(fs_lock_t * lock){
	//chMtxInit(lock);
}

static inline void fs_lock(fs_lock_t * lock){
	//chMtxLock(lock);
}

static inline void fs_unlock(fs_lock_t * lock){
	//chMtxUnlock();
}

static flash_spare_t flog_spare_buffer;
static uint16_t flash_block;
static uint16_t flash_page;
static uint8_t have_metadata;
static uint8_t page_open;

static inline flog_result_t flash_initialize(){
	page_open = 0;
	if(W25N01GW_Init()==W25N01GW_INITIALIZED)
	{
		return FLOG_SUCCESS;
	}
	else
	{
		return FLOG_FAILURE;
	}
}

static inline void flash_lock(){
	//flash.lock();
}

static inline void flash_unlock(){
	//flash.unlock();
}

static inline flog_result_t flash_open_page(uint16_t block, uint16_t page){
	flash_block = block;
	flash_page = page;
	have_metadata = 0;
	return FLOG_SUCCESS;
	// Read flash page to cache
	//return FLOG_RESULT(flash.page_open(block, page));
}

static inline void flash_close_page(){
	page_open = 0;
	//flash.unlock();
}

static inline flog_result_t flash_erase_block(uint16_t block){
	W25N01GW_errorCode_t rslt;
	rslt = W25N01GW_eraseBlock((block*W25N01GW_BLOCK_SIZE)+1,W25N01GW_BLOCK_SIZE);
	if(rslt == W25N01GW_ERASE_SUCCESS)
		return FLOG_SUCCESS;
	else
		return FLOG_FAILURE;
}

static inline flog_result_t flash_get_spares(){
//	// Read metadata from flash
//	if(!have_metadata){
//		have_metadata = 1;
//	}
//	return FLOG_RESULT(flash.page_read_continued(flog_spare_buffer, 0x800,
//	                                             sizeof(flog_spare_buffer)));
	return FLOG_SUCCESS;
}

static inline uint8_t * flash_spare(uint8_t sector){
	return &flog_spare_buffer[sector * 16 + 4];
}

static inline flog_result_t flash_block_is_bad(){
//	uint8_t buffer;
//	flash.page_read_continued(&buffer, 0x800, 1);
//	return FLOG_RESULT(buffer == 0);
	return FLOG_RESULT(0);
}

static inline void flash_set_bad_block(){

}

/*!
 @brief Commit the changes to the active page
 */
static inline void flash_commit(){
	page_open = 0;
//	flash.page_commit();
}

/*!
 @brief Read data from the flash cache (current page only)
 @param dst The destination buffer to fill
 @param chunk_in_page The chunk index within the current page
 @param offset The offset data to retrieve
 @param n The number of bytes to transfer
 @return The success or failure of the operation
 */
static inline flog_result_t flash_read_sector(uint8_t * dst, uint8_t sector, uint16_t offset, uint16_t n){
//	if(W25N01GW_read(dst,n,( FS_SECTOR_SIZE * sector) + offset) == W25N01GW_READ_SUCCESS)
	sector = sector%FS_SECTORS_PER_PAGE;
	uint32_t read_loc =(flash_block*FS_SECTOR_SIZE*FS_PAGES_PER_BLOCK*FS_SECTORS_PER_PAGE)+( flash_page*FS_SECTORS_PER_PAGE*FS_SECTOR_SIZE )+( FS_SECTOR_SIZE * sector) + offset;
	if(W25N01GW_read(dst,n,read_loc) == W25N01GW_READ_SUCCESS)
	{
		return FLOG_SUCCESS;
	}
	else
	{
		return FLOG_FAILURE;
	}
}

static inline flog_result_t flash_read_spare(uint8_t * dst, uint8_t sector){
	sector = sector%FS_SECTORS_PER_PAGE;
	if(W25N01GW_read_spare(dst,4,flash_page+(flash_block*64), 0x800+sector * 0x10) == W25N01GW_READ_SUCCESS)
	{
		return FLOG_SUCCESS;
	}
	else
	{
		return FLOG_FAILURE;
	}
}

/*!
 @brief Write chunk data to the flash cache
 @param src A pointer to the data to transfer
 @param chunk_in_page The chunk index within the current page
 @param offset The offset to write the data
 @param n The number of bytes to write
 */
static inline void flash_write_sector(uint8_t const * src, uint8_t sector, uint16_t offset, uint16_t n){
	sector = sector%FS_SECTORS_PER_PAGE;
	uint32_t write_loc =(flash_block*FS_SECTOR_SIZE*FS_PAGES_PER_BLOCK*FS_SECTORS_PER_PAGE)+( flash_page*FS_SECTORS_PER_PAGE*FS_SECTOR_SIZE )+( FS_SECTOR_SIZE * sector) + offset;
	W25N01GW_write(src,n, write_loc);
//	W25N01GW_write(src,n, FS_SECTOR_SIZE * sector + offset);
}


/*!
 @brief Write the spare data for a sector
 @param chunk_in_page The chunk index within the current page

 @note This doesn't commit the transaction
 */
static inline void flash_write_spare(uint8_t const * src, uint8_t sector){
	//flash.page_write_continued(src, 0x804 + sector * 0x10, 4);
	//W25N01GW_write(src,4, (FS_SECTOR_SIZE * sector*0x10));
	uint8_t buff[4];
	sector = sector%FS_SECTORS_PER_PAGE;
	W25N01GW_read_spare(buff,4,flash_page+(flash_block*64), 0x800+sector * 0x10);
	W25N01GW_write_spare(src,4,flash_page+(flash_block*64), 0x800+sector * 0x10);
	W25N01GW_read_spare(buff,4,flash_page+(flash_block*64), 0x800+sector * 0x10);
}

void flash_debug_warn(char const *f, ...){

}

void flash_debug_error(char const *f, ...){

}
void flash_debug_panic() {
}
void flash_high_level(flog_high_level_event_t hle) {
}

uint32_t flash_random(uint32_t max) {
    return max;
}


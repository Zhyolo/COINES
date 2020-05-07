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
 * @file flogfs_private.h
 * @author Ben Nahill <bnahill@gmail.com>
 *
 * @ingroup FLogFS
 *
 * @brief Private definitions for FLogFS internals
 *
 */

#ifndef __FLOGFS_PRIVATE_H_
#define __FLOGFS_PRIVATE_H_

#include "flogfs.h"
#include "flogfs_conf.h"

// KDevelop wants to be a jerk about the __restrict__ keyword
// #ifdef USE_RESTRICT
// 	#if defined(__GNUG__)
// 	#define restrict __restrict__
// 	#elif defined(__GNUC__)
// 	#define restrict restrict
// 	#else
// 	#define restrict
// 	#endif
// #else
// 	#if not defined(__cplusplus)
// 	#define restrict restrict
// 	#else
// 	#define restrict
// 	#endif
// #endif

#define STRING2(x) #x
#define STRING(x) STRING2(x)
#define LINESTR STRING(__LINE__)

#define MAX(a, b) ((a > b) ? a : b)
#define MIN(a, b) ((a > b) ? b : a)

//! @addtogroup FLogPrivate
//! @{

typedef enum { FLOG_STATE_RESET, FLOG_STATE_MOUNTED } flog_state_t;

typedef enum {
    FLOG_PRIME_BEGIN,
    FLOG_PRIME_END,
    FLOG_FORMAT_BEGIN,
    FLOG_FORMAT_END
} flog_high_level_event_t;

#ifdef FLOGFS_ERASE_ZERO
const uint8_t FS_ERASE_CHAR = 0x00;
#else
const uint8_t FS_ERASE_CHAR = 0xff;
#endif

/*!
 @brief A block type stored in the first byte of the first sector spare

 Those values not present represent an error
 */
typedef enum {
    #ifdef FLOGFS_ERASE_ZERO
    FLOG_BLOCK_TYPE_ERROR = 0xff,
    FLOG_BLOCK_TYPE_UNALLOCATED = 0,
    #else
    FLOG_BLOCK_TYPE_ERROR = 0,
    FLOG_BLOCK_TYPE_UNALLOCATED = 0xff,
    #endif
    FLOG_BLOCK_TYPE_INODE = 1,
    FLOG_BLOCK_TYPE_FILE = 2
} flog_block_type_t;

//! @name Invalid values
//! @{
#ifdef FLOGFS_ERASE_ZERO
#define FLOG_BLOCK_IDX_ERASED 0x00
#define FLOG_BLOCK_AGE_ERASED 0x00
#define FLOG_FILE_ID_ERASED 0x00
#define FLOG_TIMESTAMP_ERASED 0x00
#define FLOG_SECTOR_NBYTES_ERASED 0x00
#define FLOG_SECTOR_TYPE_ID_ERASED 0x00

#define FS_FIRST_BLOCK 1
#else
#define FLOG_BLOCK_IDX_ERASED ((flog_block_idx_t)(-1))
#define FLOG_BLOCK_AGE_ERASED ((flog_block_age_t)(-1))
#define FLOG_FILE_ID_ERASED ((flog_file_id_t)(-1))
#define FLOG_TIMESTAMP_ERASED ((flog_timestamp_t)(-1))
#define FLOG_SECTOR_NBYTES_ERASED ((flog_sector_nbytes_t)(-1))
#define FLOG_SECTOR_TYPE_ID_ERASED ((uint8_t)-1)

#define FS_FIRST_BLOCK 0
#endif

#define FLOG_BLOCK_IDX_INVALID ((flog_block_idx_t)(-1))
#define FLOG_BLOCK_AGE_INVALID ((flog_block_age_t)(-1))
#define FLOG_FILE_ID_INVALID ((flog_file_id_t)(-1))
#define FLOG_TIMESTAMP_INVALID ((flog_timestamp_t)(-1))
#define FLOG_SECTOR_NBYTES_INVALID ((flog_sector_nbytes_t)(-1))
#define FLOG_SECTOR_TYPE_ID_INVALID ((uint8_t)-1)
#define FLOG_SECTOR_IDX_INVALID ((flog_sector_idx_t)-1)
#define FLOG_PAGE_IDX_INVALID ((flog_page_index_t)-1)

//! @}

static char const flog_block_statistics_key[] = "Bears";

typedef struct {
    //! The age of the block
    flog_block_age_t age;
    //! The timestamp of the invalidation
    flog_timestamp_t timestamp;
    //! The next block in the chain
    flog_block_idx_t next_block;
    //! The age of @ref next_block
    flog_block_age_t next_age;
    //! Version field
    uint32_t version;
} flog_block_statistics_sector_header_t;

typedef struct {
    flog_block_statistics_sector_header_t header;
    char key[sizeof(flog_block_statistics_key)];
} flog_block_statistics_sector_with_key_t;

typedef struct {
    flog_timestamp_t timestamp;
} flog_universal_init_sector_t;

typedef struct {
    flog_block_idx_t next_block;
    flog_block_age_t next_age;
    flog_timestamp_t timestamp;
} flog_universal_tail_sector_t;

//! @defgroup FLogInodeBlockStructs Inode block structures
//! @brief Descriptions of the data in inode blocks
//! @{
typedef struct {
    flog_universal_init_sector_t universal;
    flog_block_idx_t previous;
} flog_inode_init_sector_t;

typedef struct {
    uint8_t type_id;
    uint8_t nothing;
    flog_inode_index_t inode_index;
} flog_inode_init_sector_spare_t;

typedef struct {
    flog_file_id_t file_id;
    flog_block_idx_t first_block;
    flog_block_age_t first_block_age;
    flog_timestamp_t timestamp;
} flog_inode_file_allocation_header_t;

typedef struct {
    flog_inode_file_allocation_header_t header;
    char filename[FLOG_MAX_FNAME_LEN];
} flog_inode_file_allocation_t;

typedef struct {
    flog_timestamp_t timestamp;
    flog_block_idx_t last_block;
} flog_inode_file_invalidation_header_t;

typedef struct {
    flog_inode_file_invalidation_header_t header;
} flog_inode_file_invalidation_t;

//! @} // Inode structures

//! @defgroup FLogFileBlockStructs File block structures
//! @brief Descriptions of the data in file blocks
//! @{

typedef struct {
    flog_universal_init_sector_t universal;
    flog_block_age_t age;
    flog_file_id_t file_id;
} flog_file_init_sector_header_t;

typedef struct {
    flog_universal_tail_sector_t universal;
    flog_block_nbytes_t bytes_in_block;
} flog_file_tail_sector_header_t;

typedef struct {
    uint8_t type_id;
    uint8_t nothing;
    flog_sector_nbytes_t nbytes;
} flog_file_sector_spare_t;

//! @}

//! @name Special sector indices
//! @{
typedef enum {
    FLOG_BLOCK_STATISTICS_SECTOR = (0),
    FLOG_INIT_SECTOR = (1),
    FLOG_TAIL_SECTOR = (3),
    FLOG_FILE_FIRST_DATA_SECTOR = (2),
    FLOG_INODE_FIRST_ENTRY_SECTOR = (4)
} flog_sector_special_idx_t;
//! @}


//! @} // FLogPrivate

#endif // __FLOGFS_PRIVATE_H_

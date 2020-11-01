#include <stdint.h>
#pragma once

#define BIGBAG_MAGIC 0xC5149BA9
#define BIGBAG_FREE_ENTRY_MAGIC 0xF4
#define BIGBAG_USED_ENTRY_MAGIC 0xDA

// bag files will always be 64K in size
#define BIGBAG_SIZE (64*1024)

#pragma pack(1)
struct bigbag_hdr_s {  //12 bytes in total
    uint32_t magic;
    // these offsets are from the beginning of the bagfile or 0 if not set
    uint32_t first_free; //first free chunk number
    uint32_t first_element; //first element entry point
};

struct bigbag_entry_s {
    uint32_t next; //the offset from the beginning of the file to the next element in the bag
                        // or 0 if there are no more elements.
    unsigned int entry_magic:8; //will be 0xDA for a string or 0xF4 for free space.
    unsigned int entry_len:24; // does not include sizeof bigbag_entry_s;
                                // the number of bytes after the header that belong to this entry.
    char str[0];
};

#pragma pack()

#define MIN_ENTRY_SIZE (sizeof(struct bigbag_entry_s) + 4)
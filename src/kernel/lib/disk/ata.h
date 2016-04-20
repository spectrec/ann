#ifndef __ATA_H__
#define __ATA_H__

#ifdef __USER__
# error "This file is for kernel internal use only"
#endif

#include <stdint.h>
#include <stdbool.h>

#define ATA_SECTOR_SIZE		512

int8_t disk_io_read_segment(uintptr_t va, uint32_t size, uint32_t lba);

#endif

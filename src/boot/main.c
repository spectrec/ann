#include <ata.h>

#define KERNEL_LOADER_ADDRESS	0x100000 // 1MB

__attribute__((noreturn))
void bootmain(void)
{
	uint8_t *binary = (uint8_t *)KERNEL_LOADER_ADDRESS;

	if (disk_io_read_segment((uint32_t)binary, ATA_SECTOR_SIZE, 0) != 0)
		goto error;

error:
	while (1)
		/*do nothing*/;
}

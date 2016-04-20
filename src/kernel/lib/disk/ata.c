#include "kernel/asm.h"
#include "kernel/lib/disk/ata.h"

// see http://wiki.osdev.org/ATA_PIO_Mode
#define ATA_PIO_PORT_DATA	0x1F0
#define ATA_PIO_PORT_SECT_CNT	0x1F2
#define ATA_PIO_PORT_LBA_LO	0x1F3
#define ATA_PIO_PORT_LBA_MID	0x1F4
#define ATA_PIO_PORT_LBA_HIGH	0x1F5
#define ATA_PIO_PORT_DRIVE	0x1F6
#define ATA_PIO_PORT_STATUS	0x1F7
#define ATA_PIO_PORT_COMMAND	0x1F7

#define ATA_PIO_CMD_READ	0x20
#define ATA_PIO_CMD_WRITE	0x30

#define ATA_PIO_DRIVE_MASTER	0xE0

// Indicates an error occurred
#define ATA_PIO_STATUS_ERR	(1 << 0)
// Drive Fault Error (does not set ERR)
#define ATA_PIO_STATUS_DF	(1 << 5)
// Indicates the drive is preparing to send/receive data (wait for it to clear).
#define ATA_PIO_STATUS_BSY	(1 << 7)
// Bit is clear when drive is spun down, or after an error. Set otherwise.
#define ATA_PIO_STATUS_RDY	(1 << 6)
// Set when the drive has PIO data to transfer, or is ready to accept PIO data.
#define ATA_PIO_STATUS_DRQ	(1 << 3)

static int8_t disk_io_wait_ready(bool is_write)
{
	while (1) {
		uint8_t status = inb(ATA_PIO_PORT_STATUS);

		if ((status & (ATA_PIO_STATUS_ERR | ATA_PIO_STATUS_DF)) != 0)
			return -1;
		if ((status & ATA_PIO_STATUS_BSY) != 0)
			continue;
		if (is_write == true && (status & ATA_PIO_STATUS_DRQ) == 0)
			continue;
		if ((status & ATA_PIO_STATUS_RDY) != 0)
			return 0;
	}
}

// lba - logical block address of the first sector to read
static int8_t disk_io_read(uint16_t *dst, uint32_t lba, uint32_t count)
{
	uint16_t word_count;

	if (disk_io_wait_ready(false) != 0)
		return -1;

	outb(ATA_PIO_PORT_SECT_CNT, count);
	outb(ATA_PIO_PORT_LBA_LO, lba);
	outb(ATA_PIO_PORT_LBA_MID, lba >> 8);
	outb(ATA_PIO_PORT_LBA_HIGH, lba >> 16);
	outb(ATA_PIO_PORT_DRIVE, (lba >> 24) | ATA_PIO_DRIVE_MASTER);
	outb(ATA_PIO_PORT_COMMAND, ATA_PIO_CMD_READ);

	if (disk_io_wait_ready(false) != 0)
		return -1;

	word_count = count * ATA_SECTOR_SIZE / sizeof(uint16_t);
	for (uint32_t i = 0; i < word_count; i++)
		dst[i] = inw(ATA_PIO_PORT_DATA);

	return 0;
}

int8_t disk_io_read_segment(uintptr_t va, uint32_t size, uint32_t lba)
{
	uintptr_t va_end = va + size;

	// round down to sector boundary
	va &= ~(ATA_SECTOR_SIZE - 1);

	for (; va < va_end; va += ATA_SECTOR_SIZE, lba++) {
		if (disk_io_read((void *)va, lba, 1) != 0)
			return -1;
	}

	return 0;
}

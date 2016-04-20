#include "kernel/misc/elf.h"
#include "kernel/lib/disk/ata.h"

#define KERNEL_LOADER_ADDRESS	0x10000

__attribute__((noreturn))
void bootmain(void)
{
	struct elf32_header *elf_header = (struct elf32_header *)KERNEL_LOADER_ADDRESS;

	// Start read from sector `1', because sector `0' contains bootloader
	if (disk_io_read_segment((uint32_t)elf_header, ATA_SECTOR_SIZE, 1) != 0)
		goto error;

	if (elf_header->e_magic != ELF_MAGIC)
		goto error;

	for (struct elf32_program_header *ph = ELF32_PHEADER_FIRST(elf_header);
	     ph < ELF32_PHEADER_LAST(elf_header); ph++) {
		// `+1' because kernel starts in sector `1' (not `0')
		uint32_t lba = (ph->p_offset / ATA_SECTOR_SIZE) + 1;

		if (disk_io_read_segment(ph->p_va, ph->p_memsz, lba) != 0)
			goto error;
	}

	// call entry
	((void (*)(void))(elf_header->e_entry))();

error:
	while (1)
		/*do nothing*/;
}

#include <stdint.h>

struct mm_entry {
	uint64_t base_addr;
	uint64_t addr_len;
	uint32_t type;
	uint32_t acpi_attrs;
};

#define BOOT_MM_ADDR	(void *)0x7e00

void loader_main(void)
{
	struct mm_entry *mm = BOOT_MM_ADDR;
	uint32_t cnt = *((uint32_t *)BOOT_MM_ADDR - 1);

	(void)mm;
	(void)cnt;

	while (1)
		/*do nothing*/;
}

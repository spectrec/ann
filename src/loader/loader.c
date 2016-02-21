#include <util.h>
#include <mm/mmap.h>

#include <stdbool.h>

void *alloc(uint64_t size, uint32_t align)
{
	static char *base = NULL;
	extern char end[];
	char *ret;

	if (base == NULL)
		base = end;

	ret = (void *)ROUND_DOWN((uint32_t)base, align);
	base += ROUND_UP(size, align);

	return ret;
}

uint64_t load_kernel(void)
{
	// Long mode requires page table hierarchy and a bit assembly magic,
	// so let's prepare page table hierarchy.

	return 0;
}

__attribute__((noreturn))
void enter_long_mode(uint64_t kernel_entry_address)
{
	(void)kernel_entry_address;

	while (1)
		/*do nothing*/;
}

struct bios_mmap_entry {
	uint64_t base_addr;
	uint64_t addr_len;
	uint32_t type;
	uint32_t acpi_attrs;
};

#define MEMORY_TYPE_FREE 1
bool page_is_available(uint64_t paddr, struct bios_mmap_entry *mm, uint32_t cnt)
{
	if (paddr == 0)
		// Mark the first page unavailable, because it contain bios
		// data structures. May be somewhen you will need them.
		return false;

	bool page_is_available = true;
	for (uint32_t i = 0; i < cnt; i++) {
		if (mm->base_addr > paddr)
			continue;
		if (paddr+PAGE_SIZE >= mm->base_addr+mm->addr_len)
			continue;

		// Memory areas from bios may be overlapped, so we must check
		// all areas, before we can consider that page is free.
		page_is_available &= mm->type == MEMORY_TYPE_FREE;
	}

	return page_is_available;
}

#define BOOT_MMAP_ADDR	0x7e00
// XXX: I don't know how to run 64bit kernel without entering long mode.
// So my plan is: prepare all for long mode (page tables, load
// kernel), enter long mode and jump to the kernel entry point.
void loader_main(void)
{
	// Next two parameters are prepared by the first loader
	struct bios_mmap_entry *mm = (struct bios_mmap_entry *)BOOT_MMAP_ADDR;
	uint32_t cnt = *((uint32_t *)BOOT_MMAP_ADDR - 1);

	uint64_t high_memory_address = 0;
	for (uint32_t i = 0; i < cnt; i++) {
		if (mm[i].type != MEMORY_TYPE_FREE)
			continue;
		if (mm[i].base_addr + mm[i].addr_len < high_memory_address)
			continue;

		high_memory_address = mm[i].base_addr + mm[i].addr_len;
	}

	// Allocate array of page descriptors. Each element represents one physycal page.
	uint32_t page_cnt = ROUND_DOWN(high_memory_address, PAGE_SIZE) / PAGE_SIZE;
	pages = (struct page *)alloc(sizeof(struct page) * page_cnt, PAGE_SIZE);
	for (uint32_t i = 0; i < page_cnt; i++) {
		uint64_t page_addr = (uint64_t)i * PAGE_SIZE;

		if (page_is_available(page_addr, mm, cnt) == false) {
			pages[i].ref = 1;
			continue;
		}

		LIST_INSERT_HEAD(&free_pages, &pages[i], link);
		pages[i].ref = 0;
	}

	uint64_t kernel_entry_addr = load_kernel();
	enter_long_mode(kernel_entry_addr);
}

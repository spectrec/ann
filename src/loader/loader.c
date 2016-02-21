#include <queue.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct page {
	LIST_ENTRY(page) link;
	uint32_t ref;
};
LIST_HEAD(free_pages, page) free_pages = LIST_HEAD_INITIALIZER(free_pages);
struct page *pages;

#define CONCAT(a_, b_) a_ ## b_
#define CONCAT2(a_, b_) CONCAT(a_, b_)
#define UNIQ_TOKEN(name_) CONCAT2(name_, __LINE__)

#define ROUND_DOWN(addr_, align_) ({				\
	__typeof__(addr_) UNIQ_TOKEN(addr) = addr_;		\
	UNIQ_TOKEN(addr) - (UNIQ_TOKEN(addr) % (align_));	\
})

#define ROUND_UP(addr_, align_) ({					\
	__typeof__(align_) UNIQ_TOKEN(align) = align_;			\
	ROUND_DOWN(addr_ + UNIQ_TOKEN(align) - 1, UNIQ_TOKEN(align));	\
})

#define assert(expr_) // FIXME

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

struct page *page_alloc(void)
{
	struct page *p = LIST_FIRST(&free_pages);

	if (p != NULL)
		LIST_REMOVE(p, link);
	assert(p->ref == 0);

	return p;
}

void page_free(struct page *p)
{
	LIST_INSERT_HEAD(&free_pages, p, link);
}

void page_incref(struct page *p)
{
	p->ref++;
}

void page_decref(struct page *p)
{
	assert(p->ref > 0);
	p->ref--;

	if (p->ref == 0)
		page_free(p);
}

#define PAGE_SIZE 4096
uint64_t page2pa(struct page *p)
{
	uint64_t idx = (pages - p) / sizeof(*p);

	return idx * PAGE_SIZE;
}

// TODO: rewrite it with assembly
void *memcpy(void *dest, const void *src, size_t n)
{
	const char *s = src;
	char *d = dest;

	for (size_t i = 0; i < n; i++)
		d[i] = s[i];

	return dest;
}

// TODO: enter long mode and run kernel
__attribute__((noreturn))
void enter_long_mode(void)
{
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
	uint32_t free_cnt, page_cnt = ROUND_DOWN(high_memory_address, PAGE_SIZE) / PAGE_SIZE;
	pages = (struct page *)alloc(sizeof(struct page) * page_cnt, PAGE_SIZE);
	for (uint32_t i = 0; i < page_cnt; i++) {
		uint64_t page_addr = (uint64_t)i * PAGE_SIZE;

		if (page_is_available(page_addr, mm, cnt) == false) {
			pages[i].ref = 1;
			continue;
		}

		LIST_INSERT_HEAD(&free_pages, &pages[i], link);
		pages[i].ref = 0;
		free_cnt++;
	}

	// Long mode requires page table hierarchy and a bit assembly magic,
	// so let's prepare page table hierarchy.

	// TODO:
	// - create page tables hierarchy
	// - read and map kernel

	enter_long_mode();
}

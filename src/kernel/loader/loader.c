#include "stdlib/string.h"
#include "stdlib/assert.h"

#include "kernel/lib/disk/ata.h"
#include "kernel/lib/memory/map.h"
#include "kernel/lib/memory/layout.h"
#include "kernel/lib/console/terminal.h"

#include "kernel/asm.h"
#include "kernel/cpu.h"
#include "kernel/misc/gdt.h"
#include "kernel/misc/elf.h"
#include "kernel/misc/util.h"
#include "kernel/loader/config.h"

struct bios_mmap_entry {
	uint64_t base_addr;
	uint64_t addr_len;
	uint32_t type;
	uint32_t acpi_attrs;
};

// Describe gdtr for long mode
struct gdtr {
	uint16_t limit;
	uint32_t base;
	uint32_t zero;
} __attribute__((packed));

// Some help from linker script
extern uint8_t boot_stack[], end[];

// XXX: Now (in loader) virtual address is equal to physical one
static uint64_t max_physical_address;
static uint8_t *free_memory = end;

static struct page *pages;
static uint64_t pages_cnt;

pml4e_t *pml4;

struct descriptor *gdt;
struct gdtr gdtr;

void loader_panic(const char *fmt, ...);
panic_t panic = loader_panic;

// Loader uses this struct to pass some
// useful information to kernel
struct kernel_config *config;


void *loader_alloc(uint64_t size, uint32_t align);
void loader_detect_memory(struct bios_mmap_entry *mm, uint32_t cnt);
int loader_init_memory(struct bios_mmap_entry *mm, uint32_t cnt);
int loader_map_section(struct mmap_state *state, uint64_t va, uintptr_t pa, uint64_t len, bool hard);

bool page_is_available(uint64_t paddr, struct bios_mmap_entry *mm, uint32_t cnt);

struct descriptor *loader_init_gdt(void);

int loader_read_kernel(uint64_t *kernel_entry_point);
void loader_enter_long_mode(uint64_t kernel_entry_point);

// Why this address? See `boot/boot.S'
#define BOOT_MMAP_ADDR		0x7e00
void loader_main(void)
{
	// Next two parameters are prepared by the first loader
	struct bios_mmap_entry *mm = (struct bios_mmap_entry *)BOOT_MMAP_ADDR;
	uint32_t cnt = *((uint32_t *)BOOT_MMAP_ADDR - 1);

	terminal_init();

	uint64_t kernel_entry_point;
	if (loader_read_kernel(&kernel_entry_point) != 0)
		goto something_bad;

	loader_detect_memory(mm, cnt);
	if (loader_init_memory(mm, cnt) != 0)
		goto something_bad;

	loader_enter_long_mode(kernel_entry_point);

something_bad:
	terminal_printf("Stop loading, hang\n");
}

void *loader_alloc(uint64_t size, uint32_t align)
{
	uint8_t *ret;

	ret = (void *)ROUND_UP((uint32_t)free_memory, align);
	free_memory = ret + size;

	return ret;
}

#define KERNEL_BASE_DISK_SECTOR 2048 // 1Mb
int loader_read_kernel(uint64_t *kernel_entry_point)
{
	struct elf64_header *elf_header = loader_alloc(sizeof(*elf_header), PAGE_SIZE);

	if (disk_io_read_segment((uint32_t)elf_header, ATA_SECTOR_SIZE, KERNEL_BASE_DISK_SECTOR) != 0) {
		terminal_printf("Can't read elf header\n");
		return -1;
	}
	if (elf_header->e_magic != ELF_MAGIC) {
		terminal_printf("Invalid elf format, magic mismatch (%u)", elf_header->e_magic);
		return -1;
	}

	for (struct elf64_program_header *ph = ELF64_PHEADER_FIRST(elf_header);
	     ph < ELF64_PHEADER_LAST(elf_header); ph++) {
		// Manually truncate high address part. This is needed to make
		// mapping [KERNBASE; KERNBASE+FREEMEM) -> [0; FREEMEM) valid
		ph->p_va &= 0xFFFFFFFFull;

		uint32_t lba = (ph->p_offset / ATA_SECTOR_SIZE) + KERNEL_BASE_DISK_SECTOR;
		if (disk_io_read_segment(ph->p_va, ph->p_memsz, lba) != 0) {
			terminal_printf("Can't read segment `%u'", lba);
			return -1;
		}

		if (PADDR(free_memory) < PADDR(ph->p_va + ph->p_memsz))
			// Shift `free_memory'. So `loader_alloc()' will
			// return free memory areas after this function
			free_memory = (uint8_t *)(uintptr_t)(ph->p_va + ph->p_memsz);
	}
	*kernel_entry_point = elf_header->e_entry;

	return 0;
}

#define MEMORY_TYPE_FREE 1
void loader_detect_memory(struct bios_mmap_entry *mm, uint32_t cnt)
{
	max_physical_address = 0;
	for (uint32_t i = 0; i < cnt; i++) {
		if (mm[i].type != MEMORY_TYPE_FREE)
			continue;
		if (mm[i].base_addr + mm[i].addr_len < max_physical_address)
			continue;

		max_physical_address = mm[i].base_addr + mm[i].addr_len;
	}

	pages_cnt = ROUND_DOWN(max_physical_address, PAGE_SIZE) / PAGE_SIZE;
	terminal_printf("Available memory: %u Kb (%u pages)\n",
			(uint32_t)(max_physical_address / 1024), (uint32_t)pages_cnt);
}

int loader_init_memory(struct bios_mmap_entry *mm, uint32_t cnt)
{
	static struct mmap_state state;

	config = loader_alloc(sizeof(*config), PAGE_SIZE);
	memset(config, 0, sizeof(*config));

	gdt = loader_init_gdt();

	// Allocate and init PML4
	pml4 = loader_alloc(PAGE_SIZE, PAGE_SIZE);
	memset(pml4, 0, PAGE_SIZE);

	// Allocate and initialize physical pages array
	pages = loader_alloc(SIZEOF_PAGE64 * pages_cnt, PAGE_SIZE);
	memset(pages, 0, SIZEOF_PAGE64 * pages_cnt);

	// Initialize config
	config->pages_cnt = pages_cnt;
	config->pages.ptr = pages;
	config->pml4.ptr = pml4;
	config->gdt.ptr = gdt;

	// Initialize `mmap_state'
	state.free = (struct mmap_free_pages){ NULL };
	state.pages_cnt = pages_cnt;
	state.pages = pages;
	state.pml4 = pml4;
	mmap_init(&state);

	// Fill in free pages list, skip ones used by kernel or hardware
	for (uint32_t i = 0; i < pages_cnt; i++) {
		uint64_t page_addr = (uint64_t)i * PAGE_SIZE;

		if (page_is_available(page_addr, mm, cnt) == false) {
			pages[i].ref = 1;
			continue;
		}

		// Insert head is important, it guarantees that high physical
		// addresses will be used before low ones.
		LIST_INSERT_HEAD(&state.free, &pages[i], link);
	}

	// Map kernel stack
	if (loader_map_section(&state, KERNEL_STACK_TOP - KERNEL_STACK_SIZE,
			(uintptr_t)boot_stack, KERNEL_STACK_SIZE, true) != 0)
		return -1;

	// Pass some information to kernel
	if (loader_map_section(&state, KERNEL_INFO, (uintptr_t)config, PAGE_SIZE, true) != 0)
		return -1;

	// Make APIC registers available for the kernel
	if (loader_map_section(&state, APIC_BASE, APIC_BASE_PA, PAGE_SIZE, true) != 0)
		return -1;

	// Make IO APIC registers available for the kernel
	if (loader_map_section(&state, IOAPIC_BASE, IOAPIC_BASE_PA, PAGE_SIZE, true) != 0)
		return -1;

	// Map loader to make all addresses valid after paging enable
	// (before jump to kernel entry point). We must map all until
	// `free_memory' not just `end', because `pml4' located after `end'
	if (loader_map_section(&state, 0x0, 0x0, (uintptr_t)free_memory, true) != 0)
		return -1;

	// Make continuous mapping [KERNEL_BASE, KERNEL_BASE + FREE_MEM) -> [0, FREE_MEM)
	// Without this mapping we can't compute virtual address from physical one
	if (loader_map_section(&state, KERNEL_BASE, 0x0, ROUND_DOWN(max_physical_address, PAGE_SIZE), false) != 0)
		return -1;

	return 0;
}

#define NGDT_ENTRIES	5
struct descriptor *loader_init_gdt(void)
{
	uint16_t system_segmnets_size = sizeof(struct descriptor64) * CPU_MAX_CNT;
	uint16_t user_segments_size = sizeof(struct descriptor) * NGDT_ENTRIES;
	uint16_t gdt_size = user_segments_size + system_segmnets_size;
	struct descriptor *gdt = loader_alloc(gdt_size, 16);

	gdtr.base = (uintptr_t)gdt;
	gdtr.limit = gdt_size - 1;
	gdtr.zero = 0;

	// Null descriptor - just in case
	gdt[0] = SEGMENT_DESC(0, 0x0, 0x0);

	// Kernel text
	gdt[GD_KT >> 3] = SEGMENT_DESC(USF_L | USF_P | DPL_S | USF_S | UST_X | UST_R, 0x0, 0xffffffff);

	// Kernel data (all fields, except `USF_P' are ignored)
	gdt[GD_KD >> 3] = SEGMENT_DESC(USF_P | USF_D | USF_S | DPL_S | UST_W, 0x0, 0xffffffff);

	// User text
	gdt[GD_UT >> 3] = SEGMENT_DESC(USF_L | USF_P | DPL_U | USF_S | UST_X | UST_R, 0x0, 0xffffffff);

	// User data (all fields, except `USF_P' are ignored)
	gdt[GD_UD >> 3] = SEGMENT_DESC(USF_P | USF_D | USF_S | DPL_U | UST_W, 0x0, 0xffffffff);

	return gdt;
}

bool page_is_available(uint64_t paddr, struct bios_mmap_entry *mm, uint32_t cnt)
{
	if (paddr == 0)
		// The first page contain some useful bios data strutures.
		// Reserve it just in case.
		return false;

	if (paddr >= APIC_BASE_PA && paddr < APIC_BASE_PA + PAGE_SIZE)
		// APIC registers mapped here
		return false;

	if (paddr >= IOAPIC_BASE_PA && paddr < IOAPIC_BASE_PA + PAGE_SIZE)
		// IO APIC registers mapped here
		return false;

	if (paddr >= (uint64_t)(uintptr_t)end &&
	    paddr  < (uint64_t)(uintptr_t)free_memory)
		// This address range contains kernel
		// and data allocated with `loader_alloc()'
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

int loader_map_section(struct mmap_state *state, uint64_t va, uintptr_t pa, uint64_t len, bool hard)
{
	uint64_t va_aligned = ROUND_DOWN(va, PAGE_SIZE);
	uint64_t len_aligned = ROUND_UP(len, PAGE_SIZE);

	for (uint64_t i = 0; i < len_aligned; i += PAGE_SIZE) {
		pte_t *pte = mmap_lookup(state->pml4, va_aligned + i, true);
		struct page *page;

		if (pte == NULL)
			return -1;
		assert((*pte & PTE_P) == 0);

		*pte = PTE_ADDR(pa + i) | PTE_P | PTE_W;

		page = pa2page(PTE_ADDR(pa + i));
		if (page->ref != 0)
			// Page already has been removed from free list
			continue;

		page_incref(page);
		if (hard == true) {
			// We must remove some pages from free list, to avoid
			// overriding them later
			LIST_REMOVE(page, link);
			page->link.le_next = NULL;
			page->link.le_prev = NULL;
		}
	}

	return 0;
}

void loader_enter_long_mode(uint64_t kernel_entry_point)
{
	// Reload gdt
	asm volatile("lgdt gdtr");

	// Reload segment registers
	asm volatile("movw %%ax, %%es" :: "a" (GD_KD));
	asm volatile("movw %%ax, %%ds" :: "a" (GD_KD));
	asm volatile("movw %%ax, %%ss" :: "a" (GD_KD));

	// Enable PAE
	asm volatile(
		"movl %cr4, %eax\n\t"
		"btsl $5, %eax\n\t"
		"movl %eax, %cr4\n"
	);

	// Setup CR3
	asm volatile ("movl %%eax, %%cr3" :: "a" (PADDR(pml4)));

	// Enable long mode (set EFER.LME=1)
	asm volatile (
		"movl $0xc0000080, %ecx\n\t"	// EFER MSR number
		"rdmsr\n\t"			// Read EFER
		"btsl $8, %eax\n\t"		// Set LME=1
		"wrmsr\n"			// Write EFER
	);

	// Enable paging to activate long mode
	asm volatile (
		"movl %cr0, %eax\n\t"
		"btsl $31, %eax\n\t"
		"movl %eax, %cr0\n"
	);

	extern void entry_long_mode_asm(uint64_t kernel_entry);
	entry_long_mode_asm(kernel_entry_point); // does not return
}

void loader_panic(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	terminal_vprintf(fmt, ap);
	va_end(ap);

	while (1) {
		/*do nothing*/;
	}
}

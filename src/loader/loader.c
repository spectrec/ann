#include <gdt.h>
#include <elf.h>
#include <util.h>
#include <string.h>
#include <fs/ata.h>
#include <mm/mmap.h>

extern uint8_t end[];
static uint8_t *free_memory = end;

void *alloc(uint64_t size, uint32_t align)
{
	uint8_t *ret;

	ret = (void *)ROUND_UP((uint32_t)free_memory, align);
	free_memory = ret + size;

	return ret;
}

int map_section(struct mmap_state *state, uint64_t va, uintptr_t pa, uint64_t len)
{
	uint64_t va_aligned = ROUND_DOWN(va, PAGE_SIZE);
	uint64_t len_aligned = ROUND_UP(len, PAGE_SIZE);

	for (uint64_t i = 0; i < len_aligned; i += PAGE_SIZE) {
		pte_t *pte = mmap_lookup(state->pml4, va_aligned + i, true);
		if (pte == NULL)
			return -1;
		if ((*pte & PTE_P) != 0)
			return -1;

		*pte = PTE_ADDR(pa + i) | PTE_P | PTE_W;
	}

	return 0;
}

#define LOAD_KERNEL_ERROR 0
#define KERNEL_BASE_DISK_SECTOR 2048 // 1Mb
uint64_t load_kernel(struct mmap_state *state)
{
	struct elf64_header *elf_header = alloc(sizeof(*elf_header), PAGE_SIZE);
	struct elf64_program_header *ph, *eph;

	if (disk_io_read_segment((uint32_t)elf_header, ATA_SECTOR_SIZE, KERNEL_BASE_DISK_SECTOR) != 0)
		return LOAD_KERNEL_ERROR;

	if (elf_header->e_magic != ELF_MAGIC)
		return LOAD_KERNEL_ERROR;

	ph = (struct elf64_program_header *)((uint8_t *)elf_header + elf_header->e_phoff);
	eph = ph + elf_header->e_phnum;

	// Reserve space for kernel (to avoid overwriting pages by
	// `map_section'). Align of `PAGE_SIZE' is important here,
	// because it will need for mapping
	uint64_t required_len = 0;
	for (struct elf64_program_header *p = ph; p < eph; p++)
		required_len += ROUND_UP(p->p_memsz, PAGE_SIZE);
	uint8_t *kernel = alloc(required_len, PAGE_SIZE);
	for (uint64_t i = 0; i < required_len; i += PAGE_SIZE) {
		struct page *p = pa2page((uint64_t)(uintptr_t)kernel + i);

		LIST_REMOVE(p, link);
		p->ref = 1;
	}

	for (; ph < eph; ph++) {
		if (ph->p_type != ELF_PHEADER_TYPE_LOAD)
			continue;

		uint32_t lba = (ph->p_offset / ATA_SECTOR_SIZE) + KERNEL_BASE_DISK_SECTOR;
		if (disk_io_read_segment((uintptr_t)kernel, ph->p_memsz, lba) != 0)
			return LOAD_KERNEL_ERROR;

		if (map_section(state, ph->p_va, (uintptr_t)kernel, ph->p_memsz) != 0)
			return LOAD_KERNEL_ERROR;

		// Go to next section
		kernel += ROUND_UP(ph->p_memsz, PAGE_SIZE);
	}

	return elf_header->e_entry;
}

#define GD_KT	(sizeof(struct descriptor) << 0)
#define GD_KD	(sizeof(struct descriptor) << 1)
#define GD_UD	(sizeof(struct descriptor) << 2)
#define GD_UT	(sizeof(struct descriptor) << 3)
#define GD_TSS	(sizeof(struct descriptor) << 4)

// In long mode base and limit of the segment descriptors are ignored
struct descriptor gdt[] __attribute__((aligned(8))) = {
	// Null descriptor - just in case
	[0]		= SEGMENT_DESC(0, 0x0, 0x0),

	// Kernel text
	[GD_KT >> 3]	= SEGMENT_DESC(USF_L | USF_P | DPL_S | USF_S | UST_X | UST_R, 0x0, 0xffffffff),

	// Kernel data (all fields, except `USF_P' are ignored)
	[GD_KD >> 3]	= SEGMENT_DESC(USF_P | USF_D | USF_S | DPL_S | UST_W, 0x0, 0xffffffff),

	// User text
	[GD_UT >> 3]	= SEGMENT_DESC(USF_L | USF_P | DPL_U | USF_S | UST_X | UST_R, 0x0, 0xffffffff),

	// User data (all fields, except `USF_P' are ignored)
	[GD_UD >> 3]	= SEGMENT_DESC(USF_P | USF_S | DPL_U | UST_W, 0x0, 0xffffffff),

	// Tss decriptor, will be initialized later
	[GD_TSS >> 3]	= SEGMENT_DESC(0, 0x0, 0x0),
};

// Describe gdtr for long mode
struct gdtr {
	uint16_t limit;
	uint32_t base;
	uint32_t zero;
} __attribute__((packed)) gdtr;

extern void entry_long_mode_asm(uint64_t kernel_entry);
void enter_long_mode(struct mmap_state *state, uint64_t kernel_entry_vaddr)
{
	uint32_t pml4_paddr = PADDR(state->pml4);

	gdtr.limit = sizeof(gdt);
	gdtr.base = (uintptr_t)gdt;
	gdtr.zero = 0;

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
	asm volatile ("movl %%eax, %%cr3" :: "a" (pml4_paddr));

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

	entry_long_mode_asm(kernel_entry_vaddr);

	while (1)
		/*should never happend*/;
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

	if (paddr >= (uint64_t)(uintptr_t)end && paddr < (uint64_t)(uintptr_t)free_memory)
		// This is `mmap_state'
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
	uint32_t page_cnt = ROUND_DOWN(high_memory_address, PAGE_SIZE) / PAGE_SIZE;

	// Initialize mmap state, it must be passed to kernel with the help of some magic
	struct mmap_state *state = (struct mmap_state *)alloc(sizeof(*state), 8);
	state->pages = (struct page *)alloc(sizeof(struct page) * page_cnt, PAGE_SIZE);
	state->pml4 = (pml4e_t *)alloc(PAGE_SIZE, PAGE_SIZE);
	state->free = (struct mmap_free_pages){ NULL };
	mmap_init(state);

	// PML4 is empty now
	memset(state->pml4, 0, sizeof(*state->pml4));

	// Fill in free pages list, skip ones used by kernel or hardware
	for (uint32_t i = 0; i < page_cnt; i++) {
		uint64_t page_addr = (uint64_t)i * PAGE_SIZE;

		if (page_is_available(page_addr, mm, cnt) == false) {
			state->pages[i].ref = 1;
			continue;
		}

		LIST_INSERT_HEAD(&state->free, &state->pages[i], link);
		state->pages[i].ref = 0;
	}

	// Map loader, to make all addresses valid after paging enable
	//if (map_section(state, 0x0, 0x0, (uintptr_t)end) != 0)
	if (map_section(state, 0x0, 0x0, (uintptr_t)free_memory) != 0)
		return;

	// TODO: make `pml4', `state' and `pages' available for kernel

	uint64_t kernel_entry_addr = load_kernel(state);
	if (kernel_entry_addr != 0)
		enter_long_mode(state, kernel_entry_addr);
}

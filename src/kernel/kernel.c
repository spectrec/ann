#include <x86.h>
#include <string.h>
#include <config.h>
#include <mm/mmap.h>
#include <mm/layout.h>
#include <console/terminal.h>

#include "kernel.h"
#include "interrupt.h"

void kernel_panic(const char *fmt, ...);
panic_t panic = kernel_panic;

void kernel_panic(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	terminal_vprintf(fmt, ap);
	va_end(ap);

	while (1) {
		/*do nothing*/;
	}
}

struct page32 {
	uint32_t ref;
	uint64_t links;
} __attribute__((packed));

// Loader prepared some interesting info for us. Let's process it.
void kernel_init_mmap(void)
{
	struct kernel_config *config = (struct kernel_config *)KERNEL_INFO;
	struct gdtr {
		uint16_t limit;
		uint64_t base;
	} __attribute__((packed)) gdtr;
	static struct mmap_state state;
	struct page32 *pages32;

	// Convert physical addresses into virtual ones
	config->gdt.ptr = VADDR(config->gdt.ptr);
	config->pml4.ptr = VADDR(config->pml4.ptr);
	config->pages.ptr = VADDR(config->pages.ptr);

	// Reinitialize state
	state.free = (struct mmap_free_pages){ NULL };
	state.pml4 = config->pml4.ptr;
	state.pages_cnt = config->pages_cnt;
	state.pages = config->pages.ptr;
	mmap_init(&state);

	sgdt(gdtr);

	// We must reload gdt to avoid page fault when accessing it after
	// removing unneeded mappings
	gdtr.base = config->gdt.uintptr;
	asm volatile("lgdt (%0)" : : "p"(&gdtr));

	pages32 = (struct page32 *)config->pages.ptr;
	state.pml4[0] = 0; // remove unneeded mappings

	// First of all - convert `page32' into 64-bit `page'
	for (int64_t i = state.pages_cnt-1; i >= 0; i--) {
		struct page *p = &state.pages[i];

		memset(p, 0, sizeof(*p));
		p->ref = pages32[i].ref;
	}

	// Rebuild free list
	for (uint32_t i = 0; i < state.pages_cnt; i++) {
		if (state.pages[i].ref != 0) {
			assert(state.pages[i].ref == 1);
			continue;
		}

		LIST_INSERT_HEAD(&state.free, &state.pages[i], link);
	}
}

void kernel_main(void)
{
	// Initialize bss
	extern uint8_t edata[], end[];
	memset(edata, 0, end - edata);

	// Reset terminal (without this output will not work)
	terminal_init();

	// Initialize memory (process info prepared by loader)
	kernel_init_mmap();

	// Enable interrupts and exceptions
	interrupt_init();

	asm volatile("int3");

	panic("Nothing to do");
}

#ifndef __MMAP_H__
#define __MMAP_H__

#include <queue.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PAGE_SIZE	4096
#define PAGE_SHIFT	12

#define PML4_SHIFT	39
#define PDP_SHIFT	30
#define PD_SHIFT	21
#define PT_SHIFT	12

#define PML4E_ADDR(paddr_)	((uint64_t)((paddr_) & ~0xfff))
#define PDPE_ADDR(paddr_)	((uint64_t)((paddr_) & ~0xfff))
#define PDE_ADDR(paddr_)	((uint64_t)((paddr_) & ~0xfff))
#define PTE_ADDR(paddr_)	((uint64_t)((paddr_) & ~0xfff))

#define IDX_MASK ((1 << 9) - 1) // 111111111b
#define PML4_IDX(addr_)	(((uint64_t)addr_ >> PML4_SHIFT) & IDX_MASK)
#define PDP_IDX(addr_)	(((uint64_t)addr_ >> PDP_SHIFT)  & IDX_MASK)
#define PD_IDX(addr_)	(((uint64_t)addr_ >> PD_SHIFT)   & IDX_MASK)
#define PT_IDX(addr_)	(((uint64_t)addr_ >> PT_SHIFT)   & IDX_MASK)

#define PML4E_P		(1 << 0)	// present
#define PML4E_W		(1 << 1)	// write
#define PML4E_U		(1 << 2)	// user
#define PML4E_PWT	(1 << 3)	// writethrough
#define PML4E_PCD	(1 << 4)	// cache disable
#define PML4E_A		(1 << 5)	// accessed

#define PDPE_P		(1 << 0)
#define PDPE_W		(1 << 1)
#define PDPE_U		(1 << 2)
#define PDPE_PWT	(1 << 3)
#define PDPE_PCD	(1 << 4)
#define PDPE_A		(1 << 5)

#define PDE_P		(1 << 0)
#define PDE_W		(1 << 1)
#define PDE_U		(1 << 2)
#define PDE_PWT		(1 << 3)
#define PDE_PCD		(1 << 4)
#define PDE_A		(1 << 5)

#define PTE_P		(1 << 0)
#define PTE_W		(1 << 1)
#define PTE_U		(1 << 2)
#define PTE_PWT		(1 << 3)
#define PTE_PCD		(1 << 4)
#define PTE_A		(1 << 5)
#define PTE_D		(1 << 6)	// dirty
#define PTE_PAT		(1 << 7)	// page-attribute table
#define PTE_G		(1 << 8)	// global

// Loader virtual address equal to physical one, so this value is `0'.
// Kernel must provide it's own `VADDR_BASE'
#ifndef VADDR_BASE
# define VADDR_BASE	0
#endif

#define VADDR(paddr_) ((void *)(uintptr_t)((paddr_) + VADDR_BASE))
#define PADDR(vaddr_) ((uintptr_t)(vaddr_) - VADDR_BASE)

typedef uint64_t pml4e_t; // page map level 4 entry
typedef uint64_t pdpe_t;  // page direcroty pointer entry
typedef uint64_t pde_t;   // page directory entry
typedef uint64_t pte_t;   // page table entry

struct page {
	LIST_ENTRY(page) link;
	uint32_t ref;
};
LIST_HEAD(mmap_free_pages, page);

struct mmap_state {
	// List of the free pages
	struct mmap_free_pages free;

	// Virtual address of physical pages array
	struct page *pages;

	// Virtual address of PML4
	pml4e_t *pml4;
};

void mmap_init(struct mmap_state *state);

pte_t *mmap_lookup(pml4e_t *pml4, uint64_t va, bool create);

struct page *page_alloc(void);
void page_free(struct page *p);

void page_incref(struct page *p);
void page_decref(struct page *p);

uint64_t page2pa(struct page *p);
struct page *pa2page(uint64_t addr);

#endif

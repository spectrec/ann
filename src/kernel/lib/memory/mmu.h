#ifndef __MMU_H__
#define __MMU_H__

#ifdef __USER__
# error "This file is for kernel internal use only"
#endif

#ifndef __ASSEMBLER__
# include <stdint.h>
#endif

#define PAGE_SIZE	4096
#define PAGE_SHIFT	12

#define NPML4_ENTRIES	512ull
#define NPDP_ENTRIES	512ull
#define NPD_ENTRIES	512ull
#define NPT_ENTRIES	512ull

#define PML4_SHIFT	39
#define PDP_SHIFT	30
#define PD_SHIFT	21
#define PT_SHIFT	12

#ifndef __ASSEMBLER__
# define PML4E_ADDR(paddr_)	((uint64_t)((paddr_) & ~0xfff))
# define PDPE_ADDR(paddr_)	((uint64_t)((paddr_) & ~0xfff))
# define PDE_ADDR(paddr_)	((uint64_t)((paddr_) & ~0xfff))
# define PTE_ADDR(paddr_)	((uint64_t)((paddr_) & ~0xfff))

# define IDX_MASK		((1 << 9) - 1) // 111111111b
# define PML4_IDX(addr_)	(((uint64_t)addr_ >> PML4_SHIFT) & IDX_MASK)
# define PDP_IDX(addr_)		(((uint64_t)addr_ >> PDP_SHIFT)  & IDX_MASK)
# define PD_IDX(addr_)		(((uint64_t)addr_ >> PD_SHIFT)   & IDX_MASK)
# define PT_IDX(addr_)		(((uint64_t)addr_ >> PT_SHIFT)   & IDX_MASK)

// Build linear address from indexes and offset. Don't forget about canonical address form
# define PAGE_ADDR(pml4, pdp, pd, pt, off) ({		\
	uint64_t pml4_ = pml4;				\
	uint64_t pdp_ = pdp;				\
	uint64_t pd_ = pd;				\
	uint64_t pt_ = pt;				\
	uint64_t sign_ = (pml4_ & (1ull << 8)) ?	\
			(-1ull << 48) : 0;		\
							\
	(void *)( (sign_) |				\
		  ((pml4_) << PML4_SHIFT) |		\
		  ((pdp_) << PDP_SHIFT) |		\
		  ((pd_) << PD_SHIFT) |			\
		  ((pt_) << PT_SHIFT) | (off) );	\
})

typedef uint64_t pml4e_t; // page map level 4 entry
typedef uint64_t pdpe_t;  // page direcroty pointer entry
typedef uint64_t pde_t;   // page directory entry
typedef uint64_t pte_t;   // page table entry
#endif

#define PML4E_P		(1 << 0)	// present
#define PML4E_W		(1 << 1)	// write
#define PML4E_U		(1 << 2)	// user
#define PML4E_PWT	(1 << 3)	// writethrough
#define PML4E_PCD	(1 << 4)	// cache disable
#define PML4E_A		(1 << 5)	// accessed

#define PDPE_P		(1 << 0)	// present
#define PDPE_W		(1 << 1)	// write allowed
#define PDPE_U		(1 << 2)	// user access allowed
#define PDPE_PWT	(1 << 3)	// writethrough
#define PDPE_PCD	(1 << 4)	// cache disable
#define PDPE_A		(1 << 5)	// accessed

#define PDE_P		(1 << 0)	// present
#define PDE_W		(1 << 1)	// write
#define PDE_U		(1 << 2)	// user
#define PDE_PWT		(1 << 3)	// writethrough
#define PDE_PCD		(1 << 4)	// cache disable
#define PDE_A		(1 << 5)	// accessed

#define PTE_P		(1 << 0)	// present
#define PTE_W		(1 << 1)	// write
#define PTE_U		(1 << 2)	// user
#define PTE_PWT		(1 << 3)	// writethrough
#define PTE_PCD		(1 << 4)	// cache disable
#define PTE_A		(1 << 5)	// accessed
#define PTE_D		(1 << 6)	// dirty
#define PTE_PAT		(1 << 7)	// page-attribute table
#define PTE_G		(1 << 8)	// global

#define PTE_FLAGS_MASK	(0xFFF)		// low 12 bits

// Mark page copy-on-write (kernel internal logic)
#define PTE_COW		(1 << 11)

#endif

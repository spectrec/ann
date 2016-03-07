#ifndef __MMU_H__
#define __MMU_H__

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

#endif

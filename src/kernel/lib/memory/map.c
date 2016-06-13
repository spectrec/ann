#include "kernel/asm.h"

#include "stdlib/assert.h"
#include "stdlib/string.h"

#include "kernel/lib/memory/map.h"
#include "kernel/lib/memory/layout.h"
#include "kernel/lib/console/terminal.h"

// This struct is initialized by second stage loader for kernel
static struct mmap_state *mmap_state;

void mmap_init(struct mmap_state *state)
{
	mmap_state = state;
}

struct page *page_alloc(void)
{
	struct page *p = LIST_FIRST(&mmap_state->free);

	if (p != NULL) {
		LIST_REMOVE(p, link);

		// XXX: set to `NULL' is important. Because kernel think
		// that page is free only if it has NULL links
		memset(p, 0, sizeof(*p));
	}

	return p;
}

void page_free(struct page *p)
{
	LIST_INSERT_HEAD(&mmap_state->free, p, link);
	assert(p->ref == 0);
}

void page_incref(struct page *p)
{
	p->ref++;
}

void page_decref(struct page *p)
{
	assert(p->ref > 0);
	p->ref--;

	terminal_printf("decref page %p, refs: %d\n", p, p->ref);

	if (p->ref == 0)
		page_free(p);
}

uint64_t page2pa(struct page *p)
{
	return (p - mmap_state->pages) << PAGE_SHIFT;
}

struct page *pa2page(uint64_t addr)
{
	return &mmap_state->pages[addr >> PAGE_SHIFT];
}

void *page2kva(struct page *p)
{
	return VADDR(page2pa(p));
}

pte_t *mmap_lookup(pml4e_t *pml4, uint64_t va, bool create)
{
	struct page *page4pdp = NULL, *page4pd = NULL, *page4pt = NULL;
	pdpe_t pml4e = pml4[PML4_IDX(va)];

	if ((pml4e & PML4E_P) != 0)
		goto pml4e_found;
	if (create == false)
		return NULL;

	// Prepare new page directory pointer
	if ((page4pdp = page_alloc()) == NULL)
		return NULL;
	memset(page2kva(page4pdp), 0, PAGE_SIZE);
	page4pdp->ref = 1;

	// Insert new pdp into PML4
	pml4e = pml4[PML4_IDX(va)] = page2pa(page4pdp) | PML4E_P | PML4E_W | PML4E_U;

pml4e_found:
	assert((pml4e & PML4E_P) != 0);

	pdpe_t *pdp = VADDR(PML4E_ADDR(pml4e));
	pdpe_t pdpe = pdp[PDP_IDX(va)];

	if ((pdpe & PDPE_P) != 0)
		goto pdpe_found;
	if (create == false)
		return NULL;

	// Prepare new page directory
	if ((page4pd = page_alloc()) == NULL)
		return NULL;
	memset(page2kva(page4pd), 0, PAGE_SIZE);
	page4pd->ref = 1;

	// Insert new page directory into page directory pointer table
	pdpe = pdp[PDP_IDX(va)] = page2pa(page4pd) | PDPE_P | PDPE_W | PDPE_U;

pdpe_found:
	assert((pdpe & PDPE_P) != 0);

	pde_t *pd = VADDR(PDPE_ADDR(pdpe));
	pde_t pde = pd[PD_IDX(va)];

	if ((pde & PDE_P) != 0)
		goto pde_found;
	if (create == false)
		return NULL;

	// Prepare new page table
	if ((page4pt = page_alloc()) == NULL)
		return NULL;
	memset(page2kva(page4pt), 0, PAGE_SIZE);
	page4pt->ref = 1;

	// Insert new page table into page directory
	pde = pd[PD_IDX(va)] = page2pa(page4pt) | PDE_P | PTE_W | PDE_U;

pde_found:
	assert((pde & PDE_P) != 0);

	pte_t *pt = VADDR(PDE_ADDR(pde));

	return &pt[PT_IDX(va)];
}

int page_insert(pml4e_t *pml4, struct page *p, uintptr_t va, unsigned perm)
{
	pte_t *pte = mmap_lookup(pml4, va, 1);
	if (pte == NULL)
		// no memory
		return -1;

	// remap same page (possible change permissions)
	if (PTE_ADDR(*pte) == page2pa(p)) {
		invlpg((void *)va);

		*pte = page2pa(p) | perm | PTE_P;
		return 0;
	}

	// delete old mapping if exists
	page_remove(pml4, va);

	*pte = page2pa(p) | perm | PTE_P;
	page_incref(p);

	return 0;
}

void page_remove(pml4e_t *pml4, uintptr_t va)
{
	struct page *p;
	pte_t *pte;

	p = page_lookup(pml4, va, &pte);
	if (p == NULL)
		// nothing to do
		return;

	page_decref(p);
	*pte = 0;

	invlpg((void *)va);
}

struct page *page_lookup(pml4e_t *pml4, uintptr_t va, pte_t **pte_p)
{
	pte_t *pte;

	pte = mmap_lookup(pml4, va, 0);
	if (pte_p != NULL)
		*pte_p = pte;

	if (pte == NULL || (*pte & PTE_P) == 0)
		// nothing to do
		return NULL;

	return pa2page(PTE_ADDR(*pte));
}

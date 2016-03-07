#include <assert.h>
#include <string.h>

#include "mmap.h"
#include "layout.h"

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
		assert(p->ref == 0);
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
	pml4e = pml4[PML4_IDX(va)] = page2pa(page4pdp) | PML4E_P | PML4E_W;

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
	pdpe = pdp[PDP_IDX(va)] = page2pa(page4pd) | PDPE_P | PDPE_W;

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
	pde = pd[PD_IDX(va)] = page2pa(page4pt) | PDE_P | PTE_W;

pde_found:
	assert((pde & PDE_P) != 0);

	pte_t *pt = VADDR(PDE_ADDR(pde));

	return &pt[PT_IDX(va)];
}

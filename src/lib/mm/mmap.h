#ifndef __MMAP_H__
#define __MMAP_H__

#include <mmu.h>

#include <queue.h>
#include <stdbool.h>

#define SIZEOF_PAGE64	24
struct page {
	uint32_t ref;

	LIST_ENTRY(page) link;
};
LIST_HEAD(mmap_free_pages, page);

struct mmap_state {
	// Virtual address of PML4
	pml4e_t *pml4;

	// Virtual address of physical pages array
	struct page *pages;
	uint64_t pages_cnt;

	// List of the free pages
	struct mmap_free_pages free;
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

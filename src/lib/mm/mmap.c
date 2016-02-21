#include <assert.h>

#include "mmap.h"

struct free_pages free_pages = LIST_HEAD_INITIALIZER(free_pages);
struct page *pages;

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

uint64_t page2pa(struct page *p)
{
	uint64_t idx = (pages - p) / sizeof(*p);

	return idx * PAGE_SIZE;
}

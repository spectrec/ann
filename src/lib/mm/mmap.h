#ifndef __MMAP_H__
#define __MMAP_H__

#include <queue.h>
#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096

struct page {
	LIST_ENTRY(page) link;
	uint32_t ref;
};
LIST_HEAD(free_pages, page);

struct page *page_alloc(void);
void page_free(struct page *p);

void page_incref(struct page *p);
void page_decref(struct page *p);

uint64_t page2pa(struct page *p);

extern struct free_pages free_pages;
extern struct page *pages;

#endif

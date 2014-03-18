#ifndef     __PAGE_H__
#define     __PAGE_H__


#include "arch/asm.h"
#include "list.h"

#define  PAGE_NUM   (MEM_SIZE / 4096)
/* 32MB max order */
#define  MAX_ORDER  13
#define PAGE_SHIFT 12
#define PAGE_SIZE (1 << PAGE_SHIFT)
#define PAGE_AT_BUDDY 2
#define PAGE_OUT_BUDDY 3


/* page结构,每个page结构代表一个物理内存页 */
struct page {

};

/* buddy结点，这个结点主要包含一个链表数组，数组元素为MAX_ORDER */
struct buddy_node {

};


struct page* get_page_by_phyaddr(unsigned long phy_addr);
unsigned long get_phyaddr_by_page(struct page *pg);
void * alloc_pages(int order);
struct page * __alloc_page(int order);
void __free_pages(struct page *page, int order);
unsigned long get_phyaddr_by_page(struct page *pg);
struct page* get_page_by_phyaddr(unsigned long phy_addr);
void free_pages(void *addr, int order);
void test_buddy(void);

#define alloc_page() alloc_pages(0)
#define free_page(addr) free_pages(addr, 0)

#endif

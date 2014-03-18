#include "page.h"
#include "init.h"
#include "arch/io.h"
#include "arch/asm.h"
#include "slab.h"


extern unsigned long __bss_end__;
struct page *page_set;
static struct buddy_node mem_node;
/*
    kernel_free_start 动态内存管理开始区域
    kernel_free_end 动态内存管理结束区域
    ava_page_num 页总数
    kernel_page_start page结构存放的首地址
 */
unsigned long kernel_free_start, kernel_free_end, ava_page_num;
unsigned long kernel_page_start, kernel_page_end;

/* find page from phy_addr */
struct page* get_page_by_phyaddr(unsigned long phy_addr)
{
    unsigned int indx;
    struct page *pPage;
    if (phy_addr > kernel_free_end - 1||phy_addr < kernel_free_start) {
        printf("bad phy_addr: 0x%08x\n",phy_addr);
        return NULL;
    }

    indx = (phy_addr - kernel_free_start) / 4096;
    pPage = &page_set[indx];

    if (phy_addr < pPage->phy_addr || phy_addr > (pPage->phy_addr + 0x1000)) {
        ERROR();
        return NULL;
    }
    return pPage;
}

unsigned long get_phyaddr_by_page(struct page *pg)
{
    unsigned int indx;
    unsigned long addr;

    if (pg == NULL) {
        ERROR();
        return 0;
    }

    indx = (pg - page_set);
    addr = kernel_free_start + indx * 4096;

    if (addr > kernel_free_end - 1) {
        printf("pg is 0x%08x, indx is %d page->order=%d\n",pg, indx, pg->order);
        ERROR();
        return 0;
    }

    if (pg->phy_addr != addr) {
        printf("pg->phy_addr is 0x%08x, addr is 0x%08x\n",pg->phy_addr, addr, pg->order);
        ERROR();
        return 0;
    }
    return addr;
}

/*
    在伙伴系统中，两个互为伙伴的页总是满足以下两个条件:
    (1) 两个互为伙伴的页A，B，页序号分别为x，y。order为o，那么满足：
        y = x ^ (1 << order) 或 x = y ^ (1 << order)
    (2) 两个互为伙伴的页A，B，由P分解而来，A，B的order为o，P的order为o+1，A，B，P序号分别为x，y，z。那么有：
        z = x & ~(1 << order) 或 z = y & ~(1 << order)
*/
/*
    page: page结构
    pfn： 给定page的页框号
    order: 给定page的order

    返回给定page的伙伴page的struct page指针
*/
static inline struct page *__find_page(struct page *page, int pfn, int order)
{
    int pfn_buddy;
    struct page *buddy_page;
    pfn_buddy = pfn ^ (1 << order);
    buddy_page = page + (pfn_buddy - pfn);
    return buddy_page;
}

/*
    child_pfn：某页块的任意一个孩子的页框号.
    order：孩子的order

    返回给定孩子页框号的父母页框号，这个页框号可能是自己也可能是自己的伙伴。
*/
static inline int __find_combined_pfn(int child_pfn, int order)
{
    return (child_pfn & (~(1 << order)));
}


/*  分配current_order页后的buddy调整
    有(1 << current_order) - (1 << request_order)个页将逐级安排到低order的地方。
 */
static void modify_buddy_when_alloc(struct page *page, int request_order, int current_order)
{
    struct page *pg, *pg_iter;
    int size = (1 << current_order);
    int i;
    pg = page;
    while (current_order > request_order) {
        /* 向下移动一个order */
        current_order-- ;
        /* 该order的size为((1 << current_order) >> 1) * 0x1000Bytes */
        size >> 1;
        /* 调整排头页 */
        pg += (1 << current_order);
        /* pg 成为排头页 */
        list_add(&pg->pg_lst, &mem_node.node_head[current_order]);
        pg_iter = pg;
        for (i = 0; i < (1 << current_order); i++) {
            pg_iter->order = current_order;
            pg_iter++;
        }
        pg = page;
    }
}

/* 从伙伴系统中返回一个给定order的页块，页块大小为(1 << order) * 0x1000字节 */
static struct page *__alloc_page_order(int order)
{
    int cur_order = order;
    int iter;
    int i;
    struct page *page,*page_iter;
    /* 从给定order开始遍历 */
    for (iter = order; iter < MAX_ORDER; iter++) {
        /* 该order是空的 */
        if (list_empty(&mem_node.node_head[iter]))
            continue;
        page = list_entry(mem_node.node_head[iter].next, struct page , pg_lst);
        page_iter = page;
        /* 从链表中取下来 */
        list_del(&page->pg_lst);
        /* 将要分配的page的order设置为-1，表示该page不在伙伴系统中 */
        for (i = 0; i < (1 << order); i++) {
            page_iter->order = -1;
            page_iter->flag  = PAGE_OUT_BUDDY;
            page_iter += 1;
        }
        /* 可以使用的页减少 */
        mem_node.free_num -= (1 << order);
        /* 调整buddy系统 */
        modify_buddy_when_alloc(page, order, iter);
        return page;
    }
    return NULL;
}

/* 释放一个伙伴 */
static void __free_page_order(struct page *page, int order)
{
    int page_pfn,order_ret;
    int combined_indx;
    struct page *page_buddy,*pg,*pg_group;
    int i;
    page_pfn = page - page_set;
    pg = page;
    order_ret = order;
    /* MAX_ORDER - 1这个order的内存不需要再合并成更高阶的内存 */
    while (order < MAX_ORDER - 1) {
        page_buddy = __find_page(pg, page_pfn, order);
        /* 伙伴不在伙伴系统当中,或不在同阶上 */
        if (page_buddy->order == -1 || page_buddy->order != order || page_buddy->flag != PAGE_AT_BUDDY)
            break;
        list_del(&page_buddy->pg_lst);
        /* 找到父内存块，然后合并 */
        combined_indx = __find_combined_pfn(page_pfn, order);
        pg = pg + (combined_indx - page_pfn);
        page_pfn = combined_indx;
        order++;
    }
    pg_group = pg;

    for (i = 0; i < (1 << order); i++) {
        pg_group->order = order;
        pg_group->flag = PAGE_AT_BUDDY;
        pg_group++;
    }

    list_add(&pg->pg_lst, &mem_node.node_head[order]);
    mem_node.free_num += (1 << order_ret);
}

/* 我们不能保证分配的内存和释放的内存是同一块内存，这些问题必须由内核代码编程人员来负责 */
void __free_pages(struct page *page, int order)
{
    if (page == NULL) {
        printf("page is NULL\n");
        return;
    }

    if (page->order != -1) {
        printf("cannot free pages in the buddy system\n");
        return;
    }

    if (order < 0 || order > MAX_ORDER) {
        printf("bad order num...\n");
        return;
    }
    __free_page_order(page, order);
}

void free_pages(void *addr, int order)
{
    struct page *page;
    page = get_page_by_phyaddr((unsigned long)addr);
    __free_pages(page, order);
}

struct page * __alloc_page(int order)
{
    return __alloc_page_order(order);
}


void * alloc_pages(int order)
{
    struct page *page;
    unsigned long addr;
    page = __alloc_page(order);
    if (page == NULL) {
        printf("alloc page failed...\n");
        return NULL;
    }
    addr = get_phyaddr_by_page(page);
    return (void *)addr;
}

int get_buddy_info_by_order(int order)
{
    int i, j;
    struct list_head *lp = &mem_node.node_head[order];
    struct page *page, *pg;
    i = 0;
    j = 0;
    while (lp->next  != &mem_node.node_head[order]) {
        lp = lp->next;
        page = list_entry(lp, struct page, pg_lst);
        for (j = 0; j < (1 << order); j++) {
            if (page->order == -1)
                printf("find bad page, page is 0x%08x, page's addr is 0x%08x, page_fn is %d\n",
                    page, page->phy_addr, (page - page_set));
/*
            printf("--------------------\n");
            printf("page pfn: %d, page order: %d, page addr: 0x%08x\n", (int)(page - page_set), page->order, page->phy_addr);
*/
            page++;
        }
//        printf("--------------------\n");
        i++;
    }
    return i;
}

void test_buddy(void)
{
    int i, j;

    printf("buddy has %d pages\n",mem_node.free_num);
    for (i = 0;i < MAX_ORDER; i++) {
        printf("---------------------------\n");
        j = get_buddy_info_by_order(i);
        printf("---------------------------\n");
        printf("order%d: page num is %d\n",i,j);
        printf("---------------------------\n");
    }
}

static void init_page(void)
{
//    struct page *pages[2];
    page_set = (struct page *)kernel_page_start;
    unsigned long padding_size;
    int iter;
    padding_size = kernel_free_start - kernel_page_end;
    memset((void *)kernel_page_end, 0x0, padding_size);

    for (iter = 0; iter < MAX_ORDER; iter++)
        INIT_LIST_HEAD(&mem_node.node_head[iter]);

    for (iter = 0; iter < ava_page_num; iter ++ ) {
        page_set[iter].phy_addr = (kernel_free_start + iter * 4096);
        page_set[iter].order = -1;
    }

    for (iter = 0; iter < ava_page_num; iter++)
        __free_pages(&page_set[iter], 0);
/*
    for (iter = 0; iter < 9; iter++)
        __free_pages(&page_set[iter], 0);
*/
/*
    for (iter = 0; iter < 2; iter++)
       pages[iter] = __alloc_page(1);

    for (iter=0; iter < 2; iter++)
        __free_pages(pages[iter], 1);
*/
    test_buddy();
    printf("init struct page done...\n");

    init_kmem_cache();
    test_buddy();
}
/*
        ------------
            stack
        ------------     <---- stack_end
         dynamic mem           70MB ~ 75MB
        ------------     <---- kernel_free_start
        ------------
         page struct
        ------------     <---- kernel_page_start
        ------------     <---- bss_end
           kernel
        ------------     <---- 0x30000000
*/
static void scan_memory(void)
{
    unsigned long bss_end = (unsigned long)&__bss_end__;
    /* #define PAGE_ALIGN(addr) (((addr) + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1))) */
    kernel_free_start = ((bss_end + 0x1000 - 1) & (~(0x1000 - 1)));
    kernel_free_end = STACK_SAFE;
    kernel_page_start = ((bss_end + 0x1000 - 1) & (~(0x1000 - 1)));
    kernel_free_start = (kernel_free_start + ((STACK_SAFE - kernel_free_start)/(0x1000 + sizeof(struct page))) * sizeof(struct page));
    kernel_free_start = (kernel_free_start + 0x1000 - 1) & (~(0x1000 - 1));
    ava_page_num =  (kernel_free_end - kernel_free_start) / 0x1000;
    kernel_page_end = kernel_page_start + ava_page_num * sizeof(struct page);
    printf("get memory info:\n");
    printf("size of page : %d\n", sizeof(struct page));
    printf("bss ends at 0x%08x\n",bss_end);
    printf("stack ends at 0x%08x\n",STACK_SAFE);
    printf("free memory starts at 0x%08x\n",kernel_free_start);
    printf("free memory ends at 0x%08x\n", kernel_free_end);
    printf("kernel page struct starts at 0x%08x\n",kernel_page_start);
    printf("kernel page struct ends at 0x%08x\n",kernel_page_end);
    printf("total free memory : 0x%08x\n", kernel_free_end - kernel_free_start);
    printf("total free pages  : %lu\n",ava_page_num);

    init_page();
}


subsys_initcall(scan_memory);

#include "page.h"
#include "init.h"
#include "arch/io.h"
#include "arch/asm.h"



extern unsigned long __bss_end__;

/* page对象数组的开始指针 */

/*----------------------------------------*/
/* buddy的管理结构，buddy_node结构的实例 */

/*----------------------------------------*/
/*
    kernel_free_start 动态内存管理开始区域
    kernel_free_end 动态内存管理结束区域
    ava_page_num 页总数
    kernel_page_start page结构存放的首地址
    kernel_page_end   page结构存放的结束地址
 */

/*----------------------------------------*/

/* get_page_by_phyaddr函数，这个函数给定一个物理地址返回这个物理地址页对应的page结构 */
struct page* get_page_by_phyaddr(unsigned long phy_addr)
{
	return NULL;
}


/* get_phyaddr_by_page函数，这个函数给定一个page结构返回这个结构对应页的首地址 */
unsigned long get_phyaddr_by_page(struct page *pg)
{
    return 0;
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
	return NULL;
}

/*
    child_pfn：某页块的任意一个孩子的页框号.
    order：孩子的order
    返回给定孩子页框号的父母页框号，这个页框号可能是自己也可能是自己的伙伴。
*/
static inline int __find_combined_pfn(int child_pfn, int order)
{
    return 0;
}


/*
 *  分配current_order页后的buddy调整
    有(1 << current_order) - (1 << request_order)个页将逐级安排到低order的地方。
	我们应该首先实现__alloc_page_order函数
 */
static void modify_buddy_when_alloc(struct page *page, int request_order, int current_order)
{

    /* 一直从当前order分解到需要的order处 */

        /* 向下移动一个order */

        /* 该order的size为((1 << current_order) >> 1) * 0x1000Bytes */

        /* 调整排头页 */

        /* pg 成为排头页 */

        /* 重新设置order */
}

/* 从伙伴系统中返回一个给定order的页块，页块大小为(1 << order) * 0x1000字节 */
static struct page *__alloc_page_order(int order)
{
    /* 从给定order开始遍历 */

        /* 该order是空的 */

        /* 从链表中取下来 */

        /* 将要分配的page的order设置为-1，表示该page不在伙伴系统中 */

        /* 可以使用的页减少 */

        /* 调整buddy系统 */

    return NULL;
}

/* 释放一个来自伙伴系统的页 */
static void __free_page_order(struct page *page, int order)
{

    /* MAX_ORDER - 1这个order的内存不需要再合并成更高阶的内存 */
        /* 伙伴不在伙伴系统当中,或不在同阶上 */

        /* 找到父内存块，然后合并 */
}

/* 我们不能保证分配的内存和释放的内存是同一块内存，这些问题必须由内核代码编程人员来负责 */
void __free_pages(struct page *page, int order)
{

}


void free_pages(void *addr, int order)
{

}

struct page * __alloc_page(int order)
{

}


void * alloc_pages(int order)
{

}

/* debug用 */
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

/* debug用 */
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

}


subsys_initcall(scan_memory);

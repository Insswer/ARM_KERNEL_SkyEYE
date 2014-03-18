#include "arch/mmu.h"
#include "arch/io.h"
#include "arch/asm.h"




unsigned long coarse_page_entry;
unsigned long ttb_page_entry;
unsigned long page_table_end;

void alloc_page_table(unsigned long ttb_start)
{
    /* TTB has 4096 entry, per entry uses 4B ,total need 16KB */
    ttb_page_entry = ttb_start & (~(0x4000 - 1));
    /* Coarse page table has 256 entry, per entry uses 4B, total = 4*256 = 1K
       if we use 4KB page, we need 4096 * 1K = 4MB
    */
    /*
        Coarse page table align 1K
    */
    coarse_page_entry = (ttb_start + 0x4000) & (~(0x400 - 1));
    page_table_end = coarse_page_entry + 0x400000 ;
    memset((unsigned long *)page_table_end, 0, 4096);
}

/* num 以1MB为单位 */
void build_section(unsigned long target_va,unsigned long target_pa,
                   int num, int domain, int AP, int CB)
{
    int i;
    unsigned long tmp = 0;
    unsigned long start_entry;
    unsigned long start_pa = (target_pa & 0xfff00000) >> 20;
    tmp = (target_va & 0xfff00000) >> 20;
    start_entry = (tmp << 2)|ttb_page_entry;
    for (i = 0;i < num;i ++) {
        *(volatile unsigned long *)start_entry = ((start_pa << 20)|(AP << 10)|(domain << 5)|(1 << 4)|(CB << 2)|(SECTION));
        start_entry += 4;
        start_pa += 1;
    }
}

static void enable_mmu(void)
{
    unsigned long tmp;
    tmp = __get_cp15_control();
    tmp &= ~((1 << 0)|(1 << 2)|(1 << 7)|(1 << 12)|(1 << 13));
    tmp |= ((1 << 0)|(1 << 13));
    __set_cp15_control(tmp);
}

/* 返回2级页表所在内存的位置 */
static unsigned long find_and_build_level1_page_table(unsigned long target_va, int domain)
{
    unsigned long tmp_va, tmp_start, tmp_level1;
    int indx;
    tmp_va = (target_va & 0xfff00000) >> 18;
    tmp_start = (tmp_va | ttb_page_entry) & (~(0x3));
    /* 找到1级页表的序号 */
    indx = (tmp_start - ttb_page_entry) / 4;
    /* 页表不存在， 创建一个 */
    if ((read32(tmp_start) & 0x3 )!= COARSE_TABLE_ADDRESS) {
//        printf_raw("page table not exist...build it at 0x%08x\n",tmp_start);
        /* 二级页表以256个entry为单位排列在一级页表之上 */
        tmp_level1 = coarse_page_entry + (indx * 1024);
        *(volatile unsigned long *)tmp_start = 0x0;
        *(volatile unsigned long *)tmp_start = ((tmp_level1 & 0xfffffc00)|(domain << 5)|(0x1 << 4)|(COARSE_TABLE_ADDRESS << 0));
    } else {
        tmp_level1 = *(volatile unsigned long *)tmp_start;
        tmp_level1 &= (0xfffffc00);
    }
//    printf_raw("level2 page table is at 0x%08x\n", tmp_level1);
    return tmp_level1;
}


void build_small_page_tbl(unsigned long target_va, unsigned long target_pa,
                     int domain, int AP, int CB)
{
    int i,j;
    unsigned long start_l2_entry;
    unsigned long page_offset, page_base;
    unsigned long l2_page_table_offset,l2_page_table_offset_pa;

    page_offset = (target_pa & (0xfff));
    page_base = (target_pa & 0xfffff000);
    l2_page_table_offset_pa = ((target_va & (0x000ff000)) >> 10);
    start_l2_entry = find_and_build_level1_page_table(target_va, domain);
    l2_page_table_offset = (start_l2_entry | l2_page_table_offset_pa);
//    printf_raw("map l2 page at 0x%08x\n",l2_page_table_offset);
    /* clear it first */
    *(volatile unsigned long *)l2_page_table_offset = 0;
    *(volatile unsigned long *)l2_page_table_offset = ((page_base)|(AP << 10)|(AP << 8)|(AP << 6)|(AP << 4)|(CB << 2)|(SMALL_PAGE << 0));
}

/* 最大一次映射1MB的空间 */
void build_small_pages(unsigned long target_va, unsigned long target_pa,
                     int num, int domain, int AP, int CB)
{
        int i;
        if (num > 256) {
            printf("build too many maps...\n");
            return;
        }
        for (i = 0; i < num; i++)
            build_small_page_tbl((target_va + 4096 * i), (target_pa + 4096 * i), domain, AP, CB);
}

void init_mmu(void)
{
    printf_raw("now init mmu...\n");
    /* do not check domain, we always run in sys mode */
    __set_domain_reg(0xffffffff);
    alloc_page_table(PAGE_TABLE);
    int i;
    if (ttb_page_entry != 0)
        __set_ttb_reg(ttb_page_entry & (0xffffc000));
    else {
        printf_raw("error when alloc page table for ttb\n");
        return;
    }
//    build_section(0xd8000000, 0x48000000, 512, DOMAIN_MANAGER, AP_SVC_RW_USR_RW, CB_NO_CACHE_NO_BUFFER);
//    build_section(0x30000000, 0x30000000, 2, DOMAIN_MANAGER, AP_SVC_RW_USR_RW, CB_NO_CACHE_NO_BUFFER);

    /* should be replaced by ioremap */
    build_small_pages(0xe0000000, 0x50000000, 1, DOMAIN_MANAGER, AP_SVC_RW_USR_RW, CB_NO_CACHE_NO_BUFFER);
    build_small_pages(0x51000000, 0x51000000, 1, DOMAIN_MANAGER, AP_SVC_RW_USR_RW, CB_NO_CACHE_NO_BUFFER);
    build_small_pages(0x4a000000, 0x4a000000, 1, DOMAIN_MANAGER, AP_SVC_RW_USR_RW, CB_NO_CACHE_NO_BUFFER);
    printf_raw("build I/O page table done...\n");
    printf_raw("start building page tables for OS...\n");
    for (i = 0; i < 256; i ++) {
        build_small_pages(0x30000000 + i * 0x100000, 0x30000000 + i * 0x100000, 256, DOMAIN_MANAGER, AP_SVC_RW_USR_RW, CB_NO_CACHE_NO_BUFFER);
    }
    build_small_pages(0xffff0000, 0x30000000, 1, DOMAIN_MANAGER, AP_SVC_RW_USR_RW, CB_NO_CACHE_NO_BUFFER);
    /* map zero page */
    build_small_pages(0x00000000, page_table_end, 1, DOMAIN_MANAGER, AP_SVC_RW_USR_RW, CB_NO_CACHE_NO_BUFFER);
    printf_raw("build page table done...\n");
    enable_mmu();

}


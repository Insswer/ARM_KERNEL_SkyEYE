#include "mmu.h"
#include "io.h"
#include "asm.h"




unsigned long ttb_page_entry;
unsigned long page_table_end;

void alloc_page_table(unsigned long ttb_start)
{
    /* TTB has 4096 entry, per entry uses 4B ,total need 16KB */
    ttb_page_entry = ttb_start;

    page_table_end = ttb_page_entry + 0x4000 ;
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




void init_mmu(void)
{
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
    build_section(0xd8000000, 0x48000000, 512, DOMAIN_MANAGER, AP_SVC_RW_USR_RW, CB_NO_CACHE_NO_BUFFER);
    build_section(0x30000000, 0x30000000, 256, DOMAIN_MANAGER, AP_SVC_RW_USR_RW, CB_NO_CACHE_NO_BUFFER);
    printf_raw("build page table done...\n");
    enable_mmu();
}


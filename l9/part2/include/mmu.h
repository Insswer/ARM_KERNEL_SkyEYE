#ifndef     __MMU_H__
#define     __MMU_H__

#include "asm.h"

#ifdef ARMv4

/*
    THIS FILE ONLY IMPLEMENT ARMv4,v5 MMU NOW!
*/


/*
    define DOMAIN
*/
#define     DOMAIN_NO_ACCESS        0x0
#define     DOMAIN_CLIENTS          0x1
#define     DOMAIN_RESERVED         0x2
#define     DOMAIN_MANAGER          0x3

/*
    define AP, when ARMv4, AP0/AP1/AP2/AP3 every AP has 1/4 memory
*/

#define     AP_SVC_RW_USR_RW        0x3
#define     AP_SVC_RW_USR_NA        0x1
#define     AP_SVC_RW_USR_RO        0x2

/*
    define C&B
*/

#define     CB_NO_CACHE_NO_BUFFER   0x0
//#define   CB_SHARE_DEVICE         0x1
#define     CB_WRITE_THROUGH        0x2
#define     CB_WRITE_BACK           0x3

/*
    define LVL1 PAGE_KIND
*/

#define     COARSE_TABLE_ADDRESS    0x1
#define     SECTION                 0x2
#define     FINE_TABLE_ADDRESS      0x3

/*
    define LVL2 PAGE_KIND
*/

#define     LARGE_PAGE              0x1
#define     SMALL_PAGE              0x2
#define     TINY_PAGE               0x3

/* 段只映射1MB的空间 */
void build_section(unsigned long target_va,unsigned long target_pa,
                   int num, int domain, int AP, int CB);
void build_small_page_tbl(unsigned long target_va, unsigned long target_pa,
                     int domain, int AP, int CB);
void build_small_pages(unsigned long target_va, unsigned long target_pa,
                     int num, int domain, int AP, int CB);

void init_mmu(void);
void alloc_page_table(unsigned long);

#elif defined ARMv7


#endif/* ARMv7 */

#endif

/* 这个文件我们要实现MMU的分段功能
 * 我们要实现下面几个函数：
 * void init_mmu(void);这个函数用于初始化MMU
 * static void enable_mmu(void);这个函数用于启动MMU
 * void build_section(unsigned long target_va,unsigned long target_pa,
                   int num, int domain, int AP, int CB);这个函数用于分段
   void alloc_page_table(unsigned long ttb_start);这个函数用于分配页表。
 *
 *  */



#include "mmu.h"
#include "io.h"
#include "asm.h"



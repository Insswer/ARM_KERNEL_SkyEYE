/* 指明输出的体系 */
OUTPUT_ARCH(arm)
/* 指明入口函数 */
ENTRY(_start)

/* 链接器的输出段信息 */
SECTIONS
{
    /* .表示当前链接地址，这里设置链接地址的起始地址为0x30000000 */
	. = 0x30000000;
	/* text段 */
	.text :
	{
        *(.vector)
        . = ALIGN(4096);
    /* start.o的text段放在前面 */
	arch/start.o(.text)
	*(.text)
	}
	/* 这里要求.的链接地址32字节对齐 */
	. = ALIGN(32);
    .init.text : {
		*(.init.text)
	}

	__init_start_arch = .;
	.initcall_arch : {
		*(.initcall_arch)
	}
	__init_end_arch = .;


	__init_start_subsys = .;
	.initcall_subsys : {
        *(.initcall_subsys)
	}
    __init_end_subsys = .;

    . = ALIGN(32);
    /* 数据段 */
	.data :
	{
		*(.data)
	}
    /* 对齐 */
	. = ALIGN(32);
	/* 链接脚本中可以定义符号输出，这些符号可以在程序中通过extern的方式引用 */
	__bss_start__	= .;
	/* bss段 */
	.bss :
	{
		*(.bss)
	}
	__bss_end__ = .;


}

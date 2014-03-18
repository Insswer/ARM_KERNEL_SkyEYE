
/* 这个汇编文件我们要实现和协处理器相关的一些寄存器的操作：
	__get_cp15_control函数用于获得cp15寄存器中控制寄存器的值
	__set_cp15_control函数用于设置CP15寄存器中控制寄存器的值
	__set_ttb_reg函数用于设置TTB寄存器
	__set_domain_reg函数用于设置DOMAIN寄存器
 */
    .global __get_cp15_control
    .global __set_cp15_control
    .global __set_ttb_reg
    .global __set_domain_reg




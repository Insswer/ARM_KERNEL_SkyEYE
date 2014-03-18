#ifndef     __INIT_H__
#define     __INIT_H__

#define		__init		__attribute__((section(".init.text")))
typedef		void(*initcall_t)(void);
extern	initcall_t	__init_start[], __init_end[];

#define		arch_initcall(fn)	\
	static initcall_t __initcall_arch_##fn __attribute__((unused,__section__(".initcall_arch"))) = fn

#define     subsys_initcall(fn) \
    static initcall_t __init_call_sub_##fn __attribute__((unused,__section__(".initcall_subsys"))) = fn

void  start_kernel(void);


#endif

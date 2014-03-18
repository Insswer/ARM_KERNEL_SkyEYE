#ifndef     __INIT_H__
#define     __INIT_H__

#define		__init		__attribute__((section(".init.text")))
typedef		void(*initcall_t)(void);
extern	initcall_t	__init_start_arch[], __init_end_arch[];
extern  initcall_t  __init_start_subsys[], __init_end_subsys[];
#define		arch_initcall(fn)	\
	static initcall_t __initcall_arch_##fn __attribute__((unused,__section__(".initcall_arch"))) = fn

#define     subsys_initcall(fn) \
    static initcall_t __init_call_sub_##fn __attribute__((unused,__section__(".initcall_subsys"))) = fn

void  start_kernel(void);

#define min(x, y) ({				\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; })

#define max(x, y) ({				\
	typeof(x) _max1 = (x);			\
	typeof(y) _max2 = (y);			\
	(void) (&_max1 == &_max2);		\
	_max1 > _max2 ? _max1 : _max2; })



#endif

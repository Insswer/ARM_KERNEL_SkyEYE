#include "slab.h"
#include "page.h"
#include "arch/io.h"
#include "init.h"

/* 和CPU的L1缓存的CACHE LINE相等，提高性能 */
#define CACHE_LINE_SIZE 32

static int slub_min_order;
static int slub_max_order = 3;
static int slub_min_objects;
/* 在我们slub分配器没有正常工作前我们不能使用动态内存来分配kmem_cache */
static struct kmem_cache kmem_cache;
/* 用于kmalloc的实现 */
static struct kmem_cache kmalloc_cache[KMALLOC_INDEX_MAX];

/* 返回给定order中，能放入多少个Objects */
static inline int order_objects(int order, unsigned long size)
{
	return ((PAGE_SIZE << order)) / size;
}

/* 通过大小，返回适合的order */
static inline int get_order(unsigned long size)
{
	int order;

	size = (size - 1) >> (PAGE_SHIFT - 1);
	order = -1;
	do {
		size >>= 1;
		order++;
	} while (size);
	return order;
}

/* 给定最小objects数目要求和最大order计算较合适的order */
static int slab_order(int size, int min_objects,
				int max_order, int fract_leftover)
{
	int order;
	int rem;
	int min_order = slub_min_order;
    /* 当前最小order下对象太多了 */
	if (order_objects(min_order, size) > MAX_OBJS_PER_PAGE)
		return get_order(size * MAX_OBJS_PER_PAGE) - 1;


	for (order = max(min_order,
                  /* 找到最低对齐位对应的order，然后和最小order选个最大的 */
				fls(min_objects * size - 1) - PAGE_SHIFT);
			order <= max_order; order++) {

		unsigned long slab_size = PAGE_SIZE << order;

        /* slab太小了 */
		if (slab_size < min_objects * size)
			continue;
        /*  计算剩余字节数   */
		rem = (slab_size) % size;
        /* 剩余字节数满足要求，也就是最大不浪费总共大小的四分之一 */
		if (rem <= slab_size / fract_leftover)
			break;

	}

	return order;
}

static int calculate_order(int size)
{
	int order;
	int min_objects;
	int fraction;
	int max_objects;

    /* 计算最小允许的object数目和最大的object数目 */
	min_objects = slub_min_objects;
	if (!min_objects)
		min_objects = MIN_OBJECT;
	max_objects = order_objects(slub_max_order, size);
	min_objects = min(min_objects, max_objects);

    /* 还是不行，缩小最小对象的要求 */
	while (min_objects > 1) {
		fraction = 16;
		while (fraction >= 4) {
		    /* 给定size和最小object数目计算order */
			order = slab_order(size, min_objects,
					slub_max_order, fraction);
            /* 该size和objects数目计算出的order比较适合 */
			if (order <= slub_max_order)
				return order;
            /* 降低粒度，也就是允许更多的浪费 */
			fraction /= 2;
		}
		min_objects--;
	}

	/*
	 尝试将单个对象放入给定块中
	 */
	order = slab_order(size, 1, slub_max_order, 1);
	if (order <= slub_max_order)
		return order;

	/*
	尝试是否能放入最大的order
	 */
	order = slab_order(size, 1, MAX_ORDER, 1);
	if (order < MAX_ORDER)
		return order;
	return -1;
}

/*-------------------------------------------------------------------------------*/
/* 以上函数来自Linux内核，上面函数通过贪心算法来计算适合的object数目 */
/*-------------------------------------------------------------------------------*/

/* 获得对象数目，并组织好链表指针，这个函数的主要功能就是将对象之间的指针设置好 */
static inline int kmem_cache_line_object(void *head, unsigned int size, int order)
{
    return 0;
}

/* 给定kmem_cache和对象大小，创建一个slub实例 */
struct kmem_cache *kmem_cache_create(struct kmem_cache *cache, unsigned int size)
{
   return NULL;
}

/* 从kmem_cache中分配内存 */
void *kmem_cache_alloc(struct kmem_cache *cache)
{
	return NULL;
}

/* 释放从kmem_cache中分配到的对象 */
void kmem_cache_free(struct kmem_cache *cache, void *objp)
{

}

/* 销毁一个kmem_cache，并将kmem_cache的资源返还给伙伴系统 */
void kmem_cache_destroy(struct kmem_cache *cache)
{

}

/* 初始化kmalloc函数需要的slub分配器 */
static void init_kmalloc(void)
{

}

/* 初始化slub快速分配器子系统 */
void init_kmem_cache(void)
{

}

/* 根据大小选择一个合适的分配阶 */
static inline int kmalloc_index(unsigned int size)
{
    if (!size)
        return -1;
    if (size > KMALLOC_MAX_SIZE)
        return -1;
    if (size <=          8) return 0;
	if (size <=         16) return 1;
	if (size <=         32) return 2;
	if (size <=         64) return 3;
	if (size <=        128) return 4;
	if (size <=        256) return 5;
	if (size <=        512) return 6;
	if (size <=       1024) return 7;
	if (size <=   2 * 1024) return 8;
	if (size <=   4 * 1024) return 9;
	if (size <=   8 * 1024) return 10;
	if (size <=  16 * 1024) return 11;
	if (size <=  32 * 1024) return 12;
	if (size <=  64 * 1024) return 13;
	if (size <= 128 * 1024) return 14;
	if (size <= 256 * 1024) return 15;
	if (size <= 512 * 1024) return 16;
	if (size <= 1024 * 1024) return 17;
	if (size <=  2 * 1024 * 1024) return 18;
}

/* malloc函数的内核等价实现 */
void *kmalloc(unsigned int size)
{

}

/* free函数的内核等价实现 */
void kfree(void *addr)
{
    struct kmem_cache *kmem_cache;
    if (addr == NULL) {
        return;
    }
    struct page *page = get_page_by_phyaddr((unsigned long)addr);
    if (page == NULL) {
        ERROR();
        return;
    }

    if (page == NULL) {
        ERROR();
        return;
    }
    kmem_cache = page->cachep;
    kmem_cache_free(kmem_cache, addr);
}


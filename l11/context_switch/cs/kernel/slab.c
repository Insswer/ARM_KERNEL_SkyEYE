#include "slab.h"
#include "page.h"
#include "arch/io.h"
#include "init.h"

#define CACHE_LINE_SIZE 32

static int slub_min_order;
static int slub_max_order = 3;
static int slub_min_objects;
static struct kmem_cache kmem_cache;
static struct kmem_cache kmalloc_cache[KMALLOC_INDEX_MAX];

static inline int order_objects(int order, unsigned long size)
{
	return ((PAGE_SIZE << order)) / size;
}

/* get order from size */
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


/* 获得对象数目，并组织好链表指针 */
static inline int kmem_cache_line_object(void *head, unsigned int size, int order)
{
    /* 指针长度为unsigned long */
    unsigned long p1;
    char *p;
    p1 = (unsigned long)head;
    p = (unsigned char *)head + size;
    int i,s = 0x1000 * (1 << order);
    for (i = 0; s > size; i++, s-=size) {
        /* 创建链表 */
        *(unsigned long *)p1 = (unsigned long)p;
        p1 = (unsigned long)p;
        p = p + size;
    }
    if (s == size)
        i++;
    return i;
}

struct kmem_cache *kmem_cache_create(struct kmem_cache *cache, unsigned int size)
{
    unsigned int size_align;
    int i;
    struct page *page;
    /* 对齐到cache line的大小 */
    size_align = (size + CACHE_LINE_SIZE - 1)&(~(CACHE_LINE_SIZE - 1));

    int order = calculate_order(size_align);

    if (order == -1) {
        ERROR();
        return NULL;
    }

    cache->page_order = order;
    if ((cache->head_page = __alloc_page(order)) == NULL) {
        ERROR();
        return NULL;
    }
    page = cache->head_page;

    for (i = 0; i < (1 << order); i++) {
            page->cachep = cache;
            page++;
    }
    cache->obj_block = (void *)get_phyaddr_by_page(cache->head_page);
    cache->obj_num = kmem_cache_line_object(cache->obj_block, size, order);
    cache->obj_size = size_align;

    return cache;
}


void *kmem_cache_alloc(struct kmem_cache *cache)
{
    unsigned long p;
    unsigned long next;
    int i;
    struct page *page,*pg;
    if (cache == NULL) {
        ERROR();
        return NULL;
    }

    int order  = cache->page_order;

    if (!cache->obj_num) {
        if ((page = __alloc_page(order)) == NULL) {
            ERROR();
            return NULL;
        }
        cache->obj_block = (void *)get_phyaddr_by_page(page);
        /* 重新构建链表 */
        cache->obj_num += kmem_cache_line_object(cache->obj_block, cache->obj_size, order);
        for (i = 0; i < (1 << cache->page_order); i++) {
            page->cachep = cache;
            page++;
        }
    }
    cache->obj_num--;
    /* 获得对象 */
    p = (unsigned long)(cache->obj_block);
    next = (*(unsigned long *)p);
    cache->obj_block = (void *)next;
    pg = get_page_by_phyaddr((unsigned long)p);
    pg->cachep = cache;
    return (void *)p;
}


void kmem_cache_free(struct kmem_cache *cache, void *objp)
{
    if (objp == NULL) {
        ERROR();
        return;
    }
    *(unsigned long *)objp = (unsigned long)(cache->obj_block);
    cache->obj_block = (void *)objp;
    cache->obj_num++;
}

/* 销毁一个kmem_cache，并将kmem_cache的资源返还给伙伴系统 */
void kmem_cache_destroy(struct kmem_cache *cache)
{
    int order = cache->page_order;
    struct page *pg = cache->head_page;
    int i;
    for (i = 0; i < (1 << order); i++) {
        (pg+i)->cachep = NULL;
        __free_pages(pg, 0);
    }
}

static void init_kmalloc(void)
{
    int i;
    for (i = 0; i < KMALLOC_INDEX_MAX; i ++) {
        memset(&kmalloc_cache[i], 0, sizeof(struct kmem_cache));
        kmem_cache_create(&kmalloc_cache[i], (1 << (i + 3)));
        if (kmalloc_cache[i].obj_block == NULL) {
            ERROR();
            return;
        }
        printf("kmalloc[%d] objsize = %d, obj_num = %d, order = %d, start_page=0x%08x, obj_block point to 0x%08x\n",
                        i, kmalloc_cache[i].obj_size, kmalloc_cache[i].obj_num, kmalloc_cache[i].page_order, (unsigned long)(kmalloc_cache[i].head_page),
                            (unsigned long)(kmalloc_cache[i].obj_block));
    }
}


void init_kmem_cache(void)
{
    memset(&kmem_cache, 0, sizeof(struct kmem_cache));
    kmem_cache_create(&kmem_cache, sizeof(struct kmem_cache));
    init_kmalloc();
}

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
	return -1;
}

void *kmalloc(unsigned int size)
{
    int indx = kmalloc_index(size);
    if (indx < 0)
        return NULL;
    return kmem_cache_alloc(&kmalloc_cache[indx]);
}

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


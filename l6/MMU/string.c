#include "string.h"


unsigned int strnlen(const char * s,unsigned int count)
{
	const char *sc;

	for (sc = s; count-- && *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}


void * memcpy(void * dest,const void *src,unsigned int count)
{
	char *tmp = (char *) dest, *s = (char *) src;
	while (count--)
		*tmp++ = *s++;
	return dest;
}

/* 我们还要实现一个 */
void *memset(void *s, int c, unsigned int count)
{
	char *xs = s;

	while (count--)
		*xs++ = c;
	return s;
}

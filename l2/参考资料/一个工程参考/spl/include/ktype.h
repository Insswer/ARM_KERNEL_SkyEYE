#ifndef _KTYPE_H_
#define _KTYPE_H_

#define NULL (void *)0
#define true 1
#define false 0
/* the common type*/
typedef char * string; 
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef volatile unsigned char vu8;
typedef volatile unsigned short vu16;
typedef volatile unsigned int vu32;
typedef volatile unsigned long long vu64;
typedef s8 bool;
typedef u8 byte;
#endif /*_KTYPE_H_*/

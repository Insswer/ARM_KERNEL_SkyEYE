

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP 	= $(CROSS_COMPILE)objdump
RANLIB 		= $(CROSS_COMPILE)RANLIB
MAKE 		= make

OPTFLAGS = 
PLATFORM_CPPFLAGS = 


ifndef LDSCRIPT
LDSCRIPT := $(TOPDIR)/arch/link.lds
endif


CPPFLAGS := $(OPTFLAGS) -I$(TOPDIR)/include -fno-builtin -nostdlib 
AFLAGS   := $(CPPFLAGS)
CFLAGS 	 := $(CPPFLAGS)
LDFLAGS  := -T$(LDSCRIPT)

export CROSS_COMPILE AS LD CC AR NM STRIP OBJCOPY OBJDUMP MAKE TOPDIR
export CPPFLAGS CFLAGS AFLAGS LDFLAGS PLATFLAG

####################################################################


%.o : %.s
	$(CC) $(AFLAGS) -c -o $@ $<
%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
%.d : %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
%.d : %.s
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$



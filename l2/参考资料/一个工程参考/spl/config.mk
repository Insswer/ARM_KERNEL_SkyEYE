PLATFORM_RELFALGS = -fno-strict-aliasing  -fno-common -ffixed-r8 \
	-msoft-float
PLATFORM_CPPFLAGS = -march=armv7-a 
PLATFORM_LDFLAGS = 
HOSTCFLAGS	= -Wall -Wstrict-prototypes -fomit-frame-pointer


cc-option = $(shell if $(CC) $(CFLAGS) $(1) -S -o /dev/null -xc /dev/null \
		> /dev/null 2>&1; then echo "$(1)"; else echo "$(2)"; fi ;)

PLATFORM_CPPFLAGS +=$(call cc-option,-mapcs-32,-mabi=apcs-gnu)
PLATFORM_RELFLAGS +=$(call cc-option,-mshort-load-bytes,$(call cc-option,-malignment-traps,))

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY	= $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
RANLIB 	= $(CRPSS_COMPILE)RANLIB
MAKE 	= make

RELFLAGS = $(PLATFORM_RELFALGS)

ifndef LDSCRIPT
LDSCRIPT := $(TOPDIR)/cpu/zygote.lds
endif

OPTFLAGS= -O2


OBJCFLAGS += --gap-fill=0x0
gccincdir := $(shell $(CC) -print-file-name=include)

CPPFLAGS := $(OPTFLAGS) $(RELFLAGS) -D__KERNEL__ -I$(TOPDIR)/include -fno-builtin -ffreestanding -nostdinc -isystem $(gccincdir) -pipe $(PLATFORM_CPPFLAGS)
AFLAGS := -Wa,-gstabs -D__ASSEMBLY__ $(CPPFLAGS)
CFLAGS := $(CPPFLAGS) -Wall -Wstrict-prototypes
LDFLAGS += -T$(LDSCRIPT)  


export CROSS_COMPILE AS LD CC AR NM STRIP OBJCOPY OBJDUMP MAKE
export CPPFLAGS CFLAGS 

#############################################################

%.o : %.s
	$(CC) $(AFLAGS) -c -o $@ $(CURDIR)/$<
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
	
#.depend : Makefile $(SOBJS:.o=.s) $(OBJS:.o=.c) $(AOBJS:.o:.s) $(START:.o=.s) $(COBJS:.o=.c)
#		$(CC) -M $(CPPFLAGS) $(SOBJS:.o=.s) $(OBJS:.o=.c) | \
#			sed -e "s/\:/\\\:/g"|sed -e "s/\\\: /\: /g"> $@


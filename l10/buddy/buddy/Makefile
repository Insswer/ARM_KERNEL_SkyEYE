#####################################################################
#		DEFINE TOPDIR					    #
#####################################################################

TOPDIR = $(shell pwd)
export TOPDIR


CROSS_COMPILE = arm-elf-
export CROSS_COMPILE

include $(TOPDIR)/config.mk

###########################################################################
OBJS = arch/start.o
OBJS_INIT = arch/int_control.o arch/timer.o kernel/page.o

LIBS += arch/libarch.a
LIBS += kernel/libkernel.a
LIBS += lib/lib.a


.PHONY : $(LIBS) all
PLATFLAG := -L $(shell dirname `$(CC) $(CFLAGS) -print-libgcc-file-name`) -lgcc
###########################################################################

all: main.bin

main.bin:main.elf
	$(OBJCOPY) -O binary $< $@
	$(OBJDUMP) -D main.elf > main.dis

main.elf:$(OBJS) $(OBJS_INIT) $(LIBS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) $(OBJS) $(OBJS_INIT) --start-group $(LIBS) --end-group $(PLATFLAG) -Map main.map -o $@

$(LIBS):
	$(MAKE) -C `dirname $@`

clean:
	find . -type f \
		\( -name 'core' -o -name '*.bak' -o -name '*~' \
		-o -name '*.o' -o -name '*.a' -o -name '*.d.*' -o -name '*.d' -o -name '*.dis' \) -print \
		| xargs rm -f

clobber: clean
	find . -type f \
		\( -name .depend -o -name '*.srec' -o -name '*.bin' \) \
		-print \
		| xargs rm -f
	rm -f $(OBJS) *.bak tags TAGS
	rm -fr *.*~
	rm -f main main.map main.bin MLO main.elf

distclean:	clobber


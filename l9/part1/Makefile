#####################################################################
#		DEFINE TOPDIR					    #
#####################################################################

TOPDIR = $(shell pwd)
export TOPDIR


CROSS_COMPILE = arm-elf-
export CROSS_COMPILE

include $(TOPDIR)/config.mk

OBJS 	:= arch/*.o kernel/*.o lib/*.o

.PHONY : all OBJS

all: main.bin

main.bin:main.elf
	$(OBJCOPY) -O binary $< $@
	$(OBJDUMP) -D main.elf > main.dis

main.elf:
	$(MAKE) -C arch
	$(MAKE) -C kernel
	$(MAKE) -C lib
	$(LD) $(LDFLAGS) $(OBJS) $(PLATFLAG) -Map main.map -o $@

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


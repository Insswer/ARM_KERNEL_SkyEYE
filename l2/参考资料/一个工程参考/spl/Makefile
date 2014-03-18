
ifndef TOPDIR
TOPDIR_ := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
TOPDIR  := $(shell echo "$(TOPDIR_)")
export TOPDIR
endif

include $(TOPDIR)/config.mk

#############################################################
ifndef CROSS_COMPILE
CROSS_COMPILE = arm-none-linux-gnueabi-
export CROSS_COMPILE
endif
##############################################################
#                   order is important                       #
##############################################################
OBJS = cpu/start.o

LIBS += board/libboard.a
LIBS += cpu/libcpu.a
LIBS += lib/libarm.a
LIBS += drivers/libdriver.a
LIBS += common/libcommon.a
.PHONY : $(LIBS)


#PLATFORM_LDFLAGS += -L $(shell dirname `$(CC) $(CFLAGS) -print-libgcc-file-name`) -lgcc
#PLATFORM_LDFLAGS += -lgcc -L/home/insswer/gccpanda/arm-2012.03/lib/gcc/arm-none-linux-gnueabi/4.6.3
PLATFORM_LDFLAGS += libgcc
##############################################################

ALL = MLO zygote.dis

all : $(ALL)

MLO: zygote.bin
	python mkhead.py zygote.bin 0x40300000

zygote.bin : zygote
	$(OBJCOPY) $(OBJCFLAGS) -O binary $< $@

zygote : $(OBJS) $(LIBS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) $(OBJS) --start-group $(LIBS) --end-group $(PLATFORM_LDFLAGS) -Map zygote.map -o $@

zygote.dis: zygote
	$(OBJDUMP) -D zygote > zygote.dis
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
	rm -f zygote zygote.map $(ALL) zygote.bin MLO
	
distclean:	clobber 

include $(TOPDIR)/config.mk

LIB = libcpu.a

START = start.o
OBJS = cpu.o clocks.o ddr.o emif.o
SOBJS = rom_hal.o

SOURCES = $(START:.o=.s) $(OBJS:.o=.c) $(SOBJS:.o=.s)

DEP = $(START:.o=.d) $(OBJS:.o=.d) $(SOBJS:.o=.d)

all: $(DEP) $(START) $(LIB)

$(LIB) : $(OBJS) $(SOBJS)
	$(AR) crv $@ $(OBJS) $(SOBJS)

sinclude $(DEP)

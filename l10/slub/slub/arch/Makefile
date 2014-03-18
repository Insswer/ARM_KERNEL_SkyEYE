include $(TOPDIR)/config.mk

LIB = libarch.a

START = start.o int_control.o timer.o
OBJS = mmu.o exception.o printf.o
SOBJS = entry.o

SOURCES = $(START:.o=.s) $(OBJS:.o=.c) $(SOBJS:.o=.s)

DEP = $(START:.o=.d) $(OBJS:.o=.d) $(SOBJS:.o=.d)

all: $(DEP) $(START) $(LIB)

$(LIB) : $(OBJS) $(SOBJS)
	$(AR) cr $@ $(OBJS) $(SOBJS)

sinclude $(DEP)

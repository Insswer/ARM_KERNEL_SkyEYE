include $(TOPDIR)/config.mk

LIB	= libkernel.a
INIT_OBJS := page.o
OBJS	:= main.o slab.o task.o
SOBJS   := swi.o

DEP = $(OBJS:.o=.d)

$(LIB) : $(OBJS) $(SOBJS) $(DEP)
	$(AR) cr $@ $^

all : $(INIT_OBJS) $(DEP) $(LIB)

clean :
	rm -f $(SOBJS) $(OBJS)

distclean : clean
	rm -f $(LIB) core *.bak .depend

sinclude $(DEP)


include $(TOPDIR)/config.mk

LIB	= lib.a
OBJS	:= string.o bitops.o

DEP = $(OBJS:.o=.d)

$(LIB) : $(OBJS) $(SOBJS) $(DEP)
	$(AR) cr $@ $^

all : $(INIT_OBJS) $(DEP) $(LIB)

clean :
	rm -f $(SOBJS) $(OBJS)

distclean : clean
	rm -f $(LIB) core *.bak .depend

sinclude $(DEP)

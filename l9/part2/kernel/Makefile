include $(TOPDIR)/config.mk

LIB	= libkernel.a
OBJS	:= main.o
SOBJS   := swi.o

DEP = $(OBJS:.o=.d)

$(LIB) : $(OBJS) $(SOBJS) $(DEP)
	$(AR) cr $@ $^

clean :
	rm -f $(SOBJS) $(OBJS)

distclean : clean
	rm -f $(LIB) core *.bak .depend
	
sinclude $(DEP)


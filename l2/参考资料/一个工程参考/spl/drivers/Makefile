include $(TOPDIR)/config.mk

LIB		= libdriver.a
OBJS	:= ns16550.o led.o

DEP = $(OBJS:.o=.d)

$(LIB) : $(OBJS) $(SOBJS) $(DEP)
	$(AR) crv $@ $^

clean :
	rm -f $(SOBJS) $(OBJS)

distclean : clean
	rm -f $(LIB) core *.bak .depend
	
sinclude $(DEP)

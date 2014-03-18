include $(TOPDIR)/config.mk

LIB	= libarm.a
AOBJS	= _udivsi3.o _umodsi3.o
COBJS 	= io.o printf.o ctype.o div0.o
OBJS	= $(AOBJS) $(COBJS)

DEP = $(ABJS:.o=.d) $(COBJS:.o=.d)


$(LIB) : $(DEP) $(OBJS)
	$(AR) crv $@ $(OBJS)
	
sinclude $(DEP)

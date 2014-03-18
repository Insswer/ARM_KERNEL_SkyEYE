include $(TOPDIR)/config.mk

LIB 	= libcommon.a
AOBJS	=
COBJS	= main.o
OBJS	= $(AOBJS) $(COBJS)

CPPFLAGS += -I..
DEP = $(ABJS:.o=.d) $(COBJS:.o=.d)

all : $(LIB) $(AOBJS)

$(LIB) : $(DEP) $(OBJS)
	$(AR) crv $@ $(OBJS)
	
sinclude $(DEP)

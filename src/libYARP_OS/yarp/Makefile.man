

ifndef  YARP_ROOT
YARP_ROOT = ../../..
endif

include $(YARP_ROOT)/conf/Makefile.tool.template

INCS += -I../include

VPATH += $(EXECDIR)

LIBADD += -L../lib/$(OS) -lYARP_OS

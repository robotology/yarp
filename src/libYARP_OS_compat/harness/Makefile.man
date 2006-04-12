

ifndef  YARP_ROOT
YARP_ROOT = ../../..
endif

include $(YARP_ROOT)/conf/Makefile.tool.template

EXEC_TARGET = harness

INCS += -I../include -I../../libYARP_OS/include

VPATH += $(EXECDIR)


LIBADD += -L../lib/$(OS) -lYARP_OS_compat
LIBADD += -L../../libYARP_OS/lib/$(OS) -lYARP_OS 

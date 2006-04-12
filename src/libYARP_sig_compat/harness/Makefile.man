

ifndef  YARP_ROOT
YARP_ROOT = ../../..
endif

include $(YARP_ROOT)/conf/Makefile.tool.template

EXEC_TARGET = harness

INCS += -I../include -I../../libYARP_OS/include -I../../libYARP_OS_compat/include -I../../libYARP_sig/include -I../../libYARP_sig_compat/include

VPATH += $(EXECDIR)

LIBADD += -L../lib/$(OS) -lYARP_sig
LIBADD += -L../../libYARP_sig/lib/$(OS) -lYARP_sig 
LIBADD += -L../../libYARP_sig_compat/lib/$(OS) -lYARP_sig_compat 
LIBADD += -L../../libYARP_OS/lib/$(OS) -lYARP_OS 
LIBADD += -L../../libYARP_OS_compat/lib/$(OS) -lYARP_OS_compat 


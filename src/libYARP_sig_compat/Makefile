ifndef  YARP_ROOT
YARP_ROOT = ../..
endif

include $(YARP_ROOT)/conf/Makefile.recursive.template

SUBDIRS += harness

userdox:
	mkdir -p doc
	doxygen Doxyfile.part


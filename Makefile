ifndef  YARP_ROOT
YARP_ROOT = .
endif

include $(YARP_ROOT)/conf/Makefile.recursive.template

SUBDIRS += doc src

clean: subclean

subclean:
	rm -f package/*.deb

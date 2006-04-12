
# we do not use the standard YARP makefile, to be as clear as possible
# for users looking at this

# replace this with your OS
SYSDIR = linux

# we need ACE
LIBS = -lACE

# we also need libYARP_OS2
LIBS += -L../lib/$(SYSDIR) -lYARP_OS2

CFLAGS = -I../include

TARGETS = $(patsubst %.cpp, %, $(wildcard *.cpp))

CC = g++

%: %.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

default: $(TARGETS)

clean:
	rm -f $(TARGETS)

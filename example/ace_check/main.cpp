#include <stdio.h>

#include <ace/OS.h>

// ACE can be configured to redefine main.
// we don't want that.
#ifdef main
#undef main
#endif

int main() {
	// just exercise ACE a tiny bit
	char buf[256];
	ACE_OS::sprintf(buf,"hello ACE world!");
	printf("[%s]\n",buf);
	return 0;
}


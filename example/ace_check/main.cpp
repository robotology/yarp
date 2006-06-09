#include <ace/ACE.h>
#include <stdio.h>

int main() {
  ACE::init();
  printf("Starting, stopping ACE\n");
  ACE::fini();
  return 0;
}


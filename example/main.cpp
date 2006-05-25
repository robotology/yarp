#include <stdio.h>

#include <yarp/os/Time.h>

using namespace yarp::os;

int main() {
  printf("Hello...\n");
  Time::delay(1);
  printf("...world\n");
  return 0;
}

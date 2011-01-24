#include <stdio.h>
#include <yarp/os/all.h>

int main() {
  printf("Hello from YARP [dramatic pause]\n");
  yarp::os::Time::delay(5);
  printf("[/dramatic pause]\n");
  return 0;
}

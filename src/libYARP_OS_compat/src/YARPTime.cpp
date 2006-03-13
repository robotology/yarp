
#include <yarp/YARPTime.h>

#include <yarp/os/Time.h>

using namespace yarp::os;

double YARPTime::GetTimeAsSeconds() {
  return Time::now();
}

void YARPTime::DelayInSeconds(double delay_in_seconds) {
  Time::delay(delay_in_seconds);
}


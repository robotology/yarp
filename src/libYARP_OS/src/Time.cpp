
#include <yarp/os/Time.h>

#include <ace/OS.h>

using namespace yarp::os;

/// WARNING: actual precision under WIN32 depends on setting scheduler 
/// by means of MM functions.
///
void Time::delay(double seconds) {
  ACE_Time_Value tv;
  tv.sec (long(seconds));
  tv.usec (long((seconds-int(seconds)) * 1e6));
  ACE_OS::sleep(tv);
}

double Time::now() {
  ACE_Time_Value timev = ACE_OS::gettimeofday ();
  return double(timev.sec()) + timev.usec() * 1e-6; 
}


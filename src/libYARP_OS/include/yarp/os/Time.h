#ifndef _YARP2_TIME_
#define _YARP2_TIME_

namespace yarp {
  namespace os {
    class Time;
  }
}

/**
 * Services related to time -- delay, current time.
 */
class yarp::os::Time {
public:
  /**
   * Wait for a certain number of seconds.  This may be fractional.
   * @param seconds the duration of the delay, in seconds
   */
  static void delay(double seconds);

  /**
   * Return the current time in seconds, relative to an arbitrary 
   * starting point.
   * @return the system time in seconds
   */
  static double now();
};

#endif

#ifndef _YARP2_IOEXCEPTION_
#define _YARP2_IOEXCEPTION_

#include <yarp/String.h>

namespace yarp {
  class IOException;
}

/**
 * General communication error.
 */
class yarp::IOException {
public:
  IOException(const char *txt = NULL) {
    if (txt!=NULL) {
      desc = txt;
    }
  }

  String toString() const {
    return desc;
  }
private:
  String desc;
};

#endif


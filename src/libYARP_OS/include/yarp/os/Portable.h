#ifndef _YARP2_OS_PORTABLE_
#define _YARP2_OS_PORTABLE_

#include <yarp/os/PortReader.h>
#include <yarp/os/PortWriter.h>

namespace yarp {
  namespace os {
    class Portable;
  }
}

/**
 * This is a base class for objects that can be both read from 
 * and be written to the YARP network.  It is a simple union of
 * PortReader and PortWriter.
 */
class yarp::os::Portable : public PortReader, public PortWriter {
};

#endif


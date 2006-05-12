#ifndef _YARP2_PORTREADER_
#define _YARP2_PORTREADER_

#include <yarp/os/ConstString.h>
#include <yarp/os/ConnectionReader.h>

namespace yarp {
  namespace os {
    class PortReader;
  }
}

/**
 * Interface implemented by all objects that can read themselves from
 * the network, such as Bottle objects.
 * @see Port, PortWriter
 */
class yarp::os::PortReader {
public:

  /**
   * Destructor.
   */
  virtual ~PortReader() {}

  /**
   * Read this object from a network connection.
   * Override this for your particular class.
   * @param connection an interface to the network connection for reading
   * @return true iff the object is successfully read
   */
  virtual bool read(ConnectionReader& connection) = 0;

};

#endif

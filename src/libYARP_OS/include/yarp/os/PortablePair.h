#ifndef _YARP2_PORTABLEPAIR_
#define _YARP2_PORTABLEPAIR_

#include <yarp/os/Portable.h>

namespace yarp {
  namespace os {
    template <class HEAD, class BODY> class PortablePair;
  }
}

/**
 * Group a pair of objects to be sent and received together.
 * Handy for adding general-purpose headers, for example.
 */
template <class HEAD, class BODY>
class yarp::os::PortablePair : public Portable {
public:
  /**
   * An object of the first type (HEAD).
   */
  HEAD head;

  /**
   * An object of the second type (BODY).
   */
  BODY body;

  /**
   * Reads this object pair from a network connection.
   * @param connection an interface to the network connection for reading
   * @return true iff the object pair was successfully read
   */
  virtual bool read(ConnectionReader& connection) {
    bool ok = head.read(connection);
    if (ok) {
      ok = body.read(connection);
    }
    return ok;
  }  

  /**
   * Writes this object pair to a network connection.  
   * @param connection an interface to the network connection for writing
   * @return true iff the object pair was successfully written
  */
  virtual bool write(ConnectionWriter& connection) {
    bool ok = head.write(connection);
    if (ok) {
      ok = body.write(connection);
    }
    return ok;
  }

  /**
   * This is called when the port has finished all writing operations.
   * Passes call on to head and body.
   */
  virtual void onCompletion() {
    head.onCompletion();
    body.onCompletion();
  }
};

#endif



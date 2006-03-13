#ifndef _YARP2_TWOWAYSTREAM_
#define _YARP2_TWOWAYSTREAM_

#include <yarp/Address.h>
#include <yarp/InputStream.h>
#include <yarp/OutputStream.h>

namespace yarp {
  class TwoWayStream;
}

/**
 * An object containing an InputStream + OutputStream pair.
 */
class yarp::TwoWayStream {
public:
  virtual ~TwoWayStream() {
  }

  virtual InputStream& getInputStream() = 0; // throws
  virtual OutputStream& getOutputStream() = 0; // throws

  virtual const Address& getLocalAddress() = 0; // throws
  virtual const Address& getRemoteAddress() = 0; // throws

  virtual bool isOk() = 0;

  virtual void reset() = 0;

  virtual void close() = 0; // throws
};

#endif


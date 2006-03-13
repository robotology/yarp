#ifndef _YARP2_FAKEFACE_
#define _YARP2_FAKEFACE_

#include <yarp/Face.h>

namespace yarp {
  class FakeFace;
}

/**
 * A dummy Face for testing purposes.
 */
class yarp::FakeFace : public Face {
public:
  virtual void open(const Address& address);
  virtual void close();
  virtual InputProtocol *read();
  virtual OutputProtocol *write(const Address& address);
private:
};

#endif

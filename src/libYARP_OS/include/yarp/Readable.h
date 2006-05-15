// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_READABLE_
#define _YARP2_READABLE_

#include <yarp/os/PortReader.h>

// this typedef will be removed soon - it used be a separate class

namespace yarp {
    typedef yarp::os::PortReader Readable;
    //class Readable;
}

/**
 * Specification of minimal operations an object must support to
 * be readable from a port.
 */
/*
  class yarp::Readable {
  public:
  virtual ~Readable() { }

  virtual void readBlock(ConnectionReader& reader) = 0;
  };
*/

#endif

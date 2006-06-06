// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_PORTREADERCREATOR_
#define _YARP2_PORTREADERCREATOR_

#include <yarp/os/ConstString.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/PortReader.h>

namespace yarp {
    namespace os {
        class PortReaderCreator;
    }
}

class yarp::os::PortReaderCreator {
public:

    /**
     * Destructor.
     */
    virtual ~PortReaderCreator() {}

    virtual PortReader *create() = 0;

};

#endif

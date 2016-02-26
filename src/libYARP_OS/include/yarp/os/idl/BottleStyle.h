/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_BOTTLESTYLE
#define YARP2_BOTTLESTYLE

#include <yarp/os/idl/WirePortable.h>
#include <yarp/os/idl/WireReader.h>
#include <yarp/os/idl/WireWriter.h>

namespace yarp {
    namespace os {
        namespace idl {
            template <class T> class BottleStyle;
        }
    }
}

template <class T>
class YARP_OS_API yarp::os::idl::BottleStyle : public T {
public:
    virtual bool read(yarp::os::ConnectionReader& reader) {
        return T::readBottle(reader);
    }

    virtual bool write(yarp::os::ConnectionWriter& writer) {
        return T::writeBottle(writer);
    }
};

#endif


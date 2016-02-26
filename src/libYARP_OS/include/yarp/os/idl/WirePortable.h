/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_WIREPORTABLE
#define YARP2_WIREPORTABLE

#include <yarp/os/Portable.h>

namespace yarp {
    namespace os {
        namespace idl {
            class WireReader;
            class WireWriter;
            class WirePortable;
        }
    }
}

/**
 *
 * A "tamed" Portable, that promises to serialize itself in an IDL-friendly
 * way.
 *
 */
class YARP_OS_API yarp::os::idl::WirePortable : public yarp::os::Portable {
public:
    using yarp::os::Portable::read;
    using yarp::os::Portable::write;

    virtual bool read(yarp::os::idl::WireReader& reader);

    virtual bool write(yarp::os::idl::WireWriter& writer);

    virtual bool readBare(yarp::os::ConnectionReader& reader) {
        return read(reader);
    }

    virtual bool writeBare(yarp::os::ConnectionWriter& writer) {
        return write(writer);
    }

    virtual bool readBottle(yarp::os::ConnectionReader& reader) {
        return read(reader);
    }

    virtual bool writeBottle(yarp::os::ConnectionWriter& writer) {
        return write(writer);
    }
};

#endif


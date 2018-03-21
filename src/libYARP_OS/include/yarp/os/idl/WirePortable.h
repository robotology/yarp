/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IDL_WIREPORTABLE_H
#define YARP_OS_IDL_WIREPORTABLE_H

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

#endif // YARP_OS_IDL_WIREPORTABLE_H

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_PORTABLE_H
#define YARP_OS_PORTABLE_H

#include <yarp/os/PortReader.h>
#include <yarp/os/PortWriter.h>

namespace yarp {
    namespace os {
        class Portable;
    }
}

/**
 * \ingroup comm_class
 *
 * This is a base class for objects that can be both read from
 * and be written to the YARP network.  It is a simple union of
 * PortReader and PortWriter.
 */
class YARP_OS_API yarp::os::Portable : public PortReader, public PortWriter
{
public:
    // reiterate the key inherited virtual methods, just as a reminder

    virtual bool read(ConnectionReader& reader) YARP_OVERRIDE = 0;
    virtual bool write(ConnectionWriter& writer) YARP_OVERRIDE = 0;

    virtual Type getType() { return getReadType(); }

    /**
     * Copy one portable to another, via writing and reading.
     *
     * @return true iff writer.write and reader.read both succeeded.
     */
    static bool copyPortable(PortWriter& writer, PortReader& reader);
};

#endif // YARP_OS_PORTABLE_H

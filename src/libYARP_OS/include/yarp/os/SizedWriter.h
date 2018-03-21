/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_SIZEDWRITER_H
#define YARP_OS_SIZEDWRITER_H

#include <yarp/conf/numeric.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/PortWriter.h>

namespace yarp {
    namespace os {
        class SizedWriter;
    }
}

/**
 * Minimal requirements for an efficient Writer.
 * Some protocols require knowing the size of a message up front.
 * In general, that requires generating the message before sending
 * it, but a user could do something more clever. The
 * SizedWriter class is referenced by the library instead of
 * BufferedConnectionWriter specifically to leave that possibility open.
 */
class YARP_OS_API yarp::os::SizedWriter : public PortWriter {
public:
    virtual ~SizedWriter() {}

    virtual size_t length() = 0;

    virtual size_t headerLength() = 0;

    virtual size_t length(size_t index) = 0;

    virtual const char *data(size_t index) = 0;

    virtual PortReader *getReplyHandler() = 0;

    virtual Portable *getReference() = 0;

    virtual void write(OutputStream& os) {
        for (size_t i=0; i<length(); i++) {
            Bytes b((char*)data(i), length(i));
            os.write(b);
        }
    }

    virtual bool write(ConnectionWriter& connection) override {
        for (size_t i=0; i<length(); i++) {
            connection.appendBlock((char*)data(i), length(i));
        }
        return true;
    }

    virtual bool dropRequested() = 0;


    /**
     *
     * Call when writing is about to begin.
     *
     */
    virtual void startWrite() = 0;

    /**
     *
     * Call when all writing is finished.
     *
     */
    virtual void stopWrite() = 0;

    virtual void clear() {}
};

#endif // YARP_OS_SIZEDWRITER_H

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_SIZEDWRITER_
#define _YARP2_SIZEDWRITER_

#include <yarp/os/impl/OutputStream.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/PortWriter.h>
#include <stddef.h> // defines size_t

namespace yarp {
    namespace os {
        namespace impl {
            class SizedWriter;
            using yarp::os::PortReader;
        }
    }
    using yarp::os::PortReader;
}

/**
 * Minimal requirements for an efficient Writer.
 * Some protocols require knowing the size of a message up front.
 * In general, that requires generating the message before sending
 * it, but a user could do something more clever. The
 * SizedWriter class is referenced by the library instead of
 * BufferedConnectionWriter specifically to leave that possibility open.
 */
class YARP_OS_impl_API yarp::os::impl::SizedWriter : public yarp::os::PortWriter {
public:
    virtual ~SizedWriter() {}

    virtual size_t length() = 0;

    virtual size_t headerLength() = 0;

    virtual size_t length(size_t index) = 0;

    virtual const char *data(size_t index) = 0;

    virtual PortReader *getReplyHandler() = 0;
    
    virtual yarp::os::Portable *getReference() = 0;

    virtual void write(OutputStream& os) {
        for (size_t i=0; i<length(); i++) {
            Bytes b((char*)data(i),length(i));
            os.write(b);
        }
    }

    virtual bool write(ConnectionWriter& connection) {
        for (size_t i=0; i<length(); i++) {
            connection.appendBlock((char*)data(i),length(i));
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
};

#endif

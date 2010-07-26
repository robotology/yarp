// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_SIZEDWRITER_
#define _YARP2_SIZEDWRITER_

#include <yarp/os/impl/OutputStream.h>
#include <yarp/os/PortReader.h>

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
class yarp::os::impl::SizedWriter {
public:
    virtual ~SizedWriter() {}

    virtual void write(OutputStream& os) = 0;

    virtual int length() = 0;

    virtual int headerLength() = 0;

    virtual int length(int index) = 0;

    virtual const char *data(int index) = 0;

    virtual PortReader *getReplyHandler() = 0;
    
    virtual yarp::os::Portable *getReference() = 0;
};

#endif


// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_BINPORTABLE_
#define _YARP2_BINPORTABLE_

#include <yarp/os/Portable.h>

namespace yarp {
    namespace os {
        template <class T> class BinPortable;
    }
}

/**
 * \ingroup comm_class
 *
 * \brief Class for writing and reading simple binary structures.
 * Can be used in conjunction with the Port class to send data
 * across the network.
 * Don't use this for anything containing a pointer,
 * or which needs to be portable across different compilers,
 * languages, operating systems, or processor architectures.
 */
template <class T>
class yarp::os::BinPortable : public Portable {
private:
    T t;

public:

    /**
     * Get the internal structure that will be read or written.
     * @return the internal structure that will be read or written.
     */
    T& content() {
        return t;
    }


    virtual bool read(ConnectionReader& connection) {
        // An exception will get thrown upon error.
        // Pending: translate this in expectBlock to a return value.
        connection.convertTextMode(); // if connection is text-mode, convert!
        connection.expectBlock((char*)(&t),sizeof(T));
        return true;
    }

    virtual bool write(ConnectionWriter& connection) {
        connection.appendBlock((char*)(&t),sizeof(T));
        connection.convertTextMode(); // if connection is text-mode, convert!
        return true;
    }
};

#endif

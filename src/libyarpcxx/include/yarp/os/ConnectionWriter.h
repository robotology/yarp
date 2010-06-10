// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef YARPCXX_ConnectionWriter_INC
#define YARPCXX_ConnectionWriter_INC

#include <yarp/yarpcxx.h>

namespace yarpcxx {
    namespace os {
        class ConnectionWriter;
    }
}

class yarpcxx::os::ConnectionWriter {
public:
    ConnectionWriter(yarpWriterPtr handle) : impl(handle) {}

    bool appendInt(int x) {
        return yarpWriterAppendInt(impl,x)==0;
    }

    yarpWriterPtr getHandle() { return impl; }
private:
    yarpWriterPtr impl;
};

#endif

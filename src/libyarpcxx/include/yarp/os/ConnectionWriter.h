/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

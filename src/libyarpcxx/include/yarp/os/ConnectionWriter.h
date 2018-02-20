/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPCXX_ConnectionReader_INC
#define YARPCXX_ConnectionReader_INC

#include <yarp/yarpcxx.h>

#include <yarp/os/ConstString.h>

namespace yarpcxx {
    namespace os {
        class ConnectionReader;
    }
}

class yarpcxx::os::ConnectionReader {
public:
    ConnectionReader(yarpReaderPtr handle) : impl(handle) {}

    bool expectBlock(const char *data, int len) {
        return !yarpReaderExpectBlock(impl,data,len);
    }

    ConstString expectText(int terminatingChar = '\n') {
        ConstString result;
        yarpReaderExpectText(impl,result.getHandle(),terminatingChar);
        return result;
    }

    int expectInt() {
        int x = 0;
        yarpReaderExpectInt(impl,&x);
        return x;
    }

    double expectDouble() {
        double x = 0;
        yarpReaderExpectDouble(impl,&x);
        return x;
    }

    bool isTextMode() {
        return (bool)yarpReaderIsTextMode(impl);
    }

    yarpReaderPtr getHandle() { return impl; }
private:
    yarpReaderPtr impl;
};

#endif

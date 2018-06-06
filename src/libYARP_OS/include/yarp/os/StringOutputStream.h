/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_STRINGOUTPUTSTREAM_H
#define YARP_OS_STRINGOUTPUTSTREAM_H

#include <yarp/os/OutputStream.h>
#include <yarp/os/Bytes.h>
#include <string>

namespace yarp {
    namespace os {
        class StringOutputStream;
    }
}

/**
 * An OutputStream that produces a string.  Handy for testing purposes.
 */
class yarp::os::StringOutputStream : public OutputStream {
public:
    using OutputStream::write;

    StringOutputStream() { }

    std::string toString() {
        return data;
    }

    void reset() {
        data = "";
    }

    virtual void write(const Bytes& b) override {
        std::string tmp((char*)b.get(), b.length());
        data += tmp;
    }

    virtual void close() override {
    }

    virtual bool isOk() const override {
        return true;
    }

    const std::string& str() const { return data; }

private:
    std::string data;
};

#endif // YARP_OS_STRINGOUTPUTSTREAM_H

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_STRINGOUTPUTSTREAM_H
#define YARP_OS_STRINGOUTPUTSTREAM_H

#include <yarp/os/Bytes.h>
#include <yarp/os/OutputStream.h>

#include <string>

namespace yarp {
namespace os {

/**
 * An OutputStream that produces a string.  Handy for testing purposes.
 */
class StringOutputStream : public OutputStream
{
public:
    using OutputStream::write;

    StringOutputStream()
    {
    }

    std::string toString() const
    {
        return data;
    }

    void reset()
    {
        data = "";
    }

    void write(const Bytes& b) override
    {
        std::string tmp((char*)b.get(), b.length());
        data += tmp;
    }

    void close() override
    {
    }

    bool isOk() const override
    {
        return true;
    }

    const std::string& str() const
    {
        return data;
    }

private:
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) data;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_STRINGOUTPUTSTREAM_H

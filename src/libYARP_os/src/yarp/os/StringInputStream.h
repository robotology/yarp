/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_STRINGINPUTSTREAM_H
#define YARP_OS_STRINGINPUTSTREAM_H

#include <yarp/os/Bytes.h>
#include <yarp/os/InputStream.h>

#include <string>

namespace yarp {
namespace os {

/**
 * An InputStream that reads from a string.  Handy For testing purposes.
 */
class StringInputStream : public InputStream
{
public:
    using InputStream::read;

    StringInputStream()
    {
        at = 0;
        data = "";
    }

    void reset()
    {
        at = 0;
        data = "";
    }

    void reset(const std::string& str)
    {
        at = 0;
        data = str;
    }

    void add(const std::string& txt)
    {
        data += txt;
    }

    void add(const Bytes& b)
    {
        for (size_t i = 0; i < b.length(); i++) {
            data += b.get()[i];
        }
    }

    yarp::conf::ssize_t read(Bytes& b) override
    {
        char* base = b.get();
        size_t space = b.length();
        yarp::conf::ssize_t ct = 0;
        for (size_t i = 0; i < space; i++) {
            if (at < data.length()) {
                base[i] = data[at];
                at++;
                ct++;
            }
        }
        return ct;
    }

    void close() override
    {
    }

    virtual std::string toString() const
    {
        return data;
    }

    bool isOk() const override
    {
        return true;
    }

private:
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) data;
    unsigned int at;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_STRINGINPUTSTREAM_H

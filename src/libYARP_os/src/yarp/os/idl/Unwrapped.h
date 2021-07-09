/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IDL_UNWRAPPED_H
#define YARP_OS_IDL_UNWRAPPED_H

#include <yarp/os/idl/WirePortable.h>
#include <yarp/os/idl/WireReader.h>
#include <yarp/os/idl/WireWriter.h>

namespace yarp {
namespace os {
namespace idl {

template <class T>
class Unwrapped : public yarp::os::Portable
{
public:
    T content;

    bool read(yarp::os::ConnectionReader& reader) override
    {
        WireReader wreader(reader);
        return content.read(wreader);
    }

    bool write(yarp::os::ConnectionWriter& writer) const override
    {
        WireWriter wwriter(writer);
        return content.write(wwriter);
    }
};

template <class T>
class UnwrappedView : public yarp::os::Portable
{
public:
    T& content;

    UnwrappedView(T& content) : content(content) {}

    bool read(yarp::os::ConnectionReader& reader) override
    {
        WireReader wreader(reader);
        return content.read(wreader);
    }

    bool write(yarp::os::ConnectionWriter& writer) const override
    {
        WireWriter wwriter(writer);
        return content.write(wwriter);
    }
};

} // namespace idl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IDL_UNWRAPPED_H

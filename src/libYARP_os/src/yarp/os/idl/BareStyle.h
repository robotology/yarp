/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IDL_BARESTYLE_H
#define YARP_OS_IDL_BARESTYLE_H

#include <yarp/os/idl/WirePortable.h>
#include <yarp/os/idl/WireReader.h>
#include <yarp/os/idl/WireWriter.h>

namespace yarp {
namespace os {
namespace idl {

template <class T>
class BareStyle : public T
{
public:
    bool read(yarp::os::ConnectionReader& reader) override
    {
        return T::readBare(reader);
    }

    bool write(yarp::os::ConnectionWriter& writer) const override
    {
        return T::writeBare(writer);
    }
};

} // namespace idl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IDL_BARESTYLE_H

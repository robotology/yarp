/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_PORTCOMMAND_H
#define YARP_OS_IMPL_PORTCOMMAND_H

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Portable.h>

#include <string>

namespace yarp {
namespace os {
namespace impl {

/**
 * Simple Readable and Writable object representing a command to a YARP port.
 */
class YARP_os_impl_API PortCommand :
        public yarp::os::Portable
{
public:
    PortCommand() :
            header(8)
    {
        ch = '\0';
        str = "";
    }

    PortCommand(char ch, const std::string& str) :
            header(8)
    {
        this->ch = ch;
        this->str = str;
    }

    bool write(yarp::os::ConnectionWriter& writer) const override;
    bool read(yarp::os::ConnectionReader& reader) override;

    char getKey()
    {
        return ch;
    }

    std::string getText()
    {
        return str;
    }

public:
    char ch;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) str;
    mutable yarp::os::ManagedBytes header;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PORTCOMMAND_H

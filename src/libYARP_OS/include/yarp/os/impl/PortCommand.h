/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_PORTCOMMAND_H
#define YARP_OS_IMPL_PORTCOMMAND_H

#include <yarp/os/Portable.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/ManagedBytes.h>
#include <string>

namespace yarp {
    namespace os {
        namespace impl {
            class PortCommand;
        }
    }
}

/**
 * Simple Readable and Writable object representing a command to a YARP port.
 */
class YARP_OS_impl_API yarp::os::impl::PortCommand : public yarp::os::Portable
{
public:

    PortCommand() : header(8)
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

    virtual bool write(yarp::os::ConnectionWriter& writer) override;
    virtual bool read(yarp::os::ConnectionReader& reader) override;

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
    std::string str;
    yarp::os::ManagedBytes header;
};

#endif // YARP_OS_IMPL_PORTCOMMAND_H

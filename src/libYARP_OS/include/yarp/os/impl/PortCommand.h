/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_PORTCOMMAND
#define YARP2_PORTCOMMAND

#include <yarp/os/Portable.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/ConstString.h>

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
class YARP_OS_impl_API yarp::os::impl::PortCommand : public yarp::os::Portable {
public:

    PortCommand() : header(8) {
        ch = '\0';
        str = "";
    }

    PortCommand(char ch, const ConstString& str) : header(8) {
        this->ch = ch;
        this->str = str;
    }

    virtual bool write(yarp::os::ConnectionWriter& writer);
    virtual bool read(yarp::os::ConnectionReader& reader);

    char getKey() {
        return ch;
    }

    ConstString getText() {
        return str;
    }

public:
    char ch;
    ConstString str;
    yarp::os::ManagedBytes header;
};

#endif

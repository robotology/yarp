// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_PORTCOMMAND_
#define _YARP2_PORTCOMMAND_

#include <yarp/os/Portable.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/ManagedBytes.h>
#include <yarp/String.h>

namespace yarp {
    class PortCommand;
}

/**
 * Simple Readable and Writable object representing a command to a YARP port.
 */
class yarp::PortCommand : public yarp::os::Portable {
public:

    PortCommand() : header(8) {
        ch = '\0';
        str = "";
    }

    PortCommand(char ch, const String& str) : header(8) {
        this->ch = ch;
        this->str = str;
    }

    virtual void readBlock(ConnectionReader& reader);

    virtual void writeBlock(ConnectionWriter& writer);

    virtual bool write(yarp::os::ConnectionWriter& writer) {
        writeBlock(writer);
        return true;
    }

    virtual bool read(yarp::os::ConnectionReader& reader) {
        readBlock(reader);
        return true;
    }

    char getKey() {
        return ch;
    }

    String getText() {
        return str;
    }

public:
    char ch;
    String str;
    ManagedBytes header;
};

#endif

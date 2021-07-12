/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/PortCommand.h>

#include <yarp/os/NetType.h>
#include <yarp/os/impl/LogComponent.h>

using namespace yarp::os::impl;
using namespace yarp::os;


namespace {
YARP_OS_LOG_COMPONENT(PORTCOMMAND, "yarp.os.impl.PortCommand")
} // namespace

bool PortCommand::read(ConnectionReader& reader)
{
    yCTrace(PORTCOMMAND, "PortCommand::readBlock");
    ch = '\0';
    str = "";
    if (!reader.isTextMode()) {
        bool ok = reader.expectBlock(header.get(), header.length());
        if (!ok) {
            return false;
        }
        char* base = header.get();
        if (base[4] == '~') {
            ch = base[5];
            if (ch == '\0') {
                //str = reader.expectString(reader.getSize());
                str = reader.expectText('\0');
                if (reader.isError()) {
                    return false;
                }
                if (str.length() > 0) {
                    ch = str[0];
                }
            }
        } else {
            return false;
        }
    } else {
        str = reader.expectText();
        if (reader.isError()) {
            return false;
        }
        if (str.length() > 0) {
            ch = str[0];
        }
    }
    return true;
}

bool PortCommand::write(ConnectionWriter& writer) const
{
    yCTrace(PORTCOMMAND, "PortCommand::writeBlock");
    yCDebug(PORTCOMMAND, "Writing port command, text mode [%s]\n", writer.isTextMode() ? "true" : "false");
    if (!writer.isTextMode()) {
        int len = 0;
        if (ch == '\0') {
            len = (int)str.length() + 1;
        }
        yCAssert(PORTCOMMAND, header.length() == 8);
        char* base = header.get();
        Bytes b(base, 4);
        NetType::netInt(len, b);
        base[4] = '~';
        base[5] = ch;
        base[6] = 0;
        base[7] = 1;
        writer.appendBlock(header.bytes().get(), header.bytes().length());
        if (ch == '\0') {
            writer.appendBlock(str.c_str(), str.length() + 1);
        }
    } else {
        if (ch != '\0') {
            char buf[] = "X";
            buf[0] = ch;
            writer.appendText(buf);
        } else {
            writer.appendText(str);
        }
    }
    return !writer.isError();
}

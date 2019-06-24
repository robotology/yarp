/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/PortCommand.h>

#include <yarp/os/NetType.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os::impl;
using namespace yarp::os;

bool PortCommand::read(ConnectionReader& reader)
{
    //ACE_DEBUG((LM_DEBUG, "PortCommand::readBlock"));
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
    //ACE_DEBUG((LM_DEBUG, "PortCommand::writeBlock"));
    //printf("Writing port command, text mode %d\n", writer.isTextMode());
    if (!writer.isTextMode()) {
        int len = 0;
        if (ch == '\0') {
            len = (int)str.length() + 1;
        }
        yAssert(header.length() == 8);
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

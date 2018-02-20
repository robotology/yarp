/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/StreamConnectionReader.h>
#include <yarp/os/NetType.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/Protocol.h>

#include <yarp/os/Bottle.h>

using namespace yarp::os::impl;
using namespace yarp::os;

yarp::os::ConnectionWriter *StreamConnectionReader::getWriter() {
    if (str==nullptr) {
        return nullptr;
    }
    if (writer==nullptr) {
        writer = new BufferedConnectionWriter(isTextMode(), isBareMode());
        yAssert(writer!=nullptr);
    }
    writer->clear();
    writePending = true;
    if (protocol!=nullptr) {
        protocol->willReply();
    }
    return writer;
}


void StreamConnectionReader::flushWriter() {
    if (writer!=nullptr) {
        if (writePending) {
            if (str!=nullptr) {
                if (protocol!=nullptr) {
                    protocol->reply(*writer);
                } else {
                    writer->write(str->getOutputStream());
                }
                writer->clear();
            }
        }
    }
    writePending = false;
}


StreamConnectionReader::~StreamConnectionReader() {
    if (writer!=nullptr) {
        delete writer;
        writer = nullptr;
    }
}


bool StreamConnectionReader::convertTextMode() {
    if (isTextMode()) {
        if (!convertedTextMode) {
            Bottle bot;
            bot.read(*this);
            BufferedConnectionWriter writer;
            bot.write(writer);
            ConstString s = writer.toString();
            altStream.reset(s);
            in = &altStream;
            convertedTextMode = true;
        }
    }

    return true;
}


Bytes StreamConnectionReader::readEnvelope() {
    if (protocol != nullptr) {
        const ConstString& env = protocol->getEnvelope();
        return Bytes((char*)env.c_str(), env.length());
    }
    if (parentConnectionReader != nullptr) {
        return parentConnectionReader->readEnvelope();
    }
    return Bytes(nullptr, 0);
}



Searchable& StreamConnectionReader::getConnectionModifiers() {
    if (config.size()==0) {
        if (protocol) {
            config.fromString(protocol->getSenderSpecifier().c_str());
        }
    }
    return config;
}

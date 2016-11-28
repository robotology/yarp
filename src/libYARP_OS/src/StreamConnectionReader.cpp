/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
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
    if (str==YARP_NULLPTR) {
        return YARP_NULLPTR;
    }
    if (writer==YARP_NULLPTR) {
        writer = new BufferedConnectionWriter(isTextMode(),isBareMode());
        yAssert(writer!=YARP_NULLPTR);
    }
    writer->clear();
    writePending = true;
    if (protocol!=YARP_NULLPTR) {
        protocol->willReply();
    }
    return writer;
}


void StreamConnectionReader::flushWriter() {
    if (writer!=YARP_NULLPTR) {
        if (writePending) {
            if (str!=YARP_NULLPTR) {
                if (protocol!=YARP_NULLPTR) {
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
    if (writer!=YARP_NULLPTR) {
        delete writer;
        writer = YARP_NULLPTR;
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
    if (protocol != YARP_NULLPTR) {
        const ConstString& env = protocol->getEnvelope();
        return Bytes((char*)env.c_str(),env.length());
    }
    if (parentConnectionReader != YARP_NULLPTR) {
        return parentConnectionReader->readEnvelope();
    }
    return Bytes(0,0);
}



Searchable& StreamConnectionReader::getConnectionModifiers() {
    if (config.size()==0) {
        if (protocol) {
            config.fromString(protocol->getSenderSpecifier().c_str());
        }
    }
    return config;
}

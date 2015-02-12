// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

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
    if (str==NULL) {
        return NULL;
    }
    if (writer==NULL) {
        writer = new BufferedConnectionWriter(isTextMode(),isBareMode());
        yAssert(writer!=NULL);
    }
    writer->clear();
    writePending = true;
    if (protocol!=NULL) {
        protocol->willReply();
    }
    return writer;
}


void StreamConnectionReader::flushWriter() {
    if (writer!=NULL) {
        if (writePending) {
            if (str!=NULL) {
                if (protocol!=NULL) {
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
    if (writer!=NULL) {
        delete writer;
        writer = NULL;
    }
}


bool StreamConnectionReader::convertTextMode() {
    if (isTextMode()) {
        if (!convertedTextMode) {
            Bottle bot;
            bot.read(*this);
            BufferedConnectionWriter writer;
            bot.write(writer);
            String s = writer.toString();
            altStream.reset(s);
            in = &altStream;
            convertedTextMode = true;
        }
    }

    return true;
}


Bytes StreamConnectionReader::readEnvelope() {
    if (protocol==NULL) {
        return Bytes(0,0);
    }
    const String& env = protocol->getEnvelope();
    return Bytes((char*)env.c_str(),env.length());
}



Searchable& StreamConnectionReader::getConnectionModifiers() {
    if (config.size()==0) {
        if (protocol) {
            config.fromString(protocol->getSenderSpecifier().c_str());
        }
    }
    return config;
}

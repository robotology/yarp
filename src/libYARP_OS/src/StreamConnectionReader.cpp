// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/BufferedConnectionWriter.h>
#include <yarp/StreamConnectionReader.h>
#include <yarp/NetType.h>
#include <yarp/BufferedConnectionWriter.h>

#include <yarp/os/Bottle.h>

using namespace yarp;
using namespace yarp::os;

yarp::os::ConnectionWriter *StreamConnectionReader::getWriter() {
    if (str==NULL) {
        return NULL;
    }
    if (writer==NULL) {
        writer = new BufferedConnectionWriter;
        YARP_ASSERT(writer!=NULL);
        writer->reset(isTextMode());
    }
    writer->clear();
    return writer;
}


void StreamConnectionReader::flushWriter() {
    if (writer!=NULL) {
        if (str!=NULL) {
            writer->write(str->getOutputStream());
            writer->clear();
        }
    }
}


StreamConnectionReader::~StreamConnectionReader() {
    if (writer!=NULL) {
        delete writer;
        writer = NULL;
    }
}


bool StreamConnectionReader::convertTextMode() {
    Bottle bot;
    if (isTextMode()) {
        bot.read(*this);
        BufferedConnectionWriter writer;
        bot.write(writer);
        String s = writer.toString();
        altStream.reset(s);
        in = &altStream;
    }

    return true;
}



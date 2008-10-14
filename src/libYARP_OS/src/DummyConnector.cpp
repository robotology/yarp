// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Freyr Magnusson and Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/DummyConnector.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/StreamConnectionReader.h>
#include <yarp/os/impl/StringInputStream.h>


using namespace yarp::os::impl;
using namespace yarp::os;

class DummyConnectorHelper {
private:
    BufferedConnectionWriter writer;
    StreamConnectionReader reader;
    StringInputStream sis;
    bool textMode;
public:

    DummyConnectorHelper() : writer(false), textMode(false) {
    }

    void setTextMode(bool textmode)
    {
        textMode = textmode;
        writer.reset(textMode);
    }


    ConnectionWriter& getCleanWriter()
    {
        writer.reset(textMode);
        return writer;
    }

    ConnectionWriter& getWriter()
    {
        return writer;
    }

    ConnectionReader& getReader()
    {
        String s = writer.toString();
        sis.reset();
        sis.add(s);
        Route r;
        reader.reset(sis, NULL, r, s.length(), textMode);
        return reader;
    }

    void reset()
    {
        writer.reset(textMode);
    }

};


#define HELPER(implementation) (*((DummyConnectorHelper*)implementation))


DummyConnector::DummyConnector() {
    implementation = new DummyConnectorHelper();
}


DummyConnector::~DummyConnector() {
    if (implementation!=NULL) {
        delete &HELPER(implementation);
        implementation = NULL;
    }
}

void DummyConnector::setTextMode(bool textmode) {
    HELPER(implementation).setTextMode(textmode);
}


ConnectionWriter& DummyConnector::getCleanWriter() {
    return HELPER(implementation).getCleanWriter();
}

ConnectionWriter& DummyConnector::getWriter() {
    return HELPER(implementation).getWriter();
}

ConnectionReader& DummyConnector::getReader() {
    return HELPER(implementation).getReader();
}

void DummyConnector::reset() {
    HELPER(implementation).reset();
}

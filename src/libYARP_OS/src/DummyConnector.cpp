// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Freyr Magnusson and RobotCub Consortium
 * Authors: Freyr Magnusson and Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/DummyConnector.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/StreamConnectionReader.h>
#include <yarp/os/StringInputStream.h>


using namespace yarp::os::impl;
using namespace yarp::os;

class DummyConnectorReader : public StreamConnectionReader {
public:
    BufferedConnectionWriter *altWriter;
    bool tmode;
    
    BufferedConnectionWriter *getWriter() {
        altWriter->reset(tmode);
        return altWriter;
    }
};

class DummyConnectorHelper {
private:
    BufferedConnectionWriter writer;
    DummyConnectorReader reader;
    StringInputStream sis;
    bool textMode;
public:

    DummyConnectorHelper() : writer(false), textMode(false) {
        reader.altWriter = &writer;
        reader.tmode = textMode;
    }

    void setTextMode(bool textmode)
    {
        textMode = textmode;
        writer.reset(textMode);
        reader.tmode = textMode;
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
        writer.stopWrite();
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


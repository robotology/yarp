/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * Copyright (C) 2007 Freyr Magnusson
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

    BufferedConnectionWriter *getWriter() override {
        altWriter->reset(tmode);
        return altWriter;
    }
};

class DummyConnectorHelper {
private:
    BufferedConnectionWriter writer;
    DummyConnectorReader reader;
    StringInputStream sis;
    bool textMode{false};
public:

    DummyConnectorHelper() : writer(false) {
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
        std::string s = writer.toString();
        sis.reset();
        sis.add(s);
        Route r;
        reader.reset(sis, nullptr, r, s.length(), textMode);
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
    if (implementation != nullptr) {
        delete &HELPER(implementation);
        implementation = nullptr;
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

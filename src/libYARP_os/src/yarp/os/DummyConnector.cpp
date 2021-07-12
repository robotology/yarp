/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2007 Freyr Magnusson
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/DummyConnector.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/StreamConnectionReader.h>


using namespace yarp::os::impl;
using namespace yarp::os;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
class DummyConnectorReader :
        public StreamConnectionReader
{
public:
    ConnectionWriter* altWriter;
    BufferedConnectionWriter* bufWriter;
    bool tmode;

    ConnectionWriter* getWriter() override
    {
        if (bufWriter) {
            bufWriter->reset(tmode);
        }
        return altWriter;
    }
};

class DummyConnector::Private
{
private:
    BufferedConnectionWriter writer{false};
    DummyConnectorReader reader;
    StringInputStream sis;
    bool textMode{false};

public:
    Private()
    {
        reader.altWriter = &writer;
        reader.bufWriter = dynamic_cast<BufferedConnectionWriter*>(reader.altWriter);
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

    ConnectionReader& getReader(ConnectionWriter* replyWriter)
    {
        writer.stopWrite();
        std::string s = writer.toString();
        sis.reset();
        sis.add(s);
        Route r;
        reader.reset(sis, nullptr, r, s.length(), textMode);
        reader.altWriter = ((replyWriter != nullptr) ? replyWriter : &writer);
        reader.bufWriter = dynamic_cast<BufferedConnectionWriter*>(reader.altWriter);
        return reader;
    }

    void reset()
    {
        writer.reset(textMode);
    }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

DummyConnector::DummyConnector() :
        mPriv(new Private())
{
}

DummyConnector::~DummyConnector()
{
    delete mPriv;
}

void DummyConnector::setTextMode(bool textmode)
{
    mPriv->setTextMode(textmode);
}

ConnectionWriter& DummyConnector::getCleanWriter()
{
    return mPriv->getCleanWriter();
}

ConnectionWriter& DummyConnector::getWriter()
{
    return mPriv->getWriter();
}

ConnectionReader& DummyConnector::getReader(ConnectionWriter* replywriter)
{
    return mPriv->getReader(replywriter);
}

void DummyConnector::reset()
{
    mPriv->reset();
}

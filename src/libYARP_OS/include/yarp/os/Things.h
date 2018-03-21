/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_THINGS_H
#define YARP_OS_THINGS_H

#include <yarp/os/Portable.h>
#include <yarp/os/ConnectionReader.h>

namespace yarp {
    namespace os {
        class Things;
    }
}

/**
 *
 * Base class for generic things.
 *
 */
class YARP_OS_API yarp::os::Things {
public:

    Things();

    virtual ~Things();

    /**
     * Set the reference to a PortWriter object
     *
     */
    void setPortWriter(yarp::os::PortWriter* writer) {
        yarp::os::Things::writer = writer;
    }

    yarp::os::PortWriter* getPortWriter(void) {
        return writer;
    }

    /**
     * Set the reference to a PortReader object
     *
     */
    void setPortReader(yarp::os::PortReader* reader) {
        yarp::os::Things::reader = reader;
    }

    yarp::os::PortReader* getPortReader(void) {
        return reader;
    }

    /**
     *  set a reference to a ConnectionReader
     */
    bool setConnectionReader(yarp::os::ConnectionReader& reader) {
        conReader = &reader;
        if (portable)
            delete portable;
        portable = nullptr;
        return true;
    }

    /*
     * Things writer
     */
    bool write(yarp::os::ConnectionWriter& connection) {
        if (writer)
            return writer->write(connection);
        if (portable)
            return portable->write(connection);
        return false;
    }

    void reset() {
        if (portable)
            delete portable;
        conReader = nullptr;
        writer = nullptr;
        reader = nullptr;
        portable = nullptr;
        beenRead = false;
    }

    template<typename T>
    T* cast_as(void)
    {
        if (this->writer)
            return dynamic_cast<T*>(this->writer);

        if (this->reader)
            return dynamic_cast<T*>(this->reader);

        if (!this->portable)
        {
            if (!this->conReader)
                return nullptr;
            this->portable = new T();
            if (!this->portable->read(*this->conReader))
            {
                delete this->portable;
                this->portable = nullptr;
                return nullptr;
            }
            beenRead = true;
        }
        return dynamic_cast<T*>(this->portable);
    }

    bool hasBeenRead() {
        return beenRead;
    }

private:
    bool beenRead;
    yarp::os::ConnectionReader* conReader;
    yarp::os::PortWriter* writer;
    yarp::os::PortReader* reader;
    yarp::os::Portable* portable;

};

#endif // YARP_OS_THINGS_H

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan and Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
        if(portable)
            delete portable;
        portable = NULL;
        return true;
    }

    /*
     * Things writer
     */
    bool write(yarp::os::ConnectionWriter& connection) {
        if(writer)
            return writer->write(connection);
        if(portable)
            return portable->write(connection);
        return false;
    }

    void reset() {
        if(portable)
            delete portable;
        conReader = NULL;
        writer = NULL;
        reader = NULL;
        portable = NULL;
        beenRead = false;
    }

    template<typename T>
    T* cast_as(void)
    {
        if(this->writer)
            return dynamic_cast<T*>(this->writer);

        if(this->reader)
            return dynamic_cast<T*>(this->reader);

        if(!this->portable)
        {
            if(!this->conReader)
                return NULL;
            this->portable = new T();
            if(!this->portable->read(*this->conReader))
            {
                delete this->portable;
                this->portable = NULL;
                return NULL;
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

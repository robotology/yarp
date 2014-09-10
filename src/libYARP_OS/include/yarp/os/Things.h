// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan and Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_THINGS_
#define _YARP2_THINGS_

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
        portable = NULL;
    }

    template<typename T>
    T* cast_as(void)
    {
        if(this->writer)
            return dynamic_cast<T*>(this->writer);

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
        }
        return dynamic_cast<T*>(this->portable);
    }



/**
 * TODO: these should be private indeed! 
 *       they are defined as public member to allow 
 *       accessing them in swig file via 'self->' 
 */
public: 
    yarp::os::ConnectionReader* conReader;
    yarp::os::PortWriter* writer;
    yarp::os::Portable* portable;

};

#endif

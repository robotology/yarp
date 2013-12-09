// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali paikan and Paul Fitzpatrick
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
  
    Things() { 
        conReader = NULL;
        writer = NULL;
        portable = NULL;
    }

    ~Things() {
        if(portable)
            delete portable;
    }

    /**     
     * Set the reference to a PortWrtier object 
     *
     */
    void setPortWriter(yarp::os::PortWriter* writer) {
        yarp::os::Things::writer = writer;
    }

    yarp::os::PortWriter* getPortWriter(void) {
        return writer;
    }

    /**
     *  Things reader
     */
    bool read(yarp::os::ConnectionReader& connection) {
        conReader = &connection;
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

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_KNOWN_THINGS_
#define _YARP2_KNOWN_THINGS_

#include <yarp/os/Things.h>
#include <yarp/os/ConnectionReader.h>

namespace yarp {
    namespace os {
        class KnownThings;
    }
}

#define READ_PORTABLE(type) \
    if(!getReference()) { \
        if(dummy) delete dummy; \
        dummy = new type(); \
        if(!dummy->read(*reader)) { \
            delete dummy; \
            dummy = NULL; \
            return NULL; \
        } \
        setReference(dummy); \
    } \
    return dynamic_cast<type*>(getReference())


/**
 * Class KnownThings
 */
class yarp::os::KnownThings : public yarp::os::Things {

public:
    
    KnownThings() : reader(NULL),  dummy(NULL) { 
        setReference(NULL);
    }

    ~KnownThings() {
        if(dummy)
            delete dummy;
    }

    /**
     *  Things reader
     */
    bool read(yarp::os::ConnectionReader& connection) {
        reader = &connection;
        setReference(reader->getReference());
        return true;
    }

    /*
     * Things writer
     */
    bool write(yarp::os::ConnectionWriter& connection) {
        if(!getReference())
            return false;
        return getReference()->write(connection);
    }

    void reset() {
        if(dummy)
            delete dummy;        
        setReference(NULL);
        reader = NULL;
        dummy = NULL;
    }

    yarp::os::Value* asValue() {
        READ_PORTABLE(yarp::os::Value);
    }


    yarp::os::Bottle* asBottle() {
        READ_PORTABLE(yarp::os::Bottle);
    }

    yarp::os::Property* asProperty() {
        READ_PORTABLE(yarp::os::Property);
    }

    yarp::sig::Vector* asVector() {
        READ_PORTABLE(yarp::sig::Vector);
    }

    yarp::sig::Matrix* asMatrix() {
        READ_PORTABLE(yarp::sig::Matrix);
    }

    yarp::sig::Image* asImage() {
        READ_PORTABLE(yarp::sig::Image);
    }
     
    yarp::sig::ImageOf<yarp::sig::PixelRgb>* asImageOfPixelRgb() {
        READ_PORTABLE(yarp::sig::ImageOf<yarp::sig::PixelRgb>);
    }

    yarp::sig::ImageOf<yarp::sig::PixelBgr>* asImageOfPixelBgr() {
        READ_PORTABLE(yarp::sig::ImageOf<yarp::sig::PixelBgr>);
    }

    yarp::sig::ImageOf<yarp::sig::PixelMono>* asImageOfPixelMono() {
        READ_PORTABLE(yarp::sig::ImageOf<yarp::sig::PixelMono>);
    } 

      //yarp::sig::Sound* asSound(); 

private: 
    yarp::os::ConnectionReader* reader;
    yarp::os::Portable* dummy;
};

#endif

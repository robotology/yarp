// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali paikan and Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_THINGS_
#define _YARP2_THINGS_

#include <yarp/os/Value.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Sound.h>


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
private:
    yarp::os::Portable* portable;

public:

    /**
     *
     * Directly set the reference to a portable object 
     *
     */
    inline void setReference(yarp::os::Portable* portable) {
        yarp::os::Things::portable = portable;
    }

    /**
     *
     * Directly access to the portable object 
     *
     */
    inline yarp::os::Portable* getReference(void) {
        return portable;
    }

    /**
     *  Things reader
     */
    virtual bool read(yarp::os::ConnectionReader& connection) = 0;

    /*
     * Things  writer
     */
    virtual bool write(yarp::os::ConnectionWriter& connection) = 0;

    /**
     *
     * Get Value.
     *
     * @return a pointer to Value if thing is indeed a Value object. 
     * Otherwise returns NULL.
     */
     virtual yarp::os::Value* asValue() = 0; 

    /**
     *
     * Get Bottle.
     *
     * @return a pointer to Bottle if thing is indeed a Bottle object. 
     * Otherwise returns NULL.
     */
     virtual yarp::os::Bottle* asBottle() = 0; 

    /**
     *
     * Get Porperty.
     *
     * @return a pointer to Property if thing is indeed a Property object. 
     * Otherwise returns NULL.
     */
     virtual yarp::os::Property* asProperty() = 0; 

    /**
     *
     * Get Vector.
     *
     * @return a pointer to Vector if thing is indeed a Vector object. 
     * Otherwise returns NULL.
     */
     virtual yarp::sig::Vector* asVector() = 0; 

    /**
     *
     * Get Matrix.
     *
     * @return a pointer to Matrix if thing is indeed a Matrix object. 
     * Otherwise returns NULL.
     */
     virtual yarp::sig::Matrix* asMatrix() = 0; 

    /**
     *
     * Get Image.
     *
     * @return a pointer to Image if thing is indeed an Image object. 
     * Otherwise returns NULL.
     */
     virtual yarp::sig::Image* asImage() = 0; 


    /**
     *
     * Get ImageOf<PixelRgb>.
     *
     * @return a pointer to ImageOf<PixelRgb> if thing is indeed an ImageOf<PixelRgb> object. 
     * Otherwise returns NULL.
     */
     virtual yarp::sig::ImageOf<yarp::sig::PixelRgb>* asImageOfPixelRgb() = 0; 


    /**
     *
     * Get ImageOf<PixelBgr>.
     *
     * @return a pointer to ImageOf<PixelBgr> if thing is indeed an ImageOf<PixelBgr> object. 
     * Otherwise returns NULL.
     */
     virtual yarp::sig::ImageOf<yarp::sig::PixelBgr>* asImageOfPixelBgr() = 0; 

    /**
     *
     * Get ImageOf<PixelMono>.
     *
     * @return a pointer to ImageOf<PixelMono> if thing is indeed an ImageOf<PixelMono> object. 
     * Otherwise returns NULL.
     */
     virtual yarp::sig::ImageOf<yarp::sig::PixelMono>* asImageOfPixelMono() = 0; 

  

    /**
     *
     * Get Sound.
     *
     * @return a pointer to Sound if thing is indeed a Sound object. 
     * Otherwise returns NULL.
     */
     //virtual yarp::sig::Sound* asSound() = 0; 

};

#endif

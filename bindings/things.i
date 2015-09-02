/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

/*
 *
 * Extending yarp::os::Things.h
 */
#include <stdio.h>

%extend yarp::os::Things  {
public:

    yarp::os::Value* asValue() {
        return self->cast_as<yarp::os::Value>();
    }

    yarp::os::Bottle* asBottle() {
        return self->cast_as<yarp::os::Bottle>();
    }

    yarp::os::Property* asProperty() {
        return self->cast_as<yarp::os::Property>();
    }

    yarp::sig::Vector* asVector() {
        return self->cast_as<yarp::sig::Vector>();
    }

    yarp::sig::Matrix* asMatrix() {
        return self->cast_as<yarp::sig::Matrix>();
    }

    yarp::sig::Image* asImage() {
        return self->cast_as<yarp::sig::Image>();
    }

    yarp::sig::ImageOf<yarp::sig::PixelRgb>* asImageOfPixelRgb() {
        return self->cast_as<yarp::sig::ImageOf<yarp::sig::PixelRgb> >();
    }

    yarp::sig::ImageOf<yarp::sig::PixelBgr>* asImageOfPixelBgr() {
        return self->cast_as<yarp::sig::ImageOf<yarp::sig::PixelBgr> >();
    }

    yarp::sig::ImageOf<yarp::sig::PixelMono>* asImageOfPixelMono() {
        return self->cast_as<yarp::sig::ImageOf<yarp::sig::PixelMono> >();
    }
}


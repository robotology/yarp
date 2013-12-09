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

#define READ_PORTABLE(type) \
    if(self->writer)\
        return dynamic_cast<type*>(self->writer);\
    if(!self->portable) { \
        if(!self->conReader)\
            return NULL;\
        self->portable = new type(); \
        if(!self->portable->read(*self->conReader)) { \
            delete self->portable; \
            self->portable = NULL; \
            return NULL; \
        } \
    } \
    return dynamic_cast<type*>(self->portable);


%extend yarp::os::Things  {
public:
    
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
}

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_WIREBOTTLE
#define YARP2_WIREBOTTLE

#include <yarp/os/SizedWriter.h>

#include <wire_rep_utils_api.h>

class SizedWriterTail : public yarp::os::SizedWriter {
private:
    yarp::os::SizedWriter *delegate;
    size_t payload_index, payload_offset;
public:
    void setDelegate(yarp::os::SizedWriter *delegate, int index,
                     int offset) {
        this->delegate = delegate;
        payload_index = index;
        payload_offset = offset;
    }
    
    virtual size_t length() {
        return delegate->length()-payload_index;
    }

    virtual size_t headerLength() {
        return 0; // not supported
    }

    virtual size_t length(size_t index) {
        index += payload_index;
        if (index==payload_index) return delegate->length(index)-payload_offset;
        return delegate->length(index);
    }

    virtual const char *data(size_t index) {
        index += payload_index;
        if (index==payload_index) return delegate->data(index)+payload_offset;
        return delegate->data(index);
    }

    virtual yarp::os::PortReader *getReplyHandler() {
        return delegate->getReplyHandler();
    }
    
    virtual yarp::os::Portable *getReference() {
        return delegate->getReference();
    }

    virtual bool dropRequested() { return false; }


    virtual void startWrite() {
    }

    virtual void stopWrite() {
    }
};

class YARP_wire_rep_utils_API WireBottle {
public:
    static bool checkBottle(void *cursor, int len);
    static bool extractBlobFromBottle(yarp::os::SizedWriter& src,
                                      SizedWriterTail& dest);
};

#endif

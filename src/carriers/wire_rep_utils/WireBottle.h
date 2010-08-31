// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_WIREBOTTLE
#define YARP2_WIREBOTTLE

#include <yarp/os/impl/SizedWriter.h>

class SizedWriterTail : public yarp::os::impl::SizedWriter {
private:
    yarp::os::impl::SizedWriter *delegate;
    int payload_index, payload_offset;
public:
    void setDelegate(yarp::os::impl::SizedWriter *delegate, int index,
                     int offset) {
        this->delegate = delegate;
        payload_index = index;
        payload_offset = offset;
    }
    
    virtual int length() {
        return delegate->length()-payload_index;
    }

    virtual int headerLength() {
        return 0; // not supported
    }

    virtual int length(int index) {
        index += payload_index;
        if (index==payload_index) return delegate->length(index)-payload_offset;
        return delegate->length(index);
    }

    virtual const char *data(int index) {
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
};

class WireBottle {
public:
    static bool checkBottle(void *cursor, int len);
    static bool extractBlobFromBottle(yarp::os::impl::SizedWriter& src,
                                      SizedWriterTail& dest);
};

#endif

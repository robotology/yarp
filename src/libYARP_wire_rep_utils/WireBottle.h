/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

    virtual size_t length() override {
        return delegate->length()-payload_index;
    }

    virtual size_t headerLength() override {
        return 0; // not supported
    }

    virtual size_t length(size_t index) override {
        index += payload_index;
        if (index==payload_index) return delegate->length(index)-payload_offset;
        return delegate->length(index);
    }

    virtual const char *data(size_t index) override {
        index += payload_index;
        if (index==payload_index) return delegate->data(index)+payload_offset;
        return delegate->data(index);
    }

    virtual yarp::os::PortReader *getReplyHandler() override {
        return delegate->getReplyHandler();
    }

    virtual yarp::os::Portable *getReference() override {
        return delegate->getReference();
    }

    virtual bool dropRequested() override { return false; }


    virtual void startWrite() override {
    }

    virtual void stopWrite() override {
    }
};

class YARP_wire_rep_utils_API WireBottle {
public:
    static bool checkBottle(void *cursor, int len);
    static bool extractBlobFromBottle(yarp::os::SizedWriter& src,
                                      SizedWriterTail& dest);
};

#endif

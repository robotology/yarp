// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_WIRETWIDDLER
#define YARP2_WIRETWIDDLER

#include <yarp/os/Bottle.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/NetInt32.h>

#include <yarp/os/impl/SizedWriter.h>

#include <vector>

class WireTwiddlerGap {
public:
    int buffer_start;
    int buffer_length;
    int length;
    int unit_length;
    
    WireTwiddlerGap() {
        buffer_start = 0;
        buffer_length = 0;
        length = 0;
        unit_length = 0;
    }
};

class WireTwiddler {
public:
    WireTwiddler() {
        buffer_start = 0;
    }

    bool configure(const char *txt);

private:
    int buffer_start;
    std::vector<yarp::os::NetInt32> buffer;
    std::vector<WireTwiddlerGap> gaps;

    void show();
    int configure(yarp::os::Bottle& desc, int offset);
};

class WireTwiddlerWriter {
private:
    yarp::os::impl::SizedWriter& parent;
    WireTwiddler& twiddler;
public:
    WireTwiddlerWriter(yarp::os::impl::SizedWriter& parent,
                       WireTwiddler& twiddler) : parent(parent),
                                                 twiddler(twiddler)
    {}

    virtual ~WireTwiddlerWriter() {}

    virtual int length() = 0;

    virtual int headerLength() = 0;

    virtual int length(int index) = 0;

    virtual const char *data(int index) = 0;

    virtual yarp::os::PortReader *getReplyHandler() {
        return parent.getReplyHandler();
    }
    
    virtual yarp::os::Portable *getReference() {
        return parent.getReference();
    }
};

#endif


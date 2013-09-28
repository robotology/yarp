// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_WIREIMAGE
#define YARP2_WIREIMAGE

#include <yarp/os/SizedWriter.h>
#include <yarp/os/StringOutputStream.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/sig/Image.h>
#include <stdlib.h>
#include <string.h>

#include <wire_rep_utils_api.h>

    /*
    // layout for ROS:
    // Header header
    //   uint32 seq      --> +1
    //   time stamp      --> int32 secs, int32 nsecs (sync by time)
    //   string frame_id --> string! argh / just pass it along
    // uint32 height
    // uint32 width
    // string encoding   --> string! argh
    // uint8 is_bigendian
    // uint32 step
    // uint8[] data      --> real payload
    */

#include <yarp/os/begin_pack_for_net.h>
class RosImageStamp {
public:
    yarp::os::NetInt32 seq;
    yarp::os::NetInt32 sec;
    yarp::os::NetInt32 nsec;
} PACKED_FOR_NET;
#include <yarp/os/end_pack_for_net.h>

/**
 *
 * As far as YARP is concerned, on the wire to/from ROS a raw image has:
 * + a variable 12 byte header: seq secs nsecs
 * + a constant byte sequence with information about the image source
 * + a 4 byte header with the length of the binary image payload
 * + the binary image payload
 * The "constant" sequence should change if image size or other details change
 * but we optimize for the truly constant case.
 *
 */
class RosWireImage : public yarp::os::SizedWriter {
private:
    RosImageStamp ros_seq_stamp;
    yarp::os::ManagedBytes ros_const_header;
    const yarp::sig::FlexImage *image;
public:
    RosWireImage() {
    }

    void init(const yarp::sig::FlexImage& img, 
              const yarp::os::ConstString& frame) {
        image = &img;
        yarp::os::ConnectionWriter *pbuf = 
            yarp::os::ConnectionWriter::createBufferedConnectionWriter();
        if (!pbuf) ::exit(1);
        yarp::os::ConnectionWriter& buf = *pbuf;
        yarp::os::StringOutputStream ss;
        // probably need to translate encoding format better, but at
        // a guess "rgb" and "bgr" will work ok.
        yarp::os::ConstString encoding = 
            yarp::os::Vocab::decode(image->getPixelCode()).c_str();
        if (encoding=="rgb") encoding = "rgb8";
        if (encoding=="bgr") encoding = "bgr8";
        buf.appendRawString(frame);
        buf.appendInt(image->height());
        buf.appendInt(image->width());
        buf.appendRawString(encoding);
        char is_bigendian = 1;
        buf.appendBlock(&is_bigendian,1);
        buf.appendInt((image->width()*3)+image->getPadding());
        buf.appendInt(image->getRawImageSize());
        buf.getBuffer()->write(ss);
        yarp::os::ConstString hdr = ss.toString();
        yarp::os::Bytes hdr_wrap((char*)hdr.c_str(),hdr.length());
        ros_const_header = yarp::os::ManagedBytes(hdr_wrap);
        ros_const_header.copy();
        delete pbuf;
        pbuf = 0 /*NULL*/;
    }

    void update(const yarp::sig::FlexImage *img, int seq, double t) {
        // We should check if img properties have changed.  But we don't.
        ros_seq_stamp.seq = seq;
        ros_seq_stamp.sec = (int)(t);
        ros_seq_stamp.nsec = (int)((t-(int)t)*1e9);
    }

    virtual size_t length() { return 3; }

    virtual size_t headerLength() { return 0; }

    virtual size_t length(size_t index) {
        size_t result = 0;
        switch (index) {
        case 0:
            result = sizeof(ros_seq_stamp);
            break;
        case 1:
            result = ros_const_header.length();
            break;
        case 2:
            result = image->getRawImageSize();
            break;
        default:
            result = 0;
            break;
        }
        return result;
    }

    virtual const char *data(size_t index) {
        const char *result = 0 /*NULL*/;
        switch (index) {
        case 0:
            result = (const char *)(&ros_seq_stamp);
            break;
        case 1:
            result = ros_const_header.get();
            break;
        case 2:
            result = (const char *)(image->getRawImage());
            break;
        }
        return result;
    }

    virtual yarp::os::PortReader *getReplyHandler() { return NULL; }
    
    virtual yarp::os::Portable *getReference() { return NULL; }

    virtual bool dropRequested() { return false; }

    virtual void startWrite() {}

    virtual void stopWrite() {}
};

class YARP_wire_rep_utils_API WireImage {
private:
    yarp::sig::FlexImage img;
public:
    yarp::sig::FlexImage *checkForImage(yarp::os::SizedWriter& writer);
};

#endif

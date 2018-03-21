/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP2_WIREIMAGE
#define YARP2_WIREIMAGE

#include <yarp/conf/system.h>
#include <yarp/os/SizedWriter.h>
#include <yarp/os/StringOutputStream.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/sig/Image.h>
#include <cstdlib>
#include <cstring>

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

YARP_BEGIN_PACK
class RosImageStamp {
public:
    yarp::os::NetInt32 seq;
    yarp::os::NetInt32 sec;
    yarp::os::NetInt32 nsec;
};
YARP_END_PACK

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
    RosWireImage() :
        ros_seq_stamp({0,0,0}),
        image(nullptr)
    {}

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
        switch (image->getPixelCode()) {
        case VOCAB_PIXEL_BGR:
            encoding = "bgr8";
            break;
        case VOCAB_PIXEL_RGB:
            encoding = "rgb8";
            break;
        case VOCAB_PIXEL_MONO:
            encoding = "mono8";
            break;
        case VOCAB_PIXEL_MONO16:
            encoding = "mono16";
            break;
        case VOCAB_PIXEL_MONO_FLOAT:
            encoding = "32FC1";
            break;
        }
        buf.appendRawString(frame);
        buf.appendInt(image->height());
        buf.appendInt(image->width());
        buf.appendRawString(encoding);
        char is_bigendian = 0;
        buf.appendBlock(&is_bigendian,1);
        buf.appendInt((image->width()*image->getPixelSize())+image->getPadding());
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

    virtual size_t length() override { return 3; }

    virtual size_t headerLength() override { return 0; }

    virtual size_t length(size_t index) override {
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

    virtual const char *data(size_t index) override {
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

    virtual yarp::os::PortReader *getReplyHandler() override { return NULL; }

    virtual yarp::os::Portable *getReference() override { return NULL; }

    virtual bool dropRequested() override { return false; }

    virtual void startWrite() override {}

    virtual void stopWrite() override {}
};

class YARP_wire_rep_utils_API WireImage {
private:
    yarp::sig::FlexImage img;
public:
    yarp::sig::FlexImage *checkForImage(yarp::os::SizedWriter& writer);
};

#endif

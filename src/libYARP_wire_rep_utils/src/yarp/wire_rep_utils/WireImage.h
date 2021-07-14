/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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

#include <yarp/wire_rep_utils/api.h>

namespace yarp {
namespace wire_rep_utils {

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
              const std::string& frame) {
        image = &img;
        yarp::os::ConnectionWriter *pbuf =
            yarp::os::ConnectionWriter::createBufferedConnectionWriter();
        if (!pbuf) {
            ::exit(1);
        }
        yarp::os::ConnectionWriter& buf = *pbuf;
        yarp::os::StringOutputStream ss;
        // probably need to translate encoding format better, but at
        // a guess "rgb" and "bgr" will work ok.
        std::string encoding =
            yarp::os::Vocab32::decode(image->getPixelCode()).c_str();
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
        buf.appendString(frame);
        buf.appendInt32(image->height());
        buf.appendInt32(image->width());
        buf.appendString(encoding);
        char is_bigendian = 0;
        buf.appendBlock(&is_bigendian,1);
        buf.appendInt32((image->width()*image->getPixelSize())+image->getPadding());
        buf.appendInt32(image->getRawImageSize());
        buf.getBuffer()->write(ss);
        std::string hdr = ss.toString();
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

    size_t length() const override { return 3; }

    size_t headerLength() const override { return 0; }

    size_t length(size_t index) const override {
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

    const char *data(size_t index) const override {
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

    yarp::os::PortReader *getReplyHandler() override { return NULL; }

    yarp::os::Portable *getReference() override { return NULL; }

    bool dropRequested() override { return false; }

    void startWrite() const override {}

    void stopWrite() const override {}
};

class YARP_wire_rep_utils_API WireImage {
private:
    yarp::sig::FlexImage img;
public:
    yarp::sig::FlexImage *checkForImage(yarp::os::SizedWriter& writer);
};

} // namespace wire_rep_utils
} // namespace yarp

#endif

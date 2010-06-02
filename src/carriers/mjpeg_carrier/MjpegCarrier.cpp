// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "MjpegCarrier.h"

#include <jpeglib.h>

using namespace yarp::os::impl;

bool MjpegCarrier::write(Protocol& proto, SizedWriter& writer) {
    writer.write(proto.os());

    // next step: uncompress / compress

    /*
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    //...
    cinfo.err = jpeg_std_error();
    jpeg_create_compress();
    */


    return proto.os().isOk();
}

bool MjpegCarrier::reply(Protocol& proto, SizedWriter& writer) {
    return false;
}

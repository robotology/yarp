/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "WireImage.h"

#include <yarp/sig/ImageNetworkHeader.h>

using namespace yarp::os;
using namespace yarp::sig;

FlexImage *WireImage::checkForImage(SizedWriter& writer) {
    ImageNetworkHeader hdr;
    char *header_buf = (char*)(&hdr);
    size_t header_len = sizeof(hdr);
    const char *img_buf = NULL;
    int img_len = 0;
    hdr.imgSize = -1;
    for (size_t i=0; i<writer.length(); i++) {
        const char *data = writer.data(i);
        size_t len = writer.length(i);
        //printf("block %d length %d\n", i, len);
        if (header_len<len) {
            len = header_len;
        }
        if (len>0) {
            memcpy(header_buf,data,len);
            header_len -= len;
            header_buf += len;
        }
        if (header_len == 0) {
            img_buf = data+len;
            img_len = writer.length(i)-len;
        }
    }
    if (hdr.imgSize==-1) {
        return NULL;
    }
    if (hdr.imgSize!=img_len) {
        return NULL;
    }
    //printf("Passing on a %dx%d image\n", hdr.width, hdr.height);
    int w = hdr.width;
    int h = hdr.height;
    //int row_stride = hdr.imgSize/hdr.height;
    img.setPixelCode(hdr.id);
    img.setPixelSize(hdr.depth);
    img.setQuantum(hdr.quantum);
    img.setExternal((char*)img_buf,w,h);
    return &img;
}

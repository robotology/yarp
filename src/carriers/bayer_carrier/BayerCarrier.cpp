/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "BayerCarrier.h"

#include <yarp/sig/ImageDraw.h>
#include <string.h>
#include <stdlib.h>

#ifndef USE_LIBDC1394
extern "C" {
#include "conversions.h"
}
#else
#include <dc1394/dc1394.h>
#endif

using namespace yarp::os;
using namespace yarp::sig;

// can't seem to do ipl/opencv/yarp style end-of-row padding
void setDcImage(yarp::sig::Image& yimg, dc1394video_frame_t *dc, 
                int filter) {
    if (!dc) return;
    dc->image = (unsigned char *) yimg.getRawImage();
    dc->size[0] = (uint32_t) yimg.width();
    dc->size[1] = (uint32_t) yimg.height();
    dc->position[0] = 0;
    dc->position[1] = 0;
    dc->color_coding = (yimg.getPixelCode()==VOCAB_PIXEL_MONO)?DC1394_COLOR_CODING_RAW8:DC1394_COLOR_CODING_RGB8;
    dc->color_filter = (dc1394color_filter_t)filter;
    dc->yuv_byte_order = 0;
    dc->data_depth = 8;
    dc->stride = (uint32_t) yimg.getRowSize();
    dc->video_mode = DC1394_VIDEO_MODE_640x480_RGB8; // we are bluffing
    dc->image_bytes = (uint32_t)yimg.getRawImageSize();
    dc->padding_bytes = 0;
    dc->total_bytes = dc->image_bytes;
    dc->timestamp = 0;
    dc->frames_behind = 0;
    dc->camera = NULL;
    dc->id = 0;
    dc->allocated_image_bytes = dc->image_bytes;
#ifdef YARP_LITTLE_ENDIAN
    dc->little_endian = DC1394_TRUE;
#else
    dc->little_endian = DC1394_FALSE;
#endif
    dc->data_in_padding = DC1394_FALSE;
}

yarp::os::ConnectionReader& BayerCarrier::modifyIncomingData(yarp::os::ConnectionReader& reader) {

    /*
    // minimal test of image modification
    in.read(reader);
    out.copy(in);
    out.pixel(0,0).r = 42;
    out.write(con.getWriter());
    return con.getReader();
    */

    /*
    // minimal test of bottle modification
    con.setTextMode(reader.isTextMode());
    Bottle b;
    b.read(reader);
    b.addInt(42);
    b.addString("(p.s. bork bork bork)");
    b.write(con.getWriter());
    return con.getReader();
    */

    local->setParentConnectionReader(reader);

    // libdc1394 seems to need this.
    // note that this can slow things down if input has padding.
    in.setQuantum(1);
    out.setQuantum(1);

    Route r;
    bool ok = in.read(reader);
    if (!ok) {
        local->setSize(0);
        return *local;
    }
    ImageNetworkHeader header_in_cmp;
    header_in_cmp.setFromImage(in);
    if (!need_reset) {
        need_reset = (0!=memcmp(&header_in_cmp,&header_in,sizeof(header_in)));
    }
    have_result = false;
    if (need_reset) {
        int m = DC1394_BAYER_METHOD_BILINEAR;
        Searchable& config = reader.getConnectionModifiers();
        half = false;
        if (config.check("size")) {
            if (config.find("size").asString() == "half") {
                half = true;
            }
        }
        if (config.check("method")) {
            ConstString method = config.find("method").asString();
            bayer_method_set = true;
            if (method=="ahd") {
                m = DC1394_BAYER_METHOD_AHD;
            } else if (method=="bilinear") {
                m = DC1394_BAYER_METHOD_BILINEAR;
            } else if (method=="downsample") {
                m = DC1394_BAYER_METHOD_DOWNSAMPLE;
                half = true;
            } else if (method=="edgesense") {
                m = DC1394_BAYER_METHOD_EDGESENSE;
            } else if (method=="hqlinear") {
                m = DC1394_BAYER_METHOD_HQLINEAR;
            } else if (method=="nearest") {
                m = DC1394_BAYER_METHOD_NEAREST;
            } else if (method=="simple") {
                m = DC1394_BAYER_METHOD_SIMPLE;
            } else if (method=="vng") {
                m = DC1394_BAYER_METHOD_VNG;
            } else {
                if (!warned) {
                    fprintf(stderr,"bayer method %s not recognized, try: ahd bilinear downsample edgesense hqlinear nearest simple vng\n", method.c_str());
                    warned = true;
                }
                happy = false;
                local->setSize(0);
                return *local;
            }
        }

        setFormat(config.check("order",Value("grbg")).asString().c_str());
        header_in.setFromImage(in);
        //printf("Need reset.\n");
        bayer_method = m;
        need_reset = false;
        processBuffered();
    }
    local->setSize(sizeof(header)+image_data_len);
    consumed = 0;

    return *local;
}


bool BayerCarrier::debayerHalf(yarp::sig::ImageOf<PixelMono>& src,
                               yarp::sig::ImageOf<PixelRgb>& dest) {
    // dc1394 doesn't seem safe for arbitrary data widths
    if (src.width()%8==0) {
        dc1394video_frame_t dc_src;
        dc1394video_frame_t dc_dest;
        setDcImage(src,&dc_src,dcformat);
        setDcImage(dest,&dc_dest,dcformat);
        dc1394_debayer_frames(&dc_src,&dc_dest,DC1394_BAYER_METHOD_DOWNSAMPLE);
        return true;
    }

    if (bayer_method_set && !warned) {
        fprintf(stderr, "Not using dc1394 debayer methods (image width not a multiple of 8)\n");
        warned = true;
    }

    // a safer implementation that doesn't use dc1394
    int w = src.width();
    int h = src.height();
    int wo = dest.width();
    int ho = dest.height();
    int goff1 = 1-goff;
    int roffx = roff?goff:goff1;
    int boff = 1-roff;
    int boffx = boff?goff:goff1;
    for (int yo=0; yo<ho; yo++) {
        for (int xo=0; xo<wo; xo++) {
            PixelRgb& po = dest.pixel(xo,yo);
            int x = xo*2;
            int y = yo*2;
            if (x+1>=w-1 || y+1>=h-1) {
                po = PixelRgb(0,0,0);
                continue;
            }
            po.r = src.pixel(x+roffx,y+roff);
            po.b = src.pixel(x+boffx,y+boff);
            po.g = (PixelMono)(0.5*(src.pixel(x+goff,y)+src.pixel(x+goff1,y+1)));
        }
    }
    return true;
}

bool BayerCarrier::debayerFull(yarp::sig::ImageOf<PixelMono>& src,
                               yarp::sig::ImageOf<PixelRgb>& dest) {
    // dc1394 doesn't seem safe for arbitrary data widths
    if (src.width()%8==0) {
        dc1394video_frame_t dc_src;
        dc1394video_frame_t dc_dest;
        setDcImage(src,&dc_src,dcformat);
        setDcImage(dest,&dc_dest,dcformat);
        dc1394_debayer_frames(&dc_src,&dc_dest,
                              (dc1394bayer_method_t)bayer_method);
        return true;
    }

    if (bayer_method_set && !warned) {
        fprintf(stderr, "Not using dc1394 debayer methods (image width not a multiple of 8)\n");
        warned = true;
    }
    int w = dest.width();
    int h = dest.height();
    int goff1 = 1-goff;
    int roffx = roff?goff:goff1;
    int boff = 1-roff;
    int boffx = boff?goff:goff1;
    for (int y=0; y<h; y++) {
        for (int x=0; x<w; x++) {
            PixelRgb& po = dest.pixel(x,y);

            // G
            if ((x+y)%2==goff) {
                po.g = src.pixel(x,y);
            } else {
                float g = 0;
                int ct = 0;
                if (x>0) { g += src.pixel(x-1,y); ct++; }
                if (x<w-1) { g += src.pixel(x+1,y); ct++; }
                if (y>0) { g += src.pixel(x,y-1); ct++; }
                if (y<h-1) { g += src.pixel(x,y+1); ct++; }
                if (ct>0) g /= ct;
                po.g = (int)g;
            }

            // B
            if (y%2==boff && x%2==boffx) {
                po.b = src.pixel(x,y);
            } else if (y%2==boff) {
                float b = 0;
                int ct = 0;
                if (x>0) { b += src.pixel(x-1,y); ct++; }
                if (x<w-1) { b += src.pixel(x+1,y); ct++; }
                if (ct>0) b /= ct;
                po.b = (int)b;
            } else if (x%2==boffx) {
                float b = 0;
                int ct = 0;
                if (y>0) { b += src.pixel(x,y-1); ct++; }
                if (y<h-1) { b += src.pixel(x,y+1); ct++; }
                if (ct>0) b /= ct;
                po.b = (int)b;
            } else {
                float b = 0;
                int ct = 0;
                if (x>0&&y>0) { b += src.pixel(x-1,y-1); ct++; }
                if (x>0&&y<h-1) { b += src.pixel(x-1,y+1); ct++; }
                if (x<w-1&&y>0) { b += src.pixel(x+1,y-1); ct++; }
                if (x<w-1&&y<h-1) { b += src.pixel(x+1,y+1); ct++; }
                if (ct>0) b /= ct;
                po.b = (int)b;
            }

            // R
            if (y%2==roff && x%2==roffx) {
                po.r = src.pixel(x,y);
            } else if (y%2==roff) {
                float r = 0;
                int ct = 0;
                if (x>0) { r += src.pixel(x-1,y); ct++; }
                if (x<w-1) { r += src.pixel(x+1,y); ct++; }
                if (ct>0) r /= ct;
                po.r = (int)r;
            } else if (x%2==roffx) {
                float r = 0;
                int ct = 0;
                if (y>0) { r += src.pixel(x,y-1); ct++; }
                if (y<h-1) { r += src.pixel(x,y+1); ct++; }
                if (ct>0) r /= ct;
                po.r = (int)r;
            } else {
                float r = 0;
                int ct = 0;
                if (x>0&&y>0) { r += src.pixel(x-1,y-1); ct++; }
                if (x>0&&y<h-1) { r += src.pixel(x-1,y+1); ct++; }
                if (x<w-1&&y>0) { r += src.pixel(x+1,y-1); ct++; }
                if (x<w-1&&y<h-1) { r += src.pixel(x+1,y+1); ct++; }
                if (ct>0) r /= ct;
                po.r = (int)r;
            }
        }
    }
    return true;
}

bool BayerCarrier::processBuffered() {
    if (!have_result) {
        //printf("Copy-based conversion.\n");
        if (half) {
            out.resize(in.width()/2,in.height()/2);
            debayerHalf(in,out);
        } else {
            out.resize(in);
            debayerFull(in,out);
        }
        header.setFromImage(out);
        image_data_len = (size_t)out.getRawImageSize();
    }
    have_result = true;
    return true;
}

bool BayerCarrier::processDirect(const yarp::os::Bytes& bytes) {
    if (have_result) {
        memcpy(bytes.get(),out.getRawImage(),bytes.length());
        return true;
    }
    //printf("Copyless conversion\n");
    ImageOf<PixelRgb> wrap;
    wrap.setQuantum(out.getQuantum());
    wrap.setExternal(bytes.get(),out.width(),out.height());
    if (half) {
        debayerHalf(in,wrap);
    } else {
        debayerFull(in,wrap);
    }
    return true;
}


YARP_SSIZE_T BayerCarrier::read(const yarp::os::Bytes& b) {
    // copy across small stuff - the image header
    if (consumed<sizeof(header)) {
        size_t len = b.length();
        if (len>sizeof(header)-consumed) {
            len = sizeof(header)-consumed;
        }
        memcpy(b.get(),((char*)(&header))+consumed,len);
        consumed += len;
        return (YARP_SSIZE_T) len;
    }
    // sane client will want to read image into correct-sized block
    if (b.length()==image_data_len) {
        // life is good!
        processDirect(b);
        consumed += image_data_len;
        return image_data_len;
    }
    // funky client, fall back on image copy
    processBuffered();
    if (consumed<sizeof(header)+out.getRawImageSize()) {
        size_t len = b.length();
        if (len>sizeof(header)+out.getRawImageSize()-consumed) {
            len = sizeof(header)+out.getRawImageSize()-consumed;
        }
        memcpy(b.get(),out.getRawImage()+consumed-sizeof(header),len);
        consumed += len;
        return (YARP_SSIZE_T) len;
    }
    return -1;
}


bool BayerCarrier::setFormat(const char *fmt) {
    dcformat = DC1394_COLOR_FILTER_GRBG;
    ConstString f(fmt);
    if (f.length()<2) return false;
    goff = (f[0]=='g'||f[0]=='G')?0:1;
    roff = (f[0]=='r'||f[0]=='R'||f[1]=='r'||f[1]=='R')?0:1;
    if (goff==0&&roff==0) {
        dcformat = DC1394_COLOR_FILTER_GRBG;
    } else if (goff==0&&roff==1) {
        dcformat = DC1394_COLOR_FILTER_GBRG;
    } else if (goff==1&&roff==0) {
        dcformat = DC1394_COLOR_FILTER_RGGB;
    } else if (goff==1&&roff==1) {
        dcformat = DC1394_COLOR_FILTER_BGGR;
    }
    return true;
}


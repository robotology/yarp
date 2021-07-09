/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "BayerCarrier.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/Route.h>
#include <yarp/sig/ImageDraw.h>
#include <cstring>
#include <cstdlib>

#ifndef USE_LIBDC1394
extern "C" {
#include "conversions.h"
}
#else
#include <dc1394/dc1394.h>
#endif

using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(BAYERCARRIER,
                   "yarp.carrier.bayer",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
}

// can't seem to do ipl/opencv/yarp style end-of-row padding
void setDcImage(yarp::sig::Image& yimg, dc1394video_frame_t *dc,
                int filter) {
    if (!dc) {
        return;
    }
    dc->image = yimg.getRawImage();
    dc->size[0] = static_cast<uint32_t>(yimg.width());
    dc->size[1] = static_cast<uint32_t>(yimg.height());
    dc->position[0] = 0;
    dc->position[1] = 0;
    dc->color_coding = (yimg.getPixelCode()==VOCAB_PIXEL_MONO)?DC1394_COLOR_CODING_RAW8:DC1394_COLOR_CODING_RGB8;
    dc->color_filter = static_cast<dc1394color_filter_t>(filter);
    dc->yuv_byte_order = 0;
    dc->data_depth = 8;
    dc->stride = static_cast<uint32_t>(yimg.getRowSize());
    dc->video_mode = DC1394_VIDEO_MODE_640x480_RGB8; // we are bluffing
    dc->image_bytes = static_cast<uint32_t>(yimg.getRawImageSize());
    dc->padding_bytes = 0;
    dc->total_bytes = dc->image_bytes;
    dc->timestamp = 0;
    dc->frames_behind = 0;
    dc->camera = nullptr;
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
    b.addInt32(42);
    b.addString("(p.s. bork bork bork)");
    b.write(con.getWriter());
    return con.getReader();
    */

    local->setParentConnectionReader(&reader);

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
        const Searchable& config = reader.getConnectionModifiers();
        half = false;
        if (config.check("size")) {
            if (config.find("size").asString() == "half") {
                half = true;
            }
        }
        if (config.check("method")) {
            std::string method = config.find("method").asString();
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
                yCWarning/*Once*/(BAYERCARRIER, "bayer method %s not recognized, try: ahd bilinear downsample edgesense hqlinear nearest simple vng", method.c_str());
                happy = false;
                local->setSize(0);
                return *local;
            }
        }

        setFormat(config.check("order",Value("grbg")).asString().c_str());
        header_in.setFromImage(in);
        yCTrace(BAYERCARRIER, "Need reset.");
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

    if (bayer_method_set) {
        yCWarning/*Once*/(BAYERCARRIER, "Not using dc1394 debayer methods (image width not a multiple of 8)");
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
                po = PixelRgb{0,0,0};
                continue;
            }
            po.r = src.pixel(x+roffx,y+roff);
            po.b = src.pixel(x+boffx,y+boff);
            po.g = static_cast<PixelMono>(0.5*(src.pixel(x+goff,y)+src.pixel(x+goff1,y+1)));
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
                              static_cast<dc1394bayer_method_t>(bayer_method));
        return true;
    }

    if (bayer_method_set) {
        yCWarning/*Once*/(BAYERCARRIER, "Not using dc1394 debayer methods (image width not a multiple of 8)");
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
                if (ct>0) { g /= ct; }
                po.g = static_cast<int>(g);
            }

            // B
            if (y%2==boff && x%2==boffx) {
                po.b = src.pixel(x,y);
            } else if (y%2==boff) {
                float b = 0;
                int ct = 0;
                if (x>0) { b += src.pixel(x-1,y); ct++; }
                if (x<w-1) { b += src.pixel(x+1,y); ct++; }
                if (ct>0) { b /= ct; }
                po.b = static_cast<int>(b);
            } else if (x%2==boffx) {
                float b = 0;
                int ct = 0;
                if (y>0) { b += src.pixel(x,y-1); ct++; }
                if (y<h-1) { b += src.pixel(x,y+1); ct++; }
                if (ct>0) { b /= ct; }
                po.b = static_cast<int>(b);
            } else {
                float b = 0;
                int ct = 0;
                if (x>0&&y>0) { b += src.pixel(x-1,y-1); ct++; }
                if (x>0&&y<h-1) { b += src.pixel(x-1,y+1); ct++; }
                if (x<w-1&&y>0) { b += src.pixel(x+1,y-1); ct++; }
                if (x<w-1&&y<h-1) { b += src.pixel(x+1,y+1); ct++; }
                if (ct>0) { b /= ct; }
                po.b = static_cast<int>(b);
            }

            // R
            if (y%2==roff && x%2==roffx) {
                po.r = src.pixel(x,y);
            } else if (y%2==roff) {
                float r = 0;
                int ct = 0;
                if (x>0) { r += src.pixel(x-1,y); ct++; }
                if (x<w-1) { r += src.pixel(x+1,y); ct++; }
                if (ct>0) { r /= ct; }
                po.r = static_cast<int>(r);
            } else if (x%2==roffx) {
                float r = 0;
                int ct = 0;
                if (y>0) { r += src.pixel(x,y-1); ct++; }
                if (y<h-1) { r += src.pixel(x,y+1); ct++; }
                if (ct>0) { r /= ct; }
                po.r = static_cast<int>(r);
            } else {
                float r = 0;
                int ct = 0;
                if (x>0&&y>0) { r += src.pixel(x-1,y-1); ct++; }
                if (x>0&&y<h-1) { r += src.pixel(x-1,y+1); ct++; }
                if (x<w-1&&y>0) { r += src.pixel(x+1,y-1); ct++; }
                if (x<w-1&&y<h-1) { r += src.pixel(x+1,y+1); ct++; }
                if (ct>0) { r /= ct; }
                po.r = static_cast<int>(r);
            }
        }
    }
    return true;
}

bool BayerCarrier::processBuffered() const {
    return const_cast<BayerCarrier*>(this)->processBuffered();
}

bool BayerCarrier::processBuffered() {
    if (!have_result) {
        yCTrace(BAYERCARRIER, "Copy-based conversion.");
        if (half) {
            out.resize(in.width()/2,in.height()/2);
            debayerHalf(in,out);
        } else {
            out.resize(in);
            debayerFull(in,out);
        }
        header.setFromImage(out);
        image_data_len = out.getRawImageSize();
    }
    have_result = true;
    return true;
}

bool BayerCarrier::processDirect(yarp::os::Bytes& bytes) {
    if (have_result) {
        memcpy(bytes.get(),out.getRawImage(),bytes.length());
        return true;
    }
    yCTrace(BAYERCARRIER, "Copyless conversion");
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


yarp::conf::ssize_t BayerCarrier::read(yarp::os::Bytes& b) {
    // copy across small stuff - the image header
    if (consumed<sizeof(header)) {
        size_t len = b.length();
        if (len>sizeof(header)-consumed) {
            len = sizeof(header)-consumed;
        }
        memcpy(b.get(),(reinterpret_cast<char*>(&header))+consumed,len);
        consumed += len;
        return static_cast<yarp::conf::ssize_t>(len);
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
        return static_cast<yarp::conf::ssize_t>(len);
    }
    return -1;
}


bool BayerCarrier::setFormat(const char *fmt) {
    dcformat = DC1394_COLOR_FILTER_GRBG;
    std::string f(fmt);
    if (f.length()<2) {
        return false;
    }
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

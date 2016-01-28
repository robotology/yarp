// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>

#ifdef WIN32
#define INT32 long  // jpeg's definition
#define QGLOBAL_H 1
#endif

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4091)
#endif

extern "C" {
#include <jpeglib.h>
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#ifdef WIN32
#undef INT32
#undef QGLOBAL_H
#endif

#include <setjmp.h>

#include <yarp/os/Log.h>
#include <yarp/sig/Image.h>
#include "MjpegDecompression.h"

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::mjpeg;

struct net_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};
typedef struct net_error_mgr *net_error_ptr;

typedef jpeg_source_mgr *net_src_ptr;

void init_net_source (j_decompress_ptr cinfo) {
    //net_src_ptr src = (net_src_ptr) cinfo->src;
}


boolean fill_net_input_buffer (j_decompress_ptr cinfo)
{
    // The whole JPEG data is expected to reside in the supplied memory
    // buffer, so any request for more data beyond the given buffer size
    // is treated as an error.
    JOCTET *mybuffer = (JOCTET *) cinfo->client_data;
    fprintf(stderr, "JPEG data unusually large\n");
    // Insert a fake EOI marker
    mybuffer[0] = (JOCTET) 0xFF;
    mybuffer[1] = (JOCTET) JPEG_EOI;
    cinfo->src->next_input_byte = mybuffer;
    cinfo->src->bytes_in_buffer = 2;
    return TRUE;
}

void net_error_exit (j_common_ptr cinfo) {
    net_error_ptr myerr = (net_error_ptr) cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

void skip_net_input_data (j_decompress_ptr cinfo, long num_bytes)
{
    net_src_ptr src = (net_src_ptr) cinfo->src;

    if (num_bytes > 0) {
        while (num_bytes > (long) src->bytes_in_buffer) {
            num_bytes -= (long) src->bytes_in_buffer;
            (void) (*src->fill_input_buffer) (cinfo);
        }
        src->next_input_byte += (size_t) num_bytes;
        src->bytes_in_buffer -= (size_t) num_bytes;
    }
}

void term_net_source (j_decompress_ptr cinfo) {
}

void jpeg_net_src (j_decompress_ptr cinfo, char *buf, int buflen) {
    net_src_ptr src;
    if (cinfo->src == NULL) {	/* first time for this JPEG object? */
        cinfo->src = (struct jpeg_source_mgr *)
            (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                        sizeof(jpeg_source_mgr));
        src = (net_src_ptr) cinfo->src;
    }

    src = (net_src_ptr) cinfo->src;
    src->init_source = init_net_source;
    src->fill_input_buffer = fill_net_input_buffer;
    src->skip_input_data = skip_net_input_data;
    src->resync_to_restart = jpeg_resync_to_restart; /* use default method */
    src->term_source = term_net_source;
    src->bytes_in_buffer = buflen;
    src->next_input_byte = (JOCTET *)buf;
}


class MjpegDecompressionHelper {
public:
    bool active;
    struct jpeg_decompress_struct cinfo;
    struct net_error_mgr jerr;
    JOCTET error_buffer[4];
    yarp::os::InputStream::readEnvelopeCallbackType readEnvelopeCallback;
    void* readEnvelopeCallbackData;

    MjpegDecompressionHelper() :
            active(false),
            readEnvelopeCallback(NULL),
            readEnvelopeCallbackData(NULL) {
    }

    bool setReadEnvelopeCallback(yarp::os::InputStream::readEnvelopeCallbackType callback,
                                 void* data)
    {
        readEnvelopeCallback = callback;
        readEnvelopeCallbackData = data;
        return true;
    }

    void init() {
        jpeg_create_decompress(&cinfo);
    }

    bool decompress(const Bytes& cimg, ImageOf<PixelRgb>& img) {
        bool debug = false;

        if (!active) {
            init();
            active = true;
        }
        cinfo.client_data = &error_buffer;
        cinfo.err = jpeg_std_error(&jerr.pub);
        jerr.pub.error_exit = net_error_exit;

        if (setjmp(jerr.setjmp_buffer)) {
            jpeg_finish_decompress(&cinfo);
            return false;
        }

        jpeg_net_src(&cinfo,cimg.get(),cimg.length());
        jpeg_save_markers(&cinfo, JPEG_COM, 0xFFFF);
        jpeg_read_header(&cinfo, TRUE);
        jpeg_calc_output_dimensions(&cinfo);
        if (debug) printf("Got image %dx%d\n", cinfo.output_width, cinfo.output_height);
        img.resize(cinfo.output_width,cinfo.output_height);
        jpeg_start_decompress(&cinfo);
        //int row_stride = cinfo.output_width * cinfo.output_components;

        int at = 0;
        while (cinfo.output_scanline < cinfo.output_height) {
            JSAMPLE *lines[1];
            lines[0] = (JSAMPLE*)(&img.pixel(0,at));
            jpeg_read_scanlines(&cinfo, lines, 1);
            at++;
        }
        if(readEnvelopeCallback && cinfo.marker_list && cinfo.marker_list->data_length > 0) {
            Bytes envelope(reinterpret_cast<char*>(cinfo.marker_list->data), cinfo.marker_list->data_length);
            readEnvelopeCallback(readEnvelopeCallbackData, envelope);
        }
        if (debug) printf("Read image!\n");
        jpeg_finish_decompress(&cinfo);
        return true;
    }

    void fini() {
        jpeg_destroy_decompress(&cinfo);
    }

    ~MjpegDecompressionHelper() {
        if (active) {
            fini();
            active = false;
        }
    }
};

#define HELPER(x) (*((MjpegDecompressionHelper*)(x)))

MjpegDecompression::MjpegDecompression() {
    system_resource = new MjpegDecompressionHelper;
    yAssert(system_resource!=NULL);
}

MjpegDecompression::~MjpegDecompression() {
    if (system_resource!=NULL) {
        delete &HELPER(system_resource);
        system_resource = NULL;
    }
}


bool MjpegDecompression::decompress(const yarp::os::Bytes& data, 
                                    yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
    MjpegDecompressionHelper& helper = HELPER(system_resource);
    return helper.decompress(data, image);
}

bool MjpegDecompression::setReadEnvelopeCallback(InputStream::readEnvelopeCallbackType callback,
                                                 void* data)
{
    MjpegDecompressionHelper& helper = HELPER(system_resource);
    return helper.setReadEnvelopeCallback(callback, data);
}


bool MjpegDecompression::isAutomatic() const {
#ifdef MJPEG_AUTOCOMPRESS
    return true;
#else
    return false;
#endif
}


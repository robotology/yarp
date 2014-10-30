// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <stdio.h>


/*
  On Windows, libjpeg does some slightly odd-ball stuff, including
  unconditionally defining INT32 to be "long".  This needs to
  be worked around.  Work around begins...
 */
#ifdef WIN32
#define INT32 long  // jpeg's definition
#define QGLOBAL_H 1
#endif
extern "C" {
#include <jpeglib.h>
}
#ifdef WIN32
#undef INT32
#undef QGLOBAL_H
#endif
/*
  work around ends.
 */


#include "MjpegCarrier.h"

#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>
#include <yarp/os/Name.h>
#include <yarp/os/Bytes.h>

#include "WireImage.h"

using namespace yarp::os;
using namespace yarp::sig;

#define dbg_printf if (0) printf

typedef struct {
    struct jpeg_destination_mgr pub;

    JOCTET *buffer;
    int bufsize;
    JOCTET cache[1000000];  // need to make this variable...
} net_destination_mgr;

typedef net_destination_mgr *net_destination_ptr;

void send_net_data(JOCTET *data, int len, void *client) {
    dbg_printf("Send %d bytes\n", len);
    ConnectionState *p = (ConnectionState *)client;
    char hdr[1000];
    sprintf(hdr,"\n");
    const char *brk = "\n";
    if (hdr[1]=='\0') {
        brk = "\r\n";
    }
    dbg_printf("Using terminator %s\n",(hdr[1]=='\0')?"\\r\\n":"\\n");
    sprintf(hdr,"Content-Type: image/jpeg%s\
Content-Length: %d%s%s", brk, len, brk, brk);
    Bytes hbuf(hdr,strlen(hdr));
    p->os().write(hbuf);
    Bytes buf((char *)data,len);
    /*
      // add corruption now and then, for testing.
    static int ct = 0;
    ct++;
    if (ct==50) {
        printf("Adding corruption\n");
        buf.get()[0] = 'z';
        ct = 0;
    }
    */
    p->os().write(buf);
    sprintf(hdr,"%s--boundarydonotcross%s",brk,brk);
    Bytes hbuf2(hdr,strlen(hdr));
    p->os().write(hbuf2);

}

static void init_net_destination(j_compress_ptr cinfo) {
    //printf("Initializing destination\n");
    net_destination_ptr dest = (net_destination_ptr)cinfo->dest;
    dest->buffer = &(dest->cache[0]);
    dest->bufsize = sizeof(dest->cache);
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = dest->bufsize;
}

static boolean empty_net_output_buffer(j_compress_ptr cinfo) {
    net_destination_ptr dest = (net_destination_ptr)cinfo->dest;
    printf("Empty buffer - PROBLEM\n");
    send_net_data(dest->buffer,dest->bufsize-dest->pub.free_in_buffer,
                  cinfo->client_data);
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = dest->bufsize;
    return TRUE;
}

static void term_net_destination(j_compress_ptr cinfo) {
    net_destination_ptr dest = (net_destination_ptr)cinfo->dest;
    //printf("Terminating net %d %d\n", dest->bufsize,dest->pub.free_in_buffer);
    send_net_data(dest->buffer,dest->bufsize-dest->pub.free_in_buffer,
                  cinfo->client_data);
}

void jpeg_net_dest(j_compress_ptr cinfo) {
    net_destination_ptr dest;

      //ERREXIT(cinfo, JERR_BUFFER_SIZE);

    /* The destination object is made permanent so that multiple JPEG images
     * can be written to the same buffer without re-executing jpeg_net_dest.
     */
    if (cinfo->dest == NULL) {    /* first time for this JPEG object? */
        cinfo->dest = (struct jpeg_destination_mgr *)
            (*cinfo->mem->alloc_large) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                        sizeof(net_destination_mgr));
    }

    dest = (net_destination_ptr) cinfo->dest;
    dest->pub.init_destination = init_net_destination;
    dest->pub.empty_output_buffer = empty_net_output_buffer;
    dest->pub.term_destination = term_net_destination;
}

bool MjpegCarrier::write(ConnectionState& proto, SizedWriter& writer) {
    WireImage rep;
    FlexImage *img = rep.checkForImage(writer);

    if (img==NULL) return false;
    int w = img->width();
    int h = img->height();
    int row_stride = img->getRowSize();
    JOCTET *data = (JOCTET*)img->getRawImage();

    JSAMPROW row_pointer[1];

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    cinfo.client_data = &proto;
    jpeg_create_compress(&cinfo);
    jpeg_net_dest(&cinfo);
    cinfo.image_width = w;
    cinfo.image_height = h;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    //jpeg_set_quality(&cinfo, 85, TRUE);
    dbg_printf("Starting to compress...\n");
    jpeg_start_compress(&cinfo, TRUE);
    dbg_printf("Done compressing (height %d)\n", cinfo.image_height);
    while (cinfo.next_scanline < cinfo.image_height) {
        dbg_printf("Writing row %d...\n", cinfo.next_scanline);
        row_pointer[0] = data + cinfo.next_scanline * row_stride;
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    return true;
}

bool MjpegCarrier::reply(ConnectionState& proto, SizedWriter& writer) {
    return false;
}


bool MjpegCarrier::sendHeader(ConnectionState& proto) {
    Name n(proto.getRoute().getCarrierName() + "://test");
    ConstString pathValue = n.getCarrierModifier("path");
    ConstString target = "GET /?action=stream\n\n";
    if (pathValue!="") {
        target = "GET /";
        target += pathValue;
    }
    target += " HTTP/1.1\n";
    Contact host = proto.getRoute().getToContact();
    if (host.getHost()!="") {
        target += "Host: ";
        target += host.getHost();
        target += "\r\n";
    }
    target += "\n";
    Bytes b((char*)target.c_str(),target.length());
    proto.os().write(b);
    return true;
}

bool MjpegCarrier::autoCompression() const {
#ifdef MJPEG_AUTOCOMPRESS
    return true;
#else
    return false;
#endif
}


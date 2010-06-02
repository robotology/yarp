// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "MjpegCarrier.h"

extern "C" {
#include <jpeglib.h>
}

#include <yarp/sig/Image.h>

using namespace yarp::os::impl;
using namespace yarp::sig;

typedef struct {
    struct jpeg_destination_mgr pub;

    JOCTET *buffer; /* start of buffer */
    int bufsize;
    JOCTET cache[10000];
} net_destination_mgr;

typedef net_destination_mgr *net_destination_ptr;

void send_net_data(JOCTET *data, int len, void *client) {
    printf("Send %d bytes\n", len);
    Protocol *p = (Protocol *)client;
    Bytes buf((char *)data,len);
    p->os().write(buf);
}

static void init_net_destination(j_compress_ptr cinfo) {
    printf("Initializing destination\n");
    net_destination_ptr dest = (net_destination_ptr)cinfo->dest;
    dest->buffer = &(dest->cache[0]);
    dest->bufsize = sizeof(dest->cache);
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = dest->bufsize;
}

static boolean empty_net_output_buffer(j_compress_ptr cinfo) {
    net_destination_ptr dest = (net_destination_ptr)cinfo->dest;
    printf("Empty buffer\n");
    send_net_data(dest->buffer,dest->bufsize-dest->pub.free_in_buffer,
                  cinfo->client_data);
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = dest->bufsize;
    return TRUE;
}

static void term_net_destination(j_compress_ptr cinfo) {
    net_destination_ptr dest = (net_destination_ptr)cinfo;
    printf("Terminating net\n");
    send_net_data(dest->buffer,dest->bufsize-dest->pub.free_in_buffer,
                  cinfo->client_data);
}

void jpeg_net_dest(j_compress_ptr cinfo) {
    net_destination_ptr dest;

      //ERREXIT(cinfo, JERR_BUFFER_SIZE);

    /* The destination object is made permanent so that multiple JPEG images
     * can be written to the same buffer without re-executing jpeg_net_dest.
     */
    if (cinfo->dest == NULL) {	/* first time for this JPEG object? */
        cinfo->dest = (struct jpeg_destination_mgr *)
            (*cinfo->mem->alloc_large) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                        sizeof(net_destination_mgr));
    }

    dest = (net_destination_ptr) cinfo->dest;
    dest->pub.init_destination = init_net_destination;
    dest->pub.empty_output_buffer = empty_net_output_buffer;
    dest->pub.term_destination = term_net_destination;
}

bool MjpegCarrier::write(Protocol& proto, SizedWriter& writer) {
    writer.write(proto.os());

    // next step: uncompress / compress

    ImageOf<PixelRgb> img;
    int w = 8;
    int h = 8;
    img.resize(w,h);
    for (int x=0; x<w; x++) {
        for (int y=0; y<h; y++) {
            PixelRgb& pix = img(x,y);
            pix = PixelRgb(x%256,(y*2)%256,0);
        }
    }
	JSAMPROW row_pointer[1];	/* pointer to a single row */
	int row_stride;			/* physical row width in buffer */
	row_stride = w * 3;	/* JSAMPLEs per row in image_buffer */
    JOCTET *data = (JOCTET*)img.getRawImage();

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
    jpeg_start_compress(&cinfo, TRUE);
    while (cinfo.next_scanline < cinfo.image_height) {
        printf("Writing row %d...\n", cinfo.next_scanline);
        row_pointer[0] = data + cinfo.next_scanline * row_stride;
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    return proto.os().isOk();
}

bool MjpegCarrier::reply(Protocol& proto, SizedWriter& writer) {
    return false;
}

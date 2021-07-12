/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "MjpegCarrier.h"
#include "MjpegLogComponent.h"

#include <cstdio>

/*
  On Windows, libjpeg does some slightly odd-ball stuff, including
  unconditionally defining INT32 to be "long".  This needs to
  be worked around.  Work around begins...
 */
#if defined(_WIN32)
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

#if defined(_WIN32)
#undef INT32
#undef QGLOBAL_H
#endif
/*
  work around ends.
 */

#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>
#include <yarp/os/Name.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/Route.h>

#include <yarp/wire_rep_utils/WireImage.h>

#include <map>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::wire_rep_utils;

static const std::map<int, J_COLOR_SPACE> yarpCode2Mjpeg { {VOCAB_PIXEL_MONO, JCS_GRAYSCALE},
                                                           {VOCAB_PIXEL_MONO16, JCS_GRAYSCALE},
                                                           {VOCAB_PIXEL_RGB , JCS_RGB},
                                                           {VOCAB_PIXEL_RGBA , JCS_EXT_RGBA},
                                                           {VOCAB_PIXEL_BGRA , JCS_EXT_BGRA},
                                                           {VOCAB_PIXEL_BGR , JCS_EXT_BGR} };

static const std::map<int, int> yarpCode2Channels { {VOCAB_PIXEL_MONO, 1},
                                                    {VOCAB_PIXEL_MONO16, 2},
                                                    {VOCAB_PIXEL_RGB , 3},
                                                    {VOCAB_PIXEL_RGBA , 4},
                                                    {VOCAB_PIXEL_BGRA , 4},
                                                    {VOCAB_PIXEL_BGR , 3} };


struct net_destination_mgr
{
    struct jpeg_destination_mgr pub;

    JOCTET *buffer;
    int bufsize;
    JOCTET cache[1000000];  // need to make this variable...
};

using net_destination_ptr = net_destination_mgr*;

void send_net_data(JOCTET *data, int len, void *client) {
    yCTrace(MJPEGCARRIER, "Send %d bytes", len);
    auto* p = (ConnectionState *)client;
    constexpr size_t hdr_size = 1000;
    char hdr[hdr_size];
    const char *brk = "\r\n";
    std::snprintf(hdr, hdr_size, "Content-Type: image/jpeg%s\
Content-Length: %d%s%s", brk, len, brk, brk);
    Bytes hbuf(hdr,strlen(hdr));
    p->os().write(hbuf);
    Bytes buf((char *)data,len);
    /*
      // add corruption now and then, for testing.
    static int ct = 0;
    ct++;
    if (ct==50) {
        yCTrace(MJPEGCARRIER, "Adding corruption");
        buf.get()[0] = 'z';
        ct = 0;
    }
    */
    p->os().write(buf);
    std::snprintf(hdr, hdr_size, "%s--boundarydonotcross%s", brk, brk);
    Bytes hbuf2(hdr,strlen(hdr));
    p->os().write(hbuf2);

}

static void init_net_destination(j_compress_ptr cinfo) {
    yCTrace(MJPEGCARRIER, "Initializing destination");
    auto dest = (net_destination_ptr)cinfo->dest;
    dest->buffer = &(dest->cache[0]);
    dest->bufsize = sizeof(dest->cache);
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = dest->bufsize;
}

static boolean empty_net_output_buffer(j_compress_ptr cinfo) {
    auto dest = (net_destination_ptr)cinfo->dest;
    yCWarning(MJPEGCARRIER, "Empty buffer - PROBLEM");
    send_net_data(dest->buffer,dest->bufsize-dest->pub.free_in_buffer,
                  cinfo->client_data);
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = dest->bufsize;
    return TRUE;
}

static void term_net_destination(j_compress_ptr cinfo) {
    auto dest = (net_destination_ptr)cinfo->dest;
    yCTrace(MJPEGCARRIER, "Terminating net %d %zd", dest->bufsize,dest->pub.free_in_buffer);
    send_net_data(dest->buffer,dest->bufsize-dest->pub.free_in_buffer,
                  cinfo->client_data);
}

void jpeg_net_dest(j_compress_ptr cinfo) {
    net_destination_ptr dest;

      //ERREXIT(cinfo, JERR_BUFFER_SIZE);

    /* The destination object is made permanent so that multiple JPEG images
     * can be written to the same buffer without re-executing jpeg_net_dest.
     */
    if (cinfo->dest == nullptr) {    /* first time for this JPEG object? */
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

    if (img==nullptr) return false;
    int w = img->width();
    int h = img->height();
    int row_stride = img->getRowSize();
    auto* data = (JOCTET*)img->getRawImage();

    JSAMPROW row_pointer[1];

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    cinfo.client_data = &proto;
    jpeg_create_compress(&cinfo);
    jpeg_net_dest(&cinfo);
    cinfo.image_width = w;
    cinfo.image_height = h;
    cinfo.in_color_space = yarpCode2Mjpeg.at(img->getPixelCode());
    cinfo.input_components = yarpCode2Channels.at(img->getPixelCode());
    jpeg_set_defaults(&cinfo);
    //jpeg_set_quality(&cinfo, 85, TRUE);
    yCTrace(MJPEGCARRIER, "Starting to compress...");
    jpeg_start_compress(&cinfo, TRUE);
    if(!envelope.empty()) {
        jpeg_write_marker(&cinfo, JPEG_COM, reinterpret_cast<const JOCTET*>(envelope.c_str()), envelope.length() + 1);
        envelope.clear();
    }
    yCTrace(MJPEGCARRIER, "Done compressing (height %d)", cinfo.image_height);
    while (cinfo.next_scanline < cinfo.image_height) {
        yCTrace(MJPEGCARRIER, "Writing row %d...", cinfo.next_scanline);
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
    std::string pathValue = n.getCarrierModifier("path");
    std::string target = "GET /?action=stream\n\n";
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

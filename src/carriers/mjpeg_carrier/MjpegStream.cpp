#include "MjpegStream.h"

#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>

#include <yarp/os/impl/Logger.h>

#include <jpeglib.h>
#include <setjmp.h>

using namespace yarp::os::impl;
using namespace yarp::sig;
using namespace std;

struct net_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};
typedef struct net_error_mgr *net_error_ptr;

typedef jpeg_source_mgr *net_src_ptr;

void init_net_source (j_decompress_ptr cinfo) {
  //net_src_ptr src = (net_src_ptr) cinfo->src;
}

void net_error_exit (j_common_ptr cinfo) {
  net_error_ptr myerr = (net_error_ptr) cinfo->err;
  (*cinfo->err->output_message) (cinfo);
  longjmp(myerr->setjmp_buffer, 1);
}

boolean fill_net_input_buffer (j_decompress_ptr cinfo)
{
  /* The whole JPEG data is expected to reside in the supplied memory
   * buffer, so any request for more data beyond the given buffer size
   * is treated as an error.
   */
  JOCTET *mybuffer = (JOCTET *) cinfo->client_data;
  fprintf(stderr, "JPEG data unusually large\n");
  /* Insert a fake EOI marker */
  mybuffer[0] = (JOCTET) 0xFF;
  mybuffer[1] = (JOCTET) JPEG_EOI;
  cinfo->src->next_input_byte = mybuffer;
  cinfo->src->bytes_in_buffer = 2;
  return TRUE;
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

void term_net_source (j_decompress_ptr cinfo)
{
}


void jpeg_net_src (j_decompress_ptr cinfo, char *buf, int buflen)
{
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


int MjpegStream::read(const Bytes& b) {
  if (remaining==0) {
    if (phase==1) {
      phase = 2;
      cursor = (char*)(img.getRawImage());
      remaining = img.getRawImageSize();
    } else {
      phase = 0;
    }
  }
  while (phase==0 && delegate->getInputStream().isOk()) {
    String s = "";
    do {
      s = NetType::readLine(delegate->getInputStream());
      //printf("Read %s\n", s.c_str());
    } while (s[0]!='-');
    s = NetType::readLine(delegate->getInputStream());
    if (s!="Content-Type: image/jpeg") {
      printf("Unknown content type - %s\n", s.c_str());
      continue;
    }
    s = NetType::readLine(delegate->getInputStream());
    Bottle b(s.c_str());
    if (b.get(0).asString()!="Content-Length:") {
      printf("Expected Content-Length: got - %s\n", b.get(0).asString().c_str());
      continue;
    }
    int len = b.get(1).asInt();
    //printf("Length is %d\n", len);
    do {
      s = NetType::readLine(delegate->getInputStream());
      //printf("Read %s\n", s.c_str());
    } while (s[0]!='\0');
    cimg.allocate(len);
    NetType::readFull(delegate->getInputStream(),cimg.bytes());
    struct jpeg_decompress_struct cinfo;
    struct net_error_mgr jerr;
    JOCTET error_buffer[4];
    cinfo.client_data = &error_buffer;
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = net_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
      YARP_ERROR(Logger::get(),"Skipping a problematic JPEG frame");
      jpeg_destroy_decompress(&cinfo);
      continue;
    }

    jpeg_create_decompress(&cinfo);
    jpeg_net_src(&cinfo,cimg.get(),cimg.length());
    jpeg_read_header(&cinfo, TRUE);
    jpeg_calc_output_dimensions(&cinfo);
    //printf("Got image %dx%d\n", cinfo.output_width,
    //cinfo.output_height);
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
    //printf("Read image!\n");
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    imgHeader.setFromImage(img);
    phase = 1;
    cursor = (char*)(&imgHeader);
    remaining = sizeof(imgHeader);
  }

  if (remaining>0) {
    int allow = remaining;
    if (b.length()<allow) {
      allow = b.length();
    }
    memcpy(b.get(),cursor,allow);
    cursor+=allow;
    remaining-=allow;
    //printf("returning %d bytes\n", allow);
    return allow;
  }
  return -1;
}


void MjpegStream::write(const Bytes& b) {
  delegate->getOutputStream().write(b);
}

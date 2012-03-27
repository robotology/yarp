
#include "BayerCarrier.h"

#include <yarp/sig/ImageDraw.h>

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::sig;

yarp::os::ConnectionReader& BayerCarrier::modifyIncomingData(yarp::os::ConnectionReader& reader) {

#if 0
  bool ok = in.read(reader);
  out.copy(in);
  out.pixel(0,0).r = 42;
  out.write(con.getWriter());
  return con.getReader();
#endif

#if 0
  bool ok = in.read(reader);
  out.copy(in);
  

  con.setTextMode(reader.isTextMode());
  Bottle b;
  b.read(reader);
  b.addString("BORK!");
  b.write(con.getWriter());
  printf("WRITING %s\n", b.toString().c_str());
  return con.getReader();
#endif

#if 1
  bool ok = in.read(reader);
  if (!ok) {
    fprintf(stderr,"BayerCarrier sadness\n");
    exit(1);
  }
  ImageNetworkHeader header_in_cmp;
  header_in_cmp.setFromImage(in);
  if (!need_reset) {
    need_reset = (0!=memcmp(&header_in_cmp,&header_in,sizeof(header_in)));
  }
  have_result = false;
  if (need_reset) {
    header_in.setFromImage(in);
    printf("Need reset.\n");
    processBuffered();
    need_reset = false;
  }
  Route r;
  local.reset(*this, NULL, r, sizeof(header)+image_data_len, false);
  consumed = 0;

  return local;
#endif
}


// A stub for bayer conversion
bool BayerCarrier::processCore(yarp::sig::ImageOf<PixelMono>& src,
			       yarp::sig::ImageOf<PixelRgb>& dest) {
  IMGFOR(src,x,y) {
    PixelMono& pi = src.pixel(x,y);
    PixelRgb& po = dest.pixel(x,y);
    po.r = pi;
    po.g = ((x+y)%8==0)?42:0;
    po.b = 0;
  }
  return true;
}

bool BayerCarrier::processBuffered() {
  if (!have_result) {
    printf("Copy-based conversion.\n");
    out.resize(in);
    processCore(in,out);
    header.setFromImage(out);
    image_data_len = (size_t)out.getRawImageSize();
  }
  have_result = true;
  return true;
}

bool BayerCarrier::processDirect(const yarp::os::Bytes& bytes) {
  if (have_result) {
    ACE_OS::memcpy(bytes.get(),out.getRawImage(),bytes.length());
    return true;
  }
  printf("Copyless conversion\n");
  ImageOf<PixelRgb> wrap;
  wrap.setQuantum(out.getQuantum());
  wrap.setExternal(bytes.get(),out.width(),out.height());
  processCore(in,wrap);
  return true;
}


ssize_t BayerCarrier::read(const yarp::os::Bytes& b) {
  // copy across small stuff - the image header
  if (consumed<sizeof(header)) {
    size_t len = b.length();
    if (len>sizeof(header)-consumed) {
      len = sizeof(header)-consumed;
    }
    ACE_OS::memcpy(b.get(),((char*)(&header))+consumed,len);
    consumed += len;
    return (ssize_t) len;
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
    ACE_OS::memcpy(b.get(),out.getRawImage()+consumed-sizeof(header),len);
    consumed += len;
    return (ssize_t) len;
  }
  return -1;
}

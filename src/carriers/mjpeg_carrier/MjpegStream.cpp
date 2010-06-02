#include "MjpegStream.h"

#include <yarp/os/Image.h>

using namespace yarp::os::impl;
using namespace yarp::sig;
using namespace std;

int MjpegStream::read(const Bytes& b) {
  if (sis.toString().length()==0) {
    // add a test image ...
    ImageOf<PixelRgb> img;
    img.resize(8,8);
    img.zero();
    BufferedConnectionWriter writer(false);
    img.write(writer);
    sis.add(writer.toString());
  }
  int result = sis.read(b);
  if (result>0) {
    printf("RETURNING %d bytes\n", result);
    return result;
  }
  return -1;
}


void MjpegStream::write(const Bytes& b) {
  delegate->getOutputStream().write(b);
}

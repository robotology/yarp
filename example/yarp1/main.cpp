
#include <yarp/YARPImage.h>

#include <yarp/sig/Image.h>

using namespace yarp::sig;

int main() {
  YARPImageOf<YarpPixelRGB> img1;
  ImageOf<PixelRgb> img2;
  printf("Created a YARP1 and YARP2 image\n");
  return 0;
}


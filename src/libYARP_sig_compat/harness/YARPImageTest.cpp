
/**
 *
 * This is a special set of tests, to see how well we support
 * the "Classic" YARP interface
 *
 */

#include <yarp/YARPImage.h>

#include <yarp/sig/Image.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::sig;
//using namespace yarp::os;

class YARPImageTest : public UnitTest {
public:
  virtual String getName() { return "YARPImageTest"; }

  virtual void testCreate() {
    report(0,"testing image creation...");
    YARPImageOf<YarpPixelRGB> image;
    image.Resize(256,128);
    checkEqual(image.GetWidth(),256,"check width");
    checkEqual(image.GetHeight(),128,"check height");
    long int total = 0;
    for (int x=0; x<image.GetWidth(); x++) {
      for (int y=0; y<image.GetHeight(); y++) {
	int v = (x+y)%17;
	image(x,y).r = v;
	total += v;
      }
    }
    for (int x=0; x<image.GetWidth(); x++) {
      for (int y=0; y<image.GetHeight(); y++) {
	total -= image(x,y).r;
      }
    }
    checkEqual(total,0,"assignment check");

    report(0,"checking new sig library compatibility");
    ImageOf<PixelRgb> image2;
    image2.resize(256,128);
    checkEqual(image2.width(),256,"check width");
    checkEqual(image2.height(),128,"check height");
  }

  virtual void runTests() {
    testCreate();
  }
};

static YARPImageTest theYARPImageTest;

UnitTest& getYARPImageTest() {
  return theYARPImageTest;
}


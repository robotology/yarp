
/**
 *
 * This is a special set of tests, to see how well we support
 * the "Classic" YARP interface
 *
 */

#include <yarp/sig/Image.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::sig;

class ImageTest : public UnitTest {
public:
  virtual String getName() { return "ImageTest"; }

  virtual void testCreate() {
    report(0,"testing image creation...");
    Image image;
    image.setPixelCode(YARP_PIXEL_RGB);
    image.resize(256,128);
    checkEqual(image.width(),256,"check width");
    checkEqual(image.height(),128,"check height");
    ImageOf<PixelInt> iint;
    iint.resize(256,128);
    long int total = 0;
    for (int x=0; x<iint.width(); x++) {
      for (int y=0; y<iint.height(); y++) {
	int v = (x+y)%65537;
	iint.pixel(x,y) = v;
	total += v;
      }
    }
    for (int x=0; x<iint.width(); x++) {
      for (int y=0; y<iint.height(); y++) {
	total -= iint.pixel(x,y);
      }
    }
    checkEqual(total,0,"assignment check");
  }

  virtual void runTests() {
    testCreate();
  }
};

static ImageTest theImageTest;

UnitTest& getImageTest() {
  return theImageTest;
}


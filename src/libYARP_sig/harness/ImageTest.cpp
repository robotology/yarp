
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
  }

  virtual void runTests() {
    testCreate();
  }
};

static ImageTest theImageTest;

UnitTest& getImageTest() {
  return theImageTest;
}


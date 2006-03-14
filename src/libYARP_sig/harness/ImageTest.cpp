
/**
 *
 * This is a special set of tests, to see how well we support
 * the "Classic" YARP interface
 *
 */

#include <yarp/sig/Image.h>
#include <yarp/os/Network.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/Port.h>
#include <yarp/os/Time.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::sig;
using namespace yarp::os;

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

  virtual void testTransmit() {
    report(0,"testing image transmission...");

    ImageOf<PixelRgb> img1;
    img1.resize(128,64);
    for (int x=0; x<img1.width(); x++) {
      for (int y=0; y<img1.height(); y++) {
	PixelRgb& pixel = img1.pixel(x,y);
	pixel.r = x;
	pixel.g = y;
	pixel.b = 42;
      }
    }

    PortReaderBuffer<ImageOf<PixelRgb> > buf;

    Port input, output;
    input.open("/in");
    output.open("/out");
    input.setReader(buf);

    output.addOutput(Contact::byName("/in").addCarrier("tcp"));
    Time::delay(0.2);

    report(0,"writing...");
    output.write(img1);
    output.write(img1);
    output.write(img1);
    report(0,"reading...");
    ImageOf<PixelRgb> *result = buf.read();

    checkTrue(result!=NULL,"got something check");
    if (result!=NULL) {
      checkEqual(img1.width(),result->width(),"width check");
      checkEqual(img1.height(),result->height(),"height check");
      if (img1.width()==result->width() &&
	  img1.height()==result->height()) {
	int mismatch = 0;
	for (int x=0; x<img1.width(); x++) {
	  for (int y=0; y<img1.height(); y++) {
	    PixelRgb& pix0 = img1.pixel(x,y);
	    PixelRgb& pix1 = result->pixel(x,y);
	    if (pix0.r!=pix1.r ||
		pix0.g!=pix1.g ||
		pix0.b!=pix1.b) {
	      mismatch++;
	    }
	  }
	}
	checkTrue(mismatch==0,"pixel match check");
      }
    }

   output.close();
   input.close();
  }

  virtual void runTests() {
    testCreate();
    bool netMode = Network::setLocalMode(true);
    testTransmit();
    Network::setLocalMode(netMode);
  }
};

static ImageTest theImageTest;

UnitTest& getImageTest() {
  return theImageTest;
}


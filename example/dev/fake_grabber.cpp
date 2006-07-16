#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

class FakeFrameGrabber : public IFrameGrabberImage, 
                         public yarp::dev::DeviceDriver {
private:
    int w, h;
public:
    FakeFrameGrabber() {
        h = w = 0;
    }

    bool open(int w, int h) {
        this->w = w;
        this->h = h;
        return w>0 && h>0;
    }

    virtual bool open(yarp::os::Searchable& config) { 
        // extract width and height configuration, if present
        // otherwise use 128x128
        int desiredWidth = config.check("w",Value(128)).asInt();
        int desiredHeight = config.check("h",Value(128)).asInt();
        return open(desiredWidth,desiredHeight);
    }

    virtual bool close() { 
        return true; // easy
    }

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
        Time::delay(0.5);  // simulate waiting for hardware to report
        image.resize(w,h);
        image.zero();
        return true;
    }

    virtual int height() const {
        return h;
    }

    virtual int width() const {
        return w;
    }
};


int main() {

  // give YARP a factory for creating instances of FakeFrameGrabber
  DriverCreator *fakey_factory = 
    new DriverCreatorOf<FakeFrameGrabber>("fakey",
					  "grabber",
					  "FakeFrameGrabber");
  Drivers::factory().add(fakey_factory); // hand factory over to YARP

  // use YARP to create and configure an instance of FakeFrameGrabber
  Property config("(device fakey) (w 640) (h 480)");
  PolyDriver dd(config);
  if (!dd.isValid()) {
    printf("Failed to create and configure the device\n");
    exit(1);
  }
  IFrameGrabberImage *grabberInterface;
  if (!dd.view(grabberInterface)) {
    printf("Failed to view device through IFrameGrabberImage interface\n");
    exit(1);
  }

  ImageOf<PixelRgb> img;
  grabberInterface->getImage(img);
  printf("Got a %dx%d image\n", img.width(), img.height());

  dd.close();

  return 0;
}

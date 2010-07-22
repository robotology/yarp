#include <yarp/os/impl/SizedWriter.h>
#include <yarp/sig/Image.h>

class WireImage {
private:
  yarp::sig::FlexImage img;
public:
  yarp::sig::FlexImage *checkForImage(yarp::os::impl::SizedWriter& writer);
};


#ifndef __YARP2_FRAME_GRABBER__
#define __YARP2_FRAME_GRABBER__

#include <yarp/dev/DeviceDriver.h>

namespace yarp{
  namespace dev {
    class FrameGrabber;
    class FrameGrabberOpenParameters;
  }
}

/** 
 * Common interface to a FrameGrabber.
 */
class yarp::dev::FrameGrabber: public DeviceDriver
{
 public:
   /**
    * Costructor
    */
   FrameGrabber(){}

   /** 
    * Destructor
    */
   virtual ~FrameGrabber(){}

   /**
    * Get a raw buffer from the frame grabber
    * @param pointer to the buffer to be filled (must be previously allocated)
    * @return true/false upon success/failure
    */
   virtual bool getBuffer(unsigned char *buffer)=0;

   /** 
    * Return the height of each frame.
    * @return image height
    */
   virtual int getHeight()=0;

   /** 
    * Return the width of each frame.
    * @return image width
    */
   virtual int getWidth()=0;
  
};

#endif

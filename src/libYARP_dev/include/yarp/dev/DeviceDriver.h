#ifndef __YARP2_DEVICEDRIVER__
#define __YARP2_DEVICEDRIVER__

namespace yarp {
  namespace dev {
    class DeviceDriver;
  }
}

/**
 * Interface implemented by all device drivers.
 * At the moment this is an empty class, in the future it might contain
 * methods common to all device drivers (e.g. serialization/deserialization)
 */
class yarp::dev::DeviceDriver 
{
 public:
   /**
    * Constructor
    */
   DeviceDriver(){};

   /**
    * Destructor
    */
   virtual ~DeviceDriver(){};

   /**
    * Open the DeviceDriver. Specific devices might define 
    * more suitable methods, with parameters.
    */
   bool open(){};

   /**
    * Close the DeviceDriver. 
    */
   virtual bool close()=0;
};

#endif

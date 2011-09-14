#include <yarp/os/NameSpace.h>
#include <yarp/os/Network.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>

using namespace yarp::os;

// holding action while traveling to make yarp build work again
// pardon the poor formatting
// may not be exactly right, no compiler to test...

bool NameSpace::checkNetwork() {
  Contact c = getNameServerContact();
  if (!c.isValid()) return false;
  Bottle cmd("[ping]"), reply;
  return NetworkBase::write(c,cmd,reply,true,false,5);
}

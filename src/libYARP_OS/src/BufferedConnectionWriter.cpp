
#include <yarp/BufferedConnectionWriter.h>
#include <yarp/os/Bottle.h>

using namespace yarp;
using namespace yarp::os;


bool BufferedConnectionWriter::convertTextMode() {
  if (isTextMode()) {
    String s = toString();
    Bottle b;
    b.fromBinary(s.c_str(),s.length());
    clear();
    b.write(*this);
  }
  return true;
}



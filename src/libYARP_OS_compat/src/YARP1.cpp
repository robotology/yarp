
#include <yarp/Logger.h>
#include <yarp/NameConfig.h>

// some small yarp 1 compatibility issues

using namespace yarp;


void set_yarp_debug(int x, int y) {
  if (x>10 || y>10) {
    Logger::get().setVerbosity(1);
  }
  YARP_DEBUG(Logger::get(), "got YARP1 call to set_yarp_debug");
}



/**
 * Gets the value of the YARP_ROOT symbol from the environment.
 * @return the YARP_ROOT as a null-terminated string or NULL on failure. Note
 * that the space required by the string is allocated by the OS and this function
 * is not reentrant (although it probably won't matter much).
 */
extern char * GetYarpRoot (void) {
  static String result("");
  if (result=="") {
    result = yarp::NameConfig::getEnv("YARP_ROOT");
  }
  if (result!="") {
    return (char *)result.c_str();
  }
  return NULL;
}



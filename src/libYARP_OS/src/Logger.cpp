
#include <yarp/Logger.h>

#include <ace/OS_NS_stdlib.h>
#include <ace/OS_NS_stdio.h>

using namespace yarp;

Logger Logger::root("yarp");

Logger& Logger::get() {
  return root;
}


void Logger::show(int level, const String& txt) {
  int inLevel = level;
  //ACE_OS::fprintf(stderr,"level %d txt %s\n", level, txt.c_str());
  if (verbose>0) {
    level = 10000;
  }
  if (parent == NULL) {
    if (level>=low) {
      ACE_OS::fprintf(stderr,"%s: %s\n",prefix.c_str(),txt.c_str());
	  ACE_OS::fflush(stderr);
    }
  } else {
    String more(prefix);
    more += ": ";
    more += txt;
    parent->show(inLevel,more);
  }
}


void Logger::exit(int level) {
  ACE_OS::exit(level);
}

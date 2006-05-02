
#include <yarp/Logger.h>
#include <yarp/NameClient.h>
#include <yarp/Companion.h>

#include <yarp/os/Network.h>

int main(int argc, char *argv[]) {
  // call the yarp standard companion
  ACE::init();
  int result = yarp::os::Network::main(argc,argv);
  ACE::fini();
  return result;
}



#include <yarp/FakeFace.h>
#include <yarp/Logger.h>
#include <yarp/Protocol.h>
#include <yarp/FakeTwoWayStream.h>

using namespace yarp;

static Logger log("FakeFace", Logger::get());

void FakeFace::open(const Address& address) {
  // happy to open without fuss
}

void FakeFace::close() {
}

InputProtocol *FakeFace::read() {
  log.fail("not yet implemented");
  return NULL;
}

OutputProtocol *FakeFace::write(const Address& address) {
  Protocol *prot = new Protocol(new FakeTwoWayStream());
  return prot;
}


#ifndef _YARP2_SHMEMCARRIER_
#define _YARP2_SHMEMCARRIER_

#include <yarp/AbstractCarrier.h>
#include <yarp/ShmemTwoWayStream.h>

namespace yarp {
  class ShmemCarrier;
}

class yarp::ShmemCarrier : public AbstractCarrier {
public:
  ShmemCarrier() {
  }

  virtual Carrier *create() {
    return new ShmemCarrier();
  }

  virtual String getName() {
    return "shmem";
  }

  virtual int getSpecifierCode() {
    return 2;
  }

  virtual bool requireAck() {
    return true;
  }

  virtual bool isConnectionless() {
    return false;
  }

  virtual bool checkHeader(const Bytes& header) {
    return getSpecifier(header)%16 == getSpecifierCode();
  }

  virtual void getHeader(const Bytes& header) {
    createStandardHeader(getSpecifierCode(), header);
  }

  virtual void setParameters(const Bytes& header) {
  }

  virtual void becomeShmem(Protocol& proto, bool sender) {
    YARP_ERROR(Logger::get(),"warning - SHMEM carrier is not YARP1 compatible yet");
    ShmemTwoWayStream *stream = new ShmemTwoWayStream();
    YARP_ASSERT(stream!=NULL);
    Address remote = proto.getStreams().getRemoteAddress();
    Address local = proto.getStreams().getLocalAddress();
    proto.takeStreams(NULL); // free up port from tcp
    try {
      Address base = sender?remote:local;
      // sad - YARP1 compatibility?
      base = Address("localhost",base.getPort()+1);
      stream->open(base,sender);
    } catch (IOException e) {
      delete stream;
      throw e;
    }
    proto.takeStreams(stream);
  }

  virtual void respondToHeader(Protocol& proto) {
    // i am the receiver
    becomeShmem(proto,false);
  }


  virtual void expectReplyToHeader(Protocol& proto) {
    // i am the sender
    becomeShmem(proto,true);
  }

};

#endif


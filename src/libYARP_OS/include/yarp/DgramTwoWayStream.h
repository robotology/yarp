#ifndef _YARP2_DGRAMTWOWAYSTREAM_
#define _YARP2_DGRAMTWOWAYSTREAM_

#include <yarp/TwoWayStream.h>
#include <yarp/IOException.h>
#include <yarp/ManagedBytes.h>

#include <ace/SOCK_Dgram.h>

namespace yarp {
  class DgramTwoWayStream;
}

/**
 * A stream abstraction for datagram communication.  It supports UDP and
 * MCAST.  This class is not concerned with making the stream reliable.
 */
class yarp::DgramTwoWayStream : public TwoWayStream, public InputStream, public OutputStream {

public:
  DgramTwoWayStream() {
    closed = false;
    reader = false;
    writer = false;
    dgram = NULL;
    happy = false;
  }

  virtual void open(const Address& remote);

  virtual void open(const Address& local, const Address& remote);

  virtual void join(const Address& group, bool sender);

  virtual ~DgramTwoWayStream();

  virtual InputStream& getInputStream() {
    return *this;
  }

  virtual OutputStream& getOutputStream() {
    return *this;
  }

  virtual const Address& getLocalAddress() {
    return localAddress;
  }

  virtual const Address& getRemoteAddress() {
    return remoteAddress;
  }

  virtual void interrupt();

  virtual void close();

  virtual int read(const Bytes& b);

  virtual void write(const Bytes& b);

  virtual void flush();

  virtual bool isOk();

  virtual void reset();

private:

  bool closed, reader, writer;
  ACE_SOCK_Dgram *dgram;
  ACE_INET_Addr localHandle, remoteHandle;
  Address localAddress, remoteAddress;
  ManagedBytes readBuffer, writeBuffer;
  int readAt, readAvail, writeAvail;
  bool happy;
};

#endif


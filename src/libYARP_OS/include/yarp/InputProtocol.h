#ifndef _YARP2_INPUTPROTOCOL_
#define _YARP2_INPUTPROTOCOL_

#include <yarp/Address.h>
#include <yarp/Route.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/InputStream.h>
#include <yarp/OutputStream.h>

namespace yarp {
  class InputProtocol;
  class OutputProtocol;
}

/**
 * The input side of an active connection between two ports.
 */
class yarp::InputProtocol {
public:
  virtual ~InputProtocol() {}

  // all may throw IOException

  virtual void open(const String& name) = 0;
  virtual void close() = 0;
  virtual void interrupt() = 0;

  virtual const Route& getRoute() = 0;

  virtual ConnectionReader& beginRead() = 0;
  virtual void endRead() = 0;

  virtual bool checkStreams() = 0;

  virtual void resetStreams() = 0;

  // some connections are capable of ping-ponging
  virtual OutputProtocol& getOutput() = 0;
  
  // direct access
  virtual OutputStream& getOutputStream() = 0;
  virtual InputStream& getInputStream() = 0;
  virtual const Address& getRemoteAddress() = 0;
};

#endif


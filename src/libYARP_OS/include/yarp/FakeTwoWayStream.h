#ifndef _YARP2_FAKETWOWAYSTREAM_
#define _YARP2_FAKETWOWAYSTREAM_

#include <yarp/TwoWayStream.h>
#include <yarp/StringInputStream.h>
#include <yarp/StringOutputStream.h>

namespace yarp {
  class FakeTwoWayStream;
}


/**
 * A dummy two way stream for testing purposes.
 */
class yarp::FakeTwoWayStream : public TwoWayStream {
public:
  FakeTwoWayStream(StringInputStream *target = NULL) : out(this) {
    this->target = target;
  }

  void setTarget(StringInputStream& target) {
    this->target = &target;
  }

  virtual InputStream& getInputStream() {
    return in;
  }

  virtual StringInputStream& getStringInputStream() {
    return in;
  }

  virtual OutputStream& getOutputStream() {
    return out;
  }

  virtual const Address& getLocalAddress() {
    return local;
  }

  virtual const Address& getRemoteAddress() {
    return remote;
  }

  virtual void close() {
    in.close();
    out.close();
  }

  virtual void apply(const Bytes& b) {
    if (target!=NULL) {
      target->add(b);
    }
  }

  void addInputText(const String& str) {
    in.add(str.c_str());
  }

  String getOutputText() {
    return out.toString();
  }

  String getInputText() {
    return in.toString();
  }

  virtual bool isOk() {
    return true;
  }

  virtual void reset() {
  }

private:

  class ActiveStringOutputStream : public StringOutputStream {
  public:
    ActiveStringOutputStream(FakeTwoWayStream *who) : owner(*who) {
    }

    virtual void write(const Bytes& b) {
      StringOutputStream::write(b);
      owner.apply(b);
    }
  private:
    FakeTwoWayStream& owner;
  };

  StringInputStream in;
  ActiveStringOutputStream out;
  Address local;
  Address remote;
  StringInputStream *target;
};

#endif

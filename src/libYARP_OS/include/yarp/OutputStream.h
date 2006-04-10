#ifndef _YARP2_OUTPUTSTREAM_
#define _YARP2_OUTPUTSTREAM_

#include <yarp/Bytes.h>
#include <yarp/String.h>

namespace yarp {
  class OutputStream;
}

/**
 * Simple specification of the minimum functions needed from output streams.
 * The streams could be TCP, UDP, MCAST, ...
 */
class yarp::OutputStream {
public:

  virtual ~OutputStream() { }

  // These should be called at the beginning and end of logical packets.
  // Streams are encouraged to handle errors and atomicity at the level of 
  // logical packets
  //virtual void beginPacket() {}
  //virtual void endPacket() {}

  virtual void write(char ch) { // throws
    write(Bytes(&ch,1));
  }

  virtual void write(const Bytes& b, int offset, int len) { // throws
    write(Bytes(b.get()+offset,len));
  }

  virtual void write(const Bytes& b) = 0;

  virtual void close() = 0;

  virtual void flush() {
  }

  virtual void writeLine(const String& data) {
    Bytes b((char*)(data.c_str()),data.length());
    write(b);
    write('\n');
  }

};

#endif

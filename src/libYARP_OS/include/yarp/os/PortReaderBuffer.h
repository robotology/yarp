#ifndef _YARP2_PORTREADERBUFFER_
#define _YARP2_PORTREADERBUFFER_

#include <yarp/os/PortReader.h>
#include <yarp/os/Port.h>

namespace yarp {
  namespace os {
    template <class T> class PortReaderBuffer;
  }
  class PortReaderBufferBase;
  class PortReaderBufferBaseCreator;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class yarp::PortReaderBufferBaseCreator {
public:
  virtual ~PortReaderBufferBaseCreator() {}

  virtual yarp::os::PortReader *create() = 0;
};

class yarp::PortReaderBufferBase {
public:
  PortReaderBufferBase(PortReaderBufferBaseCreator& creator) : 
    creator(creator) {
    init();
  }

  virtual ~PortReaderBufferBase();

  virtual yarp::os::PortReader *create() {
    return creator.create();
  }

  void release(yarp::os::PortReader *completed);

  bool check();

  virtual bool read(yarp::os::ConnectionReader& connection);

  void setAutoRelease(bool flag = true);

  yarp::os::PortReader *readBase();

protected:
  void init();

  PortReaderBufferBaseCreator& creator;
  void *implementation;
};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

/**
 * Buffer incoming data to a port.
 * An instance of this class can be associated with a Port by calling
 * attach().  From then on data that arrives to the Port will
 * be passed to this buffer, to be picked up at the user's leisure
 * by calling check() and read().  "T" should be a PortReader
 * class, such as Bottle.
 */
template <class T>
class yarp::os::PortReaderBuffer : public yarp::os::PortReader, private yarp::PortReaderBufferBaseCreator {
public:

  /**
   * Constructor.
   */
  PortReaderBuffer() : implementation(*this) {
  }

  /**
   * Check if data is available.
   * @return true iff data is available (i.e. a call to read() will return
   * immediately and successfully)
   */
  bool check() {
    return implementation.check();
  }

  /**
   * Wait for data.
   * @return pointer to data received on the port, or NULL on failure.
   */
  T *read() {
    return (T *)implementation.readBase();
  }

  /**
   * Attach this buffer to a particular port.  Data arriving to that
   * port will from now on be placed in this buffer.
   * @param port the port to attach to
   */
  void attach(Port& port) {
    port.setReader(*this);
  }


#ifndef DOXYGEN_SHOULD_SKIP_THIS
  /**
   * Reads objects from a network connection.
   * This method is called by a port when data is received.
   * @param connection an interface to the network connection for reading
   * @return true iff the object is successfully read
   */
  virtual bool read(ConnectionReader& connection) {
    return implementation.read(connection);
  }
  /**
   * Factory method.  New instances are created as needed to store incoming
   * data.
   *
   * @return new instance of the templated type.
   */
  virtual PortReader *create() {
    return new T;
  }

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

private:
  yarp::PortReaderBufferBase implementation;
};

#endif

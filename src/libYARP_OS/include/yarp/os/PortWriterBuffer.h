#ifndef _YARP2_PORTWRITERBUFFER_
#define _YARP2_PORTWRITERBUFFER_

#include <yarp/os/Portable.h>

namespace yarp {
  namespace os {
    class Port;
    template <class T> class PortWriterBuffer;
    class PortWriterBufferManager;
    template <class T> class PortWriterBufferAdaptor;
    class PortWriterBufferBase;
  }
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class yarp::os::PortWriterBufferManager {
public:
  virtual void onCompletion(void *tracker) = 0;
};

template <class T>
class yarp::os::PortWriterBufferAdaptor : public PortWriter {
public:
  PortWriterBufferManager& creator;
  T writer;
  void *tracker;

  PortWriterBufferAdaptor(PortWriterBufferManager& creator,
			  void *tracker) : 
    creator(creator), tracker(tracker) {}

  virtual bool write(ConnectionWriter& connection) {
    return writer.write(connection);
  }

  virtual void onCompletion() {
    writer.onCompletion();
    creator.onCompletion(tracker);
  }
};

class yarp::os::PortWriterBufferBase {
public:
  PortWriterBufferBase();

  virtual ~PortWriterBufferBase();

  virtual PortWriter *create(PortWriterBufferManager& man,
			     void *tracker) = 0;

  void *getContent();

  int getCount();

  void attach(Port& port);

  void write();


protected:

  void init();

private:
  void *implementation;
};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/


/**
 * Buffer outgoing data to a port.
 * An instance of this class can be associated with a Port by calling
 * attach().  "T" should be a PortWriter class, such as Bottle.
 */
template <class T>
class yarp::os::PortWriterBuffer : public PortWriterBufferBase {
public:

  //typedef T Type;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  virtual PortWriter *create(PortWriterBufferManager& man,
			     void *tracker) {
    return new PortWriterBufferAdaptor<T>(man,tracker);
  }
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

  /**
   * Get a free buffer to prepare for writing.
   * @return a free buffer.
   */
  T& get() {
    PortWriterBufferAdaptor<T> *content = (PortWriterBufferAdaptor<T>*)getContent();  // guaranteed to be non-NULL
    return content->writer;
  }

  /**
   * Check the number of buffers currently in use for communication.  
   * This may increase
   * as PortWriterBuffer::read is called.  It can decrease at any
   * time as buffers are successfully transmitted and made available
   * for reuse.
   * @return the number of buffers in use for communication.
   */
  int getCount() {
    return PortWriterBufferBase::getCount();
  }

  /**
   * Set the Port to which objects will be written.
   * @param port the Port to which objects will be written
   */
  void attach(Port& port) {
    PortWriterBufferBase::attach(port);
  }

  /**
   * Try to write the last buffer returned by PortWriterBuffer::get.
   */
  void write() {
    PortWriterBufferBase::write();
  }

};




#endif

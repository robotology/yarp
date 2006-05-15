// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
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
    PortReaderBufferBase(PortReaderBufferBaseCreator& creator, 
                         unsigned int maxBuffer) : 
        creator(creator), maxBuffer(maxBuffer) {
        init();
        allowReuse = true;
    }

    virtual ~PortReaderBufferBase();

    virtual yarp::os::PortReader *create() {
        return creator.create();
    }

    void release(yarp::os::PortReader *completed);

    bool check();

    virtual bool read(yarp::os::ConnectionReader& connection);

    void setAutoRelease(bool flag = true);

    void setAllowReuse(bool flag = true) {
        allowReuse = flag;
    }

    yarp::os::PortReader *readBase();

    unsigned int getMaxBuffer() {
        return maxBuffer;
    }

protected:
    void init();

    PortReaderBufferBaseCreator& creator;
    unsigned int maxBuffer;
    bool allowReuse;
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
     * @param maxBuffer Maximum number of buffers permitted (0 = no limit)
     */
    PortReaderBuffer(unsigned int maxBuffer = 0) : 
        implementation(*this,maxBuffer) {
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

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/



    /**
     * Factory method.  New instances are created as needed to store incoming
     * data.  By default, this just uses the default contructor - override
     * this if you need to do something fancier (such as allocating
     * a shared memory space).
     *
     * @return new instance of the templated type.
     */
    virtual PortReader *create() {
        return new T;
    }


private:
    yarp::PortReaderBufferBase implementation;
};

#endif

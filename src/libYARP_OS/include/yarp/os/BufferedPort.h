// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_BUFFEREDPORT_
#define _YARP2_BUFFEREDPORT_

#include <yarp/os/Contactable.h>
#include <yarp/os/Port.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/PortWriterBuffer.h>

namespace yarp {
    namespace os {
        template <class T> class BufferedPort;
    }
}

/**
 * A mini-server for performing network communication in the background.
 * If you are a beginner, you might want to use the Port class first,
 * and then come back to BufferedPort when you want to send and receive
 * messages in the background without having to stop your processing.
 * This is convenient, but requires a little care to understand the
 * life-cycle of objects written to and read from the network
 * (see BufferedPort::read and BufferedPort::write).
 */
template <class T>
class yarp::os::BufferedPort : public Contactable, 
            public TypedReader<T>, public TypedReaderCallback<T>
{
public:

    typedef T ContentType;

    BufferedPort() {
        port.enableBackgroundWrite(true);
        reader.attach(port);
        writer.attach(port);
        reader.delegate(*this);
    }

    virtual ~BufferedPort() {
        port.close();
    }


    // documentation provided in Contactable
    virtual bool open(const char *name) {
        return port.open(name);
    }

    // documentation provided in Contactable
    virtual bool open(const Contact& contact, bool registerName = true) {
        return port.open(contact,registerName);
    }

    // documentation provided in Contactable
    virtual bool addOutput(const char *name) {
        return port.addOutput(name);
    }

    // documentation provided in Contactable
    virtual bool addOutput(const char *name, const char *carrier) {
        return port.addOutput(name,carrier);
    }

    // documentation provided in Contactable
    virtual bool addOutput(const Contact& contact){
        return port.addOutput(contact);
    }

    // documentation provided in Contactable
    virtual void close() {
        port.close();
    }

    // documentation provided in Contactable
    virtual Contact where() {
        return port.where();
    }

    /**
     * Access the object which will be transmitted by the next call to 
     * BufferedPort::write.
     * The object can safely be modified by the user of this class, to
     * prepare it.  Extra objects will be created or reused as 
     * necessary depending on the state of communication with the
     * output(s) of the port.
     * @return the next object that will be written
     */
    T& prepare() {
        return writer.get();
    }

    /**
     * Write the current object being returned by BufferedPort::prepare.
     * That object should no longer be touched by the user of this class --
     * it is now owned by the communications system.  The BufferedPort::prepare
     * method should be called again to get a fresh (or reused) object
     * guaranteed to be not in use by the communications system.
     */
    void write() {
        writer.write();
    }

    virtual T *read(bool shouldWait=true,
                    bool forceStrict=false) {
        return reader.read();
    }

    virtual T *lastRead() {
        return reader.lastRead();
    }

	virtual bool isClosed() {
		return reader.isClosed();
	}

    virtual void onRead(T& datum) {
        // override this to do something
    }

private:
    Port port;
    PortReaderBuffer<T> reader;
    PortWriterBuffer<T> writer;
};

#endif

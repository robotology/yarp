// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_PORTWRITER_
#define _YARP2_PORTWRITER_

#include <yarp/os/ConnectionWriter.h>

namespace yarp {
    namespace os {
        class PortWriter;
    }
}

/**
 * Interface implemented by all objects that can write themselves to
 * the network, such as Bottle objects.
 * @see Port, PortReader
 */
class yarp::os::PortWriter {
public:
    virtual ~PortWriter() {}

    /**
     * Write this object to a network connection.  
     * Override this for your particular class.
     * Be aware that
     * depending on the nature of the connections a port has, and what
     * protocol they use, and how efficient the YARP implementation is,
     * this method may be called once, twice, or many times, as the
     * result of a single call to Port::write
     * @param connection an interface to the network connection for writing
     * @return true iff the object is successfully written
     */
    virtual bool write(ConnectionWriter& connection) = 0;

    /**
     * This is called when the port has finished all writing operations.
     */
    virtual void onCompletion() {}
};

#endif

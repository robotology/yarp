/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_PORTREADERBUFFER_H
#define YARP_OS_PORTREADERBUFFER_H

#include <yarp/os/PortReader.h>
#include <yarp/os/Port.h>
#include <yarp/os/Thread.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/LocalReader.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/TypedReader.h>
#include <yarp/os/TypedReaderCallback.h>
#include <yarp/os/TypedReaderThread.h>
#include <yarp/os/PortReaderBufferBase.h>
#include <yarp/os/PortReaderBufferBaseCreator.h>

#include <cstdio>

namespace yarp {
    namespace os {
        template <typename T> class PortReaderBuffer;
    }
}


/**
 * Buffer incoming data to a port.
 * An instance of this class can be associated with a Port by calling
 * attach().  From then on data that arrives to the Port will
 * be passed to this buffer, to be picked up at the user's leisure
 * by calling check() and read().  "T" should be a PortReader
 * class, such as Bottle.
 */
template <typename T>
class yarp::os::PortReaderBuffer :
        public yarp::os::TypedReader<T>,
        public yarp::os::LocalReader<T>,
        public yarp::os::PortReaderBufferBaseCreator
{
public:

    /**
     * Constructor.
     *
     * @param maxBuffer Maximum number of buffers permitted (0 = no limit)
     */
    PortReaderBuffer(unsigned int maxBuffer = 0);

    /**
     * Destructor.
     */
    virtual ~PortReaderBuffer();

    void detach();

    // documented in TypedReader
    virtual void setStrict(bool strict = true) override;

    /**
     * Check if data is available.
     *
     * @return true iff data is available (i.e. a call to read() will return
     *         immediately and successfully)
     */
    bool check();

    // documented in TypedReader
    virtual int getPendingReads() override;

    // documented in TypedReader
    virtual T *read(bool shouldWait=true) override;

    // documented in TypedReader
    virtual void interrupt() override;

    // documented in TypedReader
    virtual T *lastRead() override;

    /**
     * Attach this buffer to a particular port.
     *
     * Data arriving to that port will from now on be placed in this buffer.
     *
     * @param port the port to attach to
     */
    void attach(Port& port);

    // documented in TypedReader
    virtual void useCallback(TypedReaderCallback<T>& callback) override;

    // documented in TypedReader
    virtual void disableCallback() override;

    /**
     * Reads objects from a network connection.
     * This method is called by a port when data is received.
     *
     * @param connection an interface to the network connection for reading
     * @return true iff the object is successfully read
     */
    virtual bool read(ConnectionReader& connection);

    /**
     * Factory method.  New instances are created as needed to store incoming
     * data.  By default, this just uses the default contructor - override
     * this if you need to do something fancier (such as allocating
     * a shared memory space).
     *
     * @return new instance of the templated type.
     */
    virtual PortReader *create() override;

    // documented in TypedReader
    virtual void setReplier(PortReader& reader) override;

    virtual bool getEnvelope(PortReader& envelope);

    // documented in TypedReader
    virtual bool isClosed() override;

    // documented in TypedReader
    virtual ConstString getName() const override;

    // documented in LocalReader
    virtual bool acceptObject(T* obj, PortWriter *wrapper) override;

    // documented in LocalReader
    virtual bool forgetObject(T *obj, yarp::os::PortWriter *wrapper) override;

    // documented in TypedReader
    virtual void *acquire() override;

    // documented in TypedReader
    virtual void release(void *handle) override;

    // documented in TypedReader
    virtual void setTargetPeriod(double period) override;

private:
    yarp::os::PortReaderBufferBase implementation;
    bool autoDiscard;
    T *last;
    T *default_value;
    TypedReaderThread<T> *reader;
};


#include <yarp/os/PortReaderBuffer-inl.h>

#endif // YARP_OS_PORTREADERBUFFER_H

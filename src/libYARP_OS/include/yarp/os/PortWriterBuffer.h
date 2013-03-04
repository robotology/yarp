// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

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
        class PortWriterWrapper;
    }
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class yarp::os::PortWriterBufferManager {
public:
    virtual ~PortWriterBufferManager() {}

    virtual void onCompletion(void *tracker) = 0;
};

class yarp::os::PortWriterWrapper : public yarp::os::PortWriter {
public:
    virtual PortWriter *getInternal() = 0;
};

template <class T>
class yarp::os::PortWriterBufferAdaptor : public PortWriterWrapper {
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

    virtual void onCommencement() {
        writer.onCommencement();
    }

    virtual PortWriter *getInternal() {
        return &writer;
    }
};

class YARP_OS_API yarp::os::PortWriterBufferBase {
public:
    PortWriterBufferBase();

    virtual ~PortWriterBufferBase();

    virtual PortWriterWrapper *create(PortWriterBufferManager& man,
                                      void *tracker) = 0;

    void *getContent();

    bool releaseContent();

    int getCount();

    void attach(Port& port);

    void write(bool strict);


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
    virtual PortWriterWrapper *create(PortWriterBufferManager& man,
                                      void *tracker) {
        return new PortWriterBufferAdaptor<T>(man,tracker);
    }
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

    /**
     * Access the object which will be transmitted by the next call to
     * PortWriterBuffer::write.
     * The object can safely be modified by the user of this class, to
     * prepare it.  Extra objects will be created or reused as
     * necessary depending on the state of communication with the
     * output(s) of the port.
     * @return the next object that will be written
     */
    T& prepare() {
        return get();
    }

    /**
     *
     * Give the last prepared object back to YARP without writing it.
     *
     * @return true if there was a prepared object to return.
     *
     */
    bool unprepare() {
        return releaseContent();
    }

    /**
     * A synonym of PortWriterBuffer::prepare.
     * @return the next object that will be written
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
    void write(bool forceStrict=false) {
        PortWriterBufferBase::write(forceStrict);
    }

};


#endif

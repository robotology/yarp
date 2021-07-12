/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_PORTWRITERBUFFER_H
#define YARP_OS_PORTWRITERBUFFER_H

#include <yarp/os/Portable.h>
#include <yarp/os/PortWriterBufferBase.h>

namespace yarp {
namespace os {

class Port;

#ifndef DOXYGEN_SHOULD_SKIP_THIS

template <class T>
class PortWriterBufferAdaptor : public PortWriterWrapper
{
public:
    PortWriterBufferManager& creator;
    T writer;
    void* tracker;

    PortWriterBufferAdaptor(PortWriterBufferManager& creator,
                            void* tracker) :
            creator(creator),
            tracker(tracker)
    {
    }

    bool write(ConnectionWriter& connection) const override
    {
        return writer.write(connection);
    }

    void onCompletion() const override
    {
        writer.onCompletion();
        creator.onCompletion(tracker);
    }

    void onCommencement() const override
    {
        writer.onCommencement();
    }

    PortWriter* getInternal() override
    {
        return &writer;
    }
};

#endif // DOXYGEN_SHOULD_SKIP_THIS


/**
 * Buffer outgoing data to a port.
 * An instance of this class can be associated with a Port by calling
 * attach().  "T" should be a PortWriter class, such as Bottle.
 */
template <class T>
class PortWriterBuffer : public PortWriterBufferBase
{
public:
    /**
     * Access the object which will be transmitted by the next call to
     * PortWriterBuffer::write.
     * The object can safely be modified by the user of this class, to
     * prepare it.  Extra objects will be created or reused as
     * necessary depending on the state of communication with the
     * output(s) of the port.
     * @return the next object that will be written
     */
    T& prepare()
    {
        return get();
    }

    /**
     *
     * Give the last prepared object back to YARP without writing it.
     *
     * @return true if there was a prepared object to return.
     *
     */
    bool unprepare()
    {
        return releaseContent();
    }

    /**
     * A synonym of PortWriterBuffer::prepare.
     * @return the next object that will be written
     */
    T& get()
    {
        PortWriterBufferAdaptor<T>* content = (PortWriterBufferAdaptor<T>*)getContent(); // guaranteed to be non-NULL
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
    int getCount()
    {
        return PortWriterBufferBase::getCount();
    }

    /**
     * Set the Port to which objects will be written.
     * @param port the Port to which objects will be written
     */
    void attach(Port& port)
    {
        PortWriterBufferBase::attach(port);
    }

    /**
     * Try to write the last buffer returned by PortWriterBuffer::get.
     */
    void write(bool forceStrict = false)
    {
        PortWriterBufferBase::write(forceStrict);
    }

    /**
     * Wait until any pending writes are done.
     */
    void waitForWrite()
    {
        PortWriterBufferBase::waitForWrite();
    }

    virtual PortWriterWrapper* create(PortWriterBufferManager& man,
                                      void* tracker) override
    {
        return new PortWriterBufferAdaptor<T>(man, tracker);
    }
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_PORTWRITERBUFFER_H

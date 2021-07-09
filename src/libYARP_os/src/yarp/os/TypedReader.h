/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_TYPEDREADER_H
#define YARP_OS_TYPEDREADER_H


#include <yarp/os/TypedReaderCallback.h>


namespace yarp {
namespace os {

/**
 * A base class for sources of typed data.  This could be a
 * BufferedPort or a PortReaderBuffer.
 */
template <class T>
class TypedReader
{
public:
    /**
     * Call this to strictly keep all messages, or allow old ones
     * to be quietly dropped.  If you don't call this,
     * old messages will be quietly dropped.
     * @param strict True to keep all messages until they are read,
     * false to drop old messages when a new one comes in.
     * @warning If you can't read them as fast as the come in, watch out.
     */
    virtual void setStrict(bool strict = true) = 0;

    /**
     * Read an available object from the port.
     * @param shouldWait true if the method should wait until an object is
     *                   available, false if the call should return immediately
     *                   if no message is available
     * @return A pointer to an object read from the port, or nullptr if
     *         none is available and waiting was not requested.
     *         This object is owned by the communication system and should not
     *         be deleted by the user.
     *         The object is available to the user until the next call to one of
     *         the read methods, after which it should not be accessed again.
     */
    virtual T* read(bool shouldWait = true) = 0;

    /**
     * Abort any read operation currently in progress.
     */
    virtual void interrupt() = 0;

    /**
     * Get the last data returned by read()
     *
     * @return pointer to last data returned by read(), or nullptr on failure.
     */
    virtual T* lastRead() = 0;

    /**
     * Returns whether the port associated with this reader has been closed
     *
     * @return true if Port associated with this reader has been closed
     */
    virtual bool isClosed() = 0;

    /**
     * Set an object whose onRead method will be called when data is available.
     *
     * @param callback the object whose onRead method will be called with data
     */
    virtual void useCallback(TypedReaderCallback<T>& callback) = 0;

    /**
     * Remove a callback set up with useCallback()
     */
    virtual void disableCallback() = 0;

    /**
     * Check how many messages are waiting to be read.
     * @return number of pending messages
     */
    virtual int getPendingReads() = 0;

    /**
     * Destructor.
     */
    virtual ~TypedReader() = default;

    /**
     * Get name of port being read from
     * @return name of port
     */
    virtual std::string getName() const = 0;


    /**
     * If a message is received that requires a reply, use this handler.
     *
     * No buffering happens.
     *
     * @param reader the handler to use
     */
    virtual void setReplier(PortReader& reader) = 0;


    /**
     * Take control of the last object read.
     * YARP will not reuse that object until it is explicitly released
     * by the user.  Be careful - if you acquire objects without
     * releasing, YARP will keep making new ones to store incoming
     * messages.  So you need to release all objects you acquire
     * eventually to avoid running out of memory.
     *
     * @return the handle to call release() with in order to give YARP
     * back control of the last object read.
     */
    virtual void* acquire() = 0;


    /**
     * Return control to YARP of an object previously taken control of
     * with the acquire() method.
     *
     * @param handle the pointer returned by acquire() when control of
     * the object was taken by the user.
     */
    virtual void release(void* handle) = 0;


    /**
     * Try to provide data periodically.  If no new data arrives
     * in a given period, repeat the last data received (if any).
     * Similarly, the port should not pass on data more frequently
     * than the given period.
     *
     * @param period target period in (fractional) seconds.
     */
    virtual void setTargetPeriod(double period) = 0;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_TYPEDREADER_H

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_PORTCOREUNIT_H
#define YARP_OS_IMPL_PORTCOREUNIT_H

#include <yarp/os/Name.h>
#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/ThreadImpl.h>

#include <string>

namespace yarp {
namespace os {
namespace impl {

/**
 * This manages a single threaded resource related to a single
 * input or output connection.
 */

class PortCoreUnit :
        public ThreadImpl
{
public:
    /**
     * Constructor.
     *
     * @param owner the port we call home
     * @param index an id for this connection
     */
    PortCoreUnit(PortCore& owner, int index) :
            owner(owner),
            doomed(false),
            hasMode(false),
            pupped(false),
            index(index)
    {
    }

    /**
     * Destructor.
     */
    virtual ~PortCoreUnit()
    {
    }

    /**
     * @return true if this is an input connection
     */
    virtual bool isInput()
    {
        return false;
    }

    /**
     * @return true if this is an output connection
     */
    virtual bool isOutput()
    {
        return false;
    }

    /**
     * @return true if no further communication will occur using this
     * connection
     */
    virtual bool isFinished()
    {
        return false;
    }

    /**
     * @return the route (sender, receiver, carrier) associated with this
     * connection
     */
    virtual Route getRoute()
    {
        return Route("null", "null", "null");
    }

    /**
     * @return true if setDoomed() has been called to request that
     * this connection be shut down
     */
    bool isDoomed()
    {
        return doomed;
    }

    /**
     * Request that this connection be shut down as soon as possible.
     */
    void setDoomed()
    {
        doomed = true;
    }

    /**
     * Send a message on the connection.  Does nothing for an input connection.
     *
     * @param writer the message the send
     * @param reader an optional receiver for a reply
     * @param callback an optional receiver for PortWriter::onCommencement
     * and PortWriter::onCompletion calls
     * @param tracker an opaque pointer to an object doing memory management
     * for the message. Once this pointer is passed to the send() method,
     * it is "owned" until returned by a future call to send(), or by
     * a call to takeTracker().
     * @param envelope some optional text to pass along with the message
     * @param waitAfter true if we should wait for the send to complete
     * before the method returns
     * @param waitBefore true if we should wait for any in-progress send
     * to complete before stating this one
     * @parm gotReply if non-nullptr, this variable will be set to true if
     * a reply was received
     *
     * @return nullptr, or a tracker for a previous send operation that
     * is no longer in progress. The tracker is an opaque pointer passed
     * in via a previous call to send().  Once it is returned, it is
     * the caller's responsibility to manage any memory associated
     * with the message.
     */
    virtual void* send(const yarp::os::PortWriter& writer,
                       yarp::os::PortReader* reader,
                       const yarp::os::PortWriter* callback,
                       void* tracker,
                       const std::string& envelope,
                       bool waitAfter = true,
                       bool waitBefore = true,
                       bool* gotReply = nullptr)
    {
        // do nothing
        YARP_UNUSED(writer);
        YARP_UNUSED(reader);
        YARP_UNUSED(callback);
        YARP_UNUSED(envelope);
        YARP_UNUSED(waitAfter);
        YARP_UNUSED(waitBefore);
        YARP_UNUSED(gotReply);
        return tracker;
    }

    /**
     * Reacquire a tracker previously passed via send(). This method
     * may need to wait a send operation to complete before the tracker
     * can be safely accessed.
     *
     * @return the opaque tracker pointer passed to a previous call to
     * send(), or nullptr if there is no such tracker.  Once the tracker
     * has been returned, calling this method again will return nullptr.
     */
    virtual void* takeTracker()
    {
        return nullptr;
    }

    /**
     * @return true if the connection is currently in use.
     */
    virtual bool isBusy()
    {
        return false;
    }

    /**
     * Interrupt the connection.
     *
     * @return true on success.
     */
    virtual bool interrupt()
    {
        return false;
    }

    /**
     * Check the carrier used for the connection, and see if it has
     * a "log" modifier.  If so, set the connection "mode" from that
     * modifier.  A connection for which a mode is set will behave
     * differently, sending log information on port activity rather
     * than regular payload data.
     */
    void setMode()
    {
        Name name(getRoute().getCarrierName() + std::string("://test"));
        mode = name.getCarrierModifier("log", &hasMode);
    }

    /**
     * @return the index identifier supplied to the constructor.
     */
    int getIndex()
    {
        return index;
    }

    /**
     * Read the "mode" of the connection - basically, whether it
     * is used for logging or not.
     *
     * @param hasMode optional variable to store whether a mode has been set
     *
     * @return the connection mode, or the empty string if there is none
     */
    std::string getMode(bool* hasMode = nullptr)
    {
        if (hasMode != nullptr) {
            *hasMode = this->hasMode;
        }
        return (this->hasMode) ? mode : "";
    }

    /**
     * @return true if this connection was created by a `publisherUpdate`
     * message to the port's administrative interface.  We need to know
     * this so we can remove it if a succeeding call to `publisherUpdate`
     * does not mention the target this connection is serving.
     */
    bool isPupped() const
    {
        return pupped;
    }

    /**
     * @return the target this connection is serving, if created via a
     * `publisherUpdate` message to the port's administrative
     * interface.  We need to know this so we can remove it if a
     * succeeding call to `publisherUpdate` does not mention the
     * target this connection is serving.
     */
    std::string getPupString() const
    {
        return pupString;
    }

    /**
     * Tag this connection as having been created by a
     * `publisherUpdate` message to the port's administrative
     * interface, and record the name of the target it services
     * exactly as described in that message. We need to know this so
     * we can remove the connection if a succeeding call to
     * `publisherUpdate` does not mention the target this connection
     * is serving.
     */
    void setPupped(const std::string& pupString)
    {
        pupped = true;
        this->pupString = pupString;
    }

    /**
     * Set arbitrary parameters for this connection.
     * @param params the parameters to set
     */
    virtual void setCarrierParams(const yarp::os::Property& params)
    {
        YARP_UNUSED(params);
    }

    /**
     * @param [out]params parameters set by setCarrierParams()
     */
    virtual void getCarrierParams(yarp::os::Property& params)
    {
        YARP_UNUSED(params);
    }


protected:
    /**
     * @return the port to which this connection belongs
     */
    PortCore& getOwner()
    {
        return owner;
    }

private:
    PortCore& owner;       ///< the port to which this connection belongs
    bool doomed;           ///< whether the connection should shut down ASAP
    std::string mode;      ///< the logging mode of the connection
    bool hasMode;          ///< whether the logging mode has been set
    bool pupped;           ///< whether the connection was made by `publisherUpdate`
    int index;             ///< an ID assigned to the connection
    std::string pupString; ///< the target of the connection if created by `publisherUpdate`
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PORTCOREUNIT_H

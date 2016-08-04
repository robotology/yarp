/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_CONTACTABLE_H
#define YARP_OS_CONTACTABLE_H

#include <yarp/os/Contact.h>
#include <yarp/os/PortWriter.h>
#include <yarp/os/PortReport.h>
#include <yarp/os/Mutex.h>

/**
 * The main, catch-all namespace for YARP.
 */
namespace yarp {
    /**
     * An interface to the operating system, including Port based communication.
     */
    namespace os {
        class Contactable;
        class Property;
    }
}

/**
 * An abstract port.  Anything that can be expressed via a Contact.
 *
 */
class YARP_OS_API yarp::os::Contactable {

public:

    /**
     * Destructor.
     */
    virtual ~Contactable();

    /**
     * Start port operation, with automatically-chosen network parameters.
     * The port is assigned an arbitrary name, and allocated
     * network resources,
     * by communicating with the YARP name server.
     * @return true iff the port started operation successfully and is now
     * visible on the YARP network
     */
    bool open();

    /**
     * Start port operation, with a specific name, with
     * automatically-chosen network parameters.  The port is
     * registered with the given name, and allocated network
     * resources, by communicating with the YARP name server.  @return
     * true iff the port started operation successfully and is now
     * visible on the YARP network
     */
    virtual bool open(const ConstString& name) = 0;

    /**
     * Start port operation with user-chosen network parameters.
     * Contact information is supplied by the user rather than
     * the name server.  If the Contact
     * information is incomplete, the name server is used
     * to complete it (set registerName to false if you don't
     * want name server help).
     * @return true iff the port started operation successfully and is now
     * visible on the YARP network
     */
    virtual bool open(const Contact& contact, bool registerName = true) = 0;

    /**
     * Add an output connection to the specified port.
     * @param name the name of the target port
     * @return true iff the connection is successfully created
     */
    virtual bool addOutput(const ConstString& name) = 0;

    /**
     * Add an output connection to the specified port, using a specified
     * carrier
     * @param name the name of the target port
     * @param carrier the carrier (network protocol) to use, e.g. "tcp", "udp", "mcast", "text", ...
     * @return true iff the connection is successfully created
     */
    virtual bool addOutput(const ConstString& name, const ConstString& carrier) = 0;

    /**
     * Add an output connection to the specified port, using specified
     * network parameters.
     * @param contact information on how to reach the target
     * @return true iff the connection is successfully created
     */
    virtual bool addOutput(const Contact& contact) = 0;

    /**
     * Stop port activity.
     */
    virtual void close() = 0;

    /**
     * Interrupt any current reads or writes attached to the port.
     * This is useful prior to calling close(), if there are multiple
     * threads operating on the port.  Any reads or writes
     * after the call to interrupt() will fail - unless resume() is called.
     */
    virtual void interrupt() = 0;

    /**
     *
     * Put the port back in an operative state after interrupt() has
     * been called.
     *
     */
    virtual void resume() = 0;

    /**
     * Returns information about how this port can be reached.
     * @return network parameters for this port
     */
    virtual Contact where() const = 0;

    /**
     * Get name of port.
     * @return name of port
     */
    virtual ConstString getName() const;


    /**
     * Every message sent by a port can have some information added
     * to it called an envelope (this could be a timestamp, for example).
     * You can set that envelope with this method.
     *
     * Currently, for proper operation, the envelope must serialize
     * correctly in text-mode (even if you do not explicitly use
     * text-mode connections).  The Bottle or Stamp class are good
     * choices.
     *
     * @param envelope some information to add to the next message sent
     *
     * @return true iff write was successful
     *
     */
    virtual bool setEnvelope(PortWriter& envelope) = 0;


    /**
     * Every message sent by a port can have some information added to
     * it called an envelope (this could be a timestamp, for example).
     * If a port has received a message with such information, you can
     * extract that envelope with this method.
     *
     * You must be sure to match the type of your envelope for
     * getEnvelope with whatever is being sent using setEnvelope.
     * The Bottle class can read any kind of envelope (but is
     * not specialized to be efficient).
     *
     * @param envelope recipient for envelope information for last
     * message received by port.
     *
     * @return true iff read was successful
     *
     */
    virtual bool getEnvelope(PortReader& envelope) = 0;


    /**
     * Determine how many connections are arriving into this port.
     * In other words, how many other ports have this port listed as
     * an output?
     * @return number of input connections
     */
    virtual int getInputCount() = 0;

    /**
     * Determine how many output connections this port has.
     * @return number of output connections
     */
    virtual int getOutputCount() = 0;

    /**
     *
     * Get information on the state of the port - connections etc.
     * PortReport::report will be called once for each connection to
     * the port that exists right now.  To request callbacks for
     * any future connections/disconnections, use the setReporter
     * method instead.
     *
     * @param reporter callback for port event/state information
     *
     */
    virtual void getReport(PortReport& reporter) = 0;


    /**
     *
     * Set a callback to be called upon any future connections and
     * disconnections to/from the port.  To get information on
     * the current connections that exist, use the getReport method
     * instead.
     *
     * @param reporter callback for port event/state information
     *
     */
    virtual void setReporter(PortReport& reporter) = 0;

    /**
     * Remove the callback which is called upon any future connections and
     * disconnections to/from the port.
     */
    virtual void resetReporter() = 0;

    /**
     * Report whether the port is currently writing data.
     * @result true iff the port is writing in the background.
     */
    virtual bool isWriting() = 0;


    /**
     * Set an external reader for port data.
     * @param reader the external reader to use
     */
    virtual void setReader(PortReader& reader) = 0;

    /**
     * Set an external reader for unrecognized administrative port messages.
     * @param reader the external reader to use
     */
    virtual void setAdminReader(PortReader& reader) = 0;

    /**
     *
     * Configure the port to allow or forbid inputs.  By default,
     * ports allow anything.
     *
     * @param expectInput set to true if this port will be used for input
     *
     */
    virtual void setInputMode(bool expectInput) = 0;

    /**
     *
     * Configure the port to allow or forbid outputs.  By default,
     * ports allow anything.
     *
     * @param expectOutput set to true if this port will be used for output
     *
     */
    virtual void setOutputMode(bool expectOutput) = 0;

    /**
     *
     * Configure the port to be RPC only.  By default all ports can be
     * used for RPC or streaming communication.
     *
     * @param expectRpc set to true if this port will be used for RPC only
     *
     */
    virtual void setRpcMode(bool expectRpc) = 0;

    /**
     *
     * Get the type of data the port has committed to send/receive.  Not 
     * all ports commit to a particular type of data.
     *
     * @return type of data to expect/provide
     *
     */
    virtual Type getType() = 0;

    /**
     *
     * Commit the port to a particular type of data.
     *
     * @param typ the type of data the port will send/receive
     *
     */
    virtual void promiseType(const Type& typ) = 0;

    /**
     *
     * Access unstructured port properties.
     *
     * @param readOnly set this if you won't be modifying the properties.
     *
     * @return the port properties (or NULL if readOnly and none have been set)
     *
     */
    virtual Property *acquireProperties(bool readOnly) = 0;

    /**
     *
     * End access unstructured port properties.
     *
     * @param prop the port property object provided by acquireProperties()
     *
     */
    virtual void releaseProperties(Property *prop) = 0;

    /**
     *
     * Choose whether to prepend a node name (if one is available) to
     * the port's name. Node names are set using yarp::os::Node.
     *
     * @param flag true if the node name should be added to port names
     *
     */
    virtual void includeNodeInName(bool flag) = 0;

    /**
     *
     * Shorthand for setInputMode(true), setOutputMode(false), setRpcMode(false)
     *
     */
    void setReadOnly() {
        setInputMode(true);
        setOutputMode(false);
        setRpcMode(false);
    }

    /**
     *
     * Shorthand for setInputMode(false), setOutputMode(true), setRpcMode(false)
     *
     */
    void setWriteOnly() {
        setInputMode(false);
        setOutputMode(true);
        setRpcMode(false);
    }

    /**
     *
     * Shorthand for setInputMode(true), setOutputMode(false), setRpcMode(true)
     *
     */
    void setRpcServer() {
        setInputMode(true);
        setOutputMode(false);
        setRpcMode(true);
    }

    /**
     *
     * Shorthand for setInputMode(false), setOutputMode(true), setRpcMode(true)
     *
     */
    void setRpcClient() {
        setInputMode(false);
        setOutputMode(true);
        setRpcMode(true);
    }

    /**
     *
     * Add a lock to use when invoking callbacks.  mutex.lock() will
     * be called before and mutex.unlock() will be called after the
     * callback.  This applies at least to callbacks set by setReader and
     * setAdminReader, and in future may apply to other callbacks.
     *
     * @param mutex the lock to use. If NULL, a mutex will be allocated
     * internally by the port, and destroyed with the port.
     *
     */
    virtual bool setCallbackLock(yarp::os::Mutex *mutex = NULL) = 0;

    /**
     *
     * Remove a lock on callbacks added with setCallbackLock()
     *
     */
    virtual bool removeCallbackLock() = 0;

    /**
     *
     * Lock callbacks until unlockCallback() is called.  Has no effect
     * if no lock has been set via a call to setCallbackLock().  Will
     * block if callbacks are already locked.
     * @return true if callbacks were locked
     *
     */
    virtual bool lockCallback() = 0;

    /**
     *
     * Try to lock callbacks until unlockCallback() is called.  Has no effect
     * if no lock has been set via a call to setCallbackLock().  Returns
     * immediately.
     *
     * @return true if callbacks were locked by this call, false if they
     * were already locked.
     *
     */
    virtual bool tryLockCallback() = 0;

    /**
     *
     * Unlock callbacks.
     *
     */
    virtual void unlockCallback() = 0;
};

#endif // YARP_OS_CONTACTABLE_H

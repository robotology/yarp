/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_PORT_H
#define YARP_OS_PORT_H

#include <yarp/os/api.h>
#include <yarp/os/Portable.h>
#include <yarp/os/UnbufferedContactable.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/PortWriter.h>
#include <yarp/os/PortReaderCreator.h>

// Defined in this file:
#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
namespace yarp { namespace os { class Port; }}
#endif // YARP_NO_DEPRECATED

namespace yarp {
namespace os {

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
/**
 * @ingroup comm_class
 *
 * A mini-server for network communication.
 * It maintains a dynamic collection of incoming and outgoing connections.
 * Data coming from any incoming connection can be received by calling
 * Port::read.
 * Calls to Port::write result in data being sent to all the outgoing
 * connections.
 *
 * Communication with Port objects is blocking by default, this means
 * that YARP will not drop messages and timing between readers and senders
 * will be coupled (notice that messages may still get dropped in the network
 * if you use an unreliable protocol like UDP).
 *
 * @sa For help sending and receiving data in the background and decouple
 *     timing, see BufferedPort, or PortReaderBuffer and PortWriterBuffer.
 *
 * For examples and help, see:
 * @li @ref what_is_a_port
 * @li @ref note_ports
 * @li @ref port_expert
 * @li @ref yarp_buffering
 *
 * @deprecated since YARP 3.0.0. YARP no longer supports Port and BufferedPort.
 *             Use yarp::os::Publisher or yarp::os::Subscriber instead.
 */
class
YARP_OS_DEPRECATED_API_MSG("YARP no longer supports Port and BufferedPort. Use yarp::os::Publisher or yarp::os::Subscriber instead.")
Port : public UnbufferedContactable
{

public:
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.72
    using Contactable::open;
#endif // YARP_NO_DEPRECATED

    /**
     * Constructor.
     *
     * The port begins life in a dormant state.
     * Call Port::open to start things happening.
     */
    Port();

    /**
     * Destructor.
     */
    virtual ~Port();

    // Documented in Contactable
    bool open(const ConstString& name) override;

    // Documented in Contactable
    bool open(const Contact& contact, bool registerName = true) override;

    /**
     * Open a port wrapping an existing port.
     */
    bool sharedOpen(Port& port);

    /**
     * Start port without making it accessible from the network.
     * The port is unusable for many purposes, other than sending
     * out data to specific outputs added with addOutput().
     *
     * @param name an arbitrary name that connections from this
     *             port will claim to be from.
     * @return true on success
     */
    bool openFake(const ConstString& name);

    // Documented in Contactable
    bool addOutput(const ConstString& name) override;

    // Documented in Contactable
    bool addOutput(const ConstString& name, const ConstString& carrier) override;

    // Documented in Contactable
    bool addOutput(const Contact& contact) override;

    // Documented in Contactable
    void close() override;

    // Documented in Contactable
    virtual void interrupt() override;

    // Documented in Contactable
    virtual void resume() override;

    // Documented in Contactable
    Contact where() const override;

    // Documented in UnbufferedContactable
    bool write(PortWriter& writer, PortWriter *callback = nullptr) const override;

    // Documented in UnbufferedContactable
    bool write(PortWriter& writer, PortReader& reader,
               PortWriter *callback = nullptr) const override;

    // Documented in UnbufferedContactable
    bool read(PortReader& reader, bool willReply = false) override;

    // Documented in UnbufferedContactable
    bool reply(PortWriter& writer) override;

    // Documented in UnbufferedContactable
    bool replyAndDrop(PortWriter& writer) override;

    // Documented in Contactable
    void setReader(PortReader& reader) override;

    // Documented in Contactable
    virtual void setAdminReader(PortReader& reader) override;

    /**
     * Set a creator for readers for port data.
     *
     * Every port that input is received from will be automatically
     * given its own reader.  Handy if you care about the identity
     * of the receiver.
     *
     * @param creator the "factory" for creating PortReader object
     */
    void setReaderCreator(PortReaderCreator& creator);

    /**
     * control whether writing from this port is done in the background.
     *
     * @param backgroundFlag if true, calls to Port::write should return
     *                       immediately
     */
    void enableBackgroundWrite(bool backgroundFlag);


    // Documented in Contactable
    bool isWriting() override;

    // Documented in Contactable
    virtual bool setEnvelope(PortWriter& envelope) override;

    // Documented in Contactable
    virtual bool getEnvelope(PortReader& envelope) override;

    // Documented in Contactable
    virtual int getInputCount() override;

    // Documented in Contactable
    virtual int getOutputCount() override;

    // Documented in Contactable
    virtual void getReport(PortReport& reporter) override;

    // Documented in Contactable
    virtual void setReporter(PortReport& reporter) override;

    // Documented in Contactable
    virtual void resetReporter() override;

    /**
     * Turn on/off "admin" mode.
     *
     * With admin mode on, messages received from this port will be treated by
     * other ports as administrative messages, rather than as data to pass on to
     * the owner of the port.
     *
     * @param adminMode administration flag
     */
    void setAdminMode(bool adminMode = true);

    // Documented in Contactable
    void setInputMode(bool expectInput) override;

    // Documented in Contactable
    void setOutputMode(bool expectOutput) override;

    // Documented in Contactable
    void setRpcMode(bool expectRpc) override;

    /**
     * Set a timeout on network operations.
     *
     * Use a negative number to wait forever (this is the default behavior).
     * Call this method early, since it won't affect connections that have
     * already been made.
     *
     * @param timeout number of seconds to wait for individual network
     *                operations to complete (negative to wait forever).
     */
    bool setTimeout(float timeout);

    /**
     * Set whether the port should issue messages about its operations.
     *
     * @param level verbosity level, -1 to inhibit messages.
     */
    void setVerbosity(int level);

    /**
     * Get port verbosity level.
     *
     * @return port verbosity level.
     */
    int getVerbosity();

    // Documented in Contactable
    virtual Type getType() override;

    // Documented in Contactable
    virtual void promiseType(const Type& typ) override;

    // Documented in Contactable
    virtual Property *acquireProperties(bool readOnly) override;

    // Documented in Contactable
    virtual void releaseProperties(Property *prop) override;

    // Documented in Contactable
    virtual void includeNodeInName(bool flag) override;

    /**
     * Check if the port has been opened.
     */
    bool isOpen() const;

    // Documented in Contactable
    virtual bool setCallbackLock(yarp::os::Mutex *mutex = nullptr) override;

    // Documented in Contactable
    virtual bool removeCallbackLock() override;

    // Documented in Contactable
    virtual bool lockCallback() override;

    // Documented in Contactable
    virtual bool tryLockCallback() override;

    // Documented in Contactable
    virtual void unlockCallback() override;

private:
    void *implementation;
    bool owned;

    void *needImplementation() const;

    bool open(const Contact& contact,
              bool registerName,
              const char *fakeName);

};

#endif // YARP_NO_DEPRECATED

} // namespace os
} // namespace yarp

#endif // YARP_OS_PORT_H

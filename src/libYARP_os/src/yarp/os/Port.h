/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_PORT_H
#define YARP_OS_PORT_H

#include <yarp/os/api.h>

#include <yarp/os/PortReader.h>
#include <yarp/os/PortReaderCreator.h>
#include <yarp/os/PortWriter.h>
#include <yarp/os/Portable.h>
#include <yarp/os/UnbufferedContactable.h>


namespace yarp {
namespace os {

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
 */
class YARP_os_API Port : public UnbufferedContactable
{

public:
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
    ~Port() override;

    // Documented in Contactable
    bool open(const std::string& name) override;

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
    bool openFake(const std::string& name);

    // Documented in Contactable
    bool addOutput(const std::string& name) override;

    // Documented in Contactable
    bool addOutput(const std::string& name, const std::string& carrier) override;

    // Documented in Contactable
    bool addOutput(const Contact& contact) override;

    // Documented in Contactable
    void close() override;

    // Documented in Contactable
    void interrupt() override;

    // Documented in Contactable
    void resume() override;

    // Documented in Contactable
    Contact where() const override;

    // Documented in UnbufferedContactable
    bool write(const PortWriter& writer,
               const PortWriter* callback = nullptr) const override;

    // Documented in UnbufferedContactable
    bool write(const PortWriter& writer,
               PortReader& reader,
               const PortWriter* callback = nullptr) const override;

    // Documented in UnbufferedContactable
    bool read(PortReader& reader, bool willReply = false) override;

    // Documented in UnbufferedContactable
    bool reply(PortWriter& writer) override;

    // Documented in UnbufferedContactable
    bool replyAndDrop(PortWriter& writer) override;

    // Documented in Contactable
    void setReader(PortReader& reader) override;

    // Documented in Contactable
    void setAdminReader(PortReader& reader) override;

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
    bool setEnvelope(PortWriter& envelope) override;

    // Documented in Contactable
    bool getEnvelope(PortReader& envelope) override;

    // Documented in Contactable
    int getInputCount() override;

    // Documented in Contactable
    int getOutputCount() override;

    // Documented in Contactable
    void getReport(PortReport& reporter) override;

    // Documented in Contactable
    void setReporter(PortReport& reporter) override;

    // Documented in Contactable
    void resetReporter() override;

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

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
    /**
     * Set whether the port should issue messages about its operations.
     *
     * @param level verbosity level, -1 to inhibit messages.
     * @deprecated since YARP 3.4
     */
    YARP_DEPRECATED_MSG("Use LogComponent instead")
    void setVerbosity(int level);

    /**
     * Get port verbosity level.
     *
     * @return port verbosity level.
     * @deprecated since YARP 3.4
     */
    YARP_DEPRECATED
    int getVerbosity();
#endif

    // Documented in Contactable
    Type getType() override;

    // Documented in Contactable
    void promiseType(const Type& typ) override;

    // Documented in Contactable
    Property* acquireProperties(bool readOnly) override;

    // Documented in Contactable
    void releaseProperties(Property* prop) override;

    // Documented in Contactable
    void includeNodeInName(bool flag) override;

    /**
     * Check if the port has been opened.
     */
    bool isOpen() const;

#ifndef YARP_NO_DEPRECATED // Since YARP 3.3
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    // Documented in Contactable
    YARP_DEPRECATED_MSG("Use setCallbackLock with std::mutex instead")
    bool setCallbackLock(yarp::os::Mutex* mutex) override;
YARP_WARNING_POP
#endif

    // Documented in Contactable
    bool setCallbackLock(std::mutex* mutex = nullptr) override;

    // Documented in Contactable
    bool removeCallbackLock() override;

    // Documented in Contactable
    bool lockCallback() override;

    // Documented in Contactable
    bool tryLockCallback() override;

    // Documented in Contactable
    void unlockCallback() override;

private:
    void* implementation;
    bool owned;

    void* needImplementation() const;

    bool open(const Contact& contact,
              bool registerName,
              const char* fakeName);
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_PORT_H

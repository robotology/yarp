/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_ABSTRACTCONTACTABLE_H
#define YARP_OS_ABSTRACTCONTACTABLE_H

#include <yarp/os/Port.h>
#include <yarp/os/UnbufferedContactable.h>

namespace yarp {
namespace os {

/**
 * A default implementation of an abstract port. The class hierarchy
 * for ports is misshapen because we want to keep it SWIG compatible,
 * and SWIG can't yet produce good translations of code that in C++
 * looks like multiple inheritance but is in fact just implementing
 * multiple interfaces.  There's work in SWIG on supporting
 * interfaces, so this can be simplified a lot at that point.
 *
 */
class YARP_os_API AbstractContactable : public UnbufferedContactable
{

public:
    /**
     * Get the concrete Port being used for communication.
     */
    virtual Port& asPort() = 0;

    /**
     * Get the concrete Port being used for communication, const version.
     */
    virtual const Port& asPort() const = 0;

    // Documented in Contactable
    bool open(const std::string& name) override;

    // Documented in Contactable
    bool open(const Contact& contact, bool registerName = true) override;

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

    // Documented in Contactable
    std::string getName() const override;

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

    // Documented in Contactable
    bool isWriting() override;

    // Documented in Contactable
    void setReader(PortReader& reader) override;

    // Documented in Contactable
    void setAdminReader(PortReader& reader) override;

    // Documented in Contactable
    void setInputMode(bool expectInput) override;

    // Documented in Contactable
    void setOutputMode(bool expectOutput) override;

    // Documented in Contactable
    void setRpcMode(bool expectRpc) override;

    // Documented in Contactable
    Type getType() override;

    // Documented in Contactable
    void promiseType(const Type& typ) override;

    // Documented in Contactable
    Property* acquireProperties(bool readOnly) override;

    // Documented in Contactable
    void releaseProperties(Property* prop) override;

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
    void includeNodeInName(bool flag) override;

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
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_ABSTRACTCONTACTABLE_H

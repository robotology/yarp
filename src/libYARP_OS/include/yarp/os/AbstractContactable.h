/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_ABSTRACTCONTACTABLE_H
#define YARP_OS_ABSTRACTCONTACTABLE_H

#include <yarp/os/UnbufferedContactable.h>
#include <yarp/os/Port.h>


// Defined in this file:
namespace yarp { namespace os { class AbstractContactable; }}


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
class YARP_OS_API AbstractContactable : public UnbufferedContactable
{

public:
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.72
    using Contactable::open;
#endif // YARP_NO_DEPRECATED

    /**
     * Get the concrete Port being used for communication.
     */
    virtual Port& asPort() = 0;

    /**
     * Get the concrete Port being used for communication, const version.
     */
    virtual const Port& asPort() const = 0;

    // Documented in Contactable
    virtual bool open(const std::string& name) override;

    // Documented in Contactable
    virtual bool open(const Contact& contact, bool registerName = true) override;

    // Documented in Contactable
    virtual bool addOutput(const std::string& name) override;

    // Documented in Contactable
    virtual bool addOutput(const std::string& name, const std::string& carrier) override;

    // Documented in Contactable
    virtual bool addOutput(const Contact& contact) override;

    // Documented in Contactable
    virtual void close() override;

    // Documented in Contactable
    virtual void interrupt() override;

    // Documented in Contactable
    virtual void resume() override;

    // Documented in Contactable
    virtual Contact where() const override;

    // Documented in Contactable
    virtual std::string getName() const override;

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

    // Documented in Contactable
    virtual bool isWriting() override;

    // Documented in Contactable
    virtual void setReader(PortReader& reader) override;

    // Documented in Contactable
    virtual void setAdminReader(PortReader& reader) override;

    // Documented in Contactable
    virtual void setInputMode(bool expectInput) override;

    // Documented in Contactable
    virtual void setOutputMode(bool expectOutput) override;

    // Documented in Contactable
    virtual void setRpcMode(bool expectRpc) override;

    // Documented in Contactable
    virtual Type getType() override;

    // Documented in Contactable
    virtual void promiseType(const Type& typ) override;

    // Documented in Contactable
    virtual Property* acquireProperties(bool readOnly) override;

    // Documented in Contactable
    virtual void releaseProperties(Property* prop) override;

    // Documented in UnbufferedContactable
    virtual bool write(const PortWriter& writer,
                       const PortWriter* callback = nullptr) const override;

    // Documented in UnbufferedContactable
    virtual bool write(const PortWriter& writer,
                       PortReader& reader,
                       const PortWriter* callback = nullptr) const override;

    // Documented in UnbufferedContactable
    virtual bool read(PortReader& reader, bool willReply = false) override;

    // Documented in UnbufferedContactable
    virtual bool reply(PortWriter& writer) override;

    // Documented in UnbufferedContactable
    virtual bool replyAndDrop(PortWriter& writer) override;

    // Documented in Contactable
    virtual void includeNodeInName(bool flag) override;

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
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_ABSTRACTCONTACTABLE_H

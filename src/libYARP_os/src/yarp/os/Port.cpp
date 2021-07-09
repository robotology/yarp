/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Port.h>

#include <yarp/conf/system.h>
#include <yarp/conf/environment.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Network.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/PortCoreAdapter.h>

using namespace yarp::os::impl;
using namespace yarp::os;

namespace {
YARP_OS_LOG_COMPONENT(PORT, "yarp.os.Port")
} // namespace

void* Port::needImplementation() const
{
    if (implementation != nullptr) {
        return implementation;
    }
    Port* self = const_cast<Port*>(this);
    self->implementation = new yarp::os::impl::PortCoreAdapter(*self);
    yCAssert(PORT, self->implementation != nullptr);
    self->owned = true;
    return self->implementation;
}

// implementation is a PortCoreAdapter
#define IMPL() (*reinterpret_cast<yarp::os::impl::PortCoreAdapter*>(needImplementation()))

Port::Port() :
        implementation(nullptr),
        owned(false)
{
}

Port::~Port()
{
    if (implementation != nullptr) {
        close();
        if (owned) {
            delete (static_cast<PortCoreAdapter*>(implementation));
        }
        implementation = nullptr;
        owned = false;
    }
}

bool Port::sharedOpen(Port& port)
{
    close();
    if (owned) {
        delete (static_cast<PortCoreAdapter*>(implementation));
    }
    implementation = port.implementation;
    owned = false;
    return true;
}

bool Port::openFake(const std::string& name)
{
    return open(Contact(name), false, name.c_str());
}

bool Port::open(const std::string& name)
{
    return open(Contact(name));
}

bool Port::open(const Contact& contact, bool registerName)
{
    return open(contact, registerName, nullptr);
}

bool Port::open(const Contact& contact, bool registerName, const char* fakeName)
{
    Contact contact2 = contact;

    if (!NetworkBase::initialized()) {
        yCError(PORT, "YARP not initialized; create a yarp::os::Network object before using ports");
        return false;
    }

    std::string n = contact2.getName();

    NameConfig conf;
    std::string nenv = std::string("YARP_RENAME") + conf.getSafeString(n);
    std::string rename = yarp::conf::environment::get_string(nenv);
    if (!rename.empty()) {
        n = rename;
        contact2.setName(n);
    }

    bool local = false;
    if (n.empty() && contact2.getPort() <= 0) {
        local = true;
        registerName = false;
        n = "...";
    }

    NestedContact nc(n);
    if (!nc.getNestedName().empty()) {
        if (nc.getNodeName().empty()) {
            Nodes& nodes = NameClient::getNameClient().getNodes();
            nodes.requireActiveName();
            std::string node_name = nodes.getActiveName();
            if (!node_name.empty()) {
                n = n + node_name;
            }
        }
    }

    PortCoreAdapter* currentCore = &(IMPL());
    if (currentCore != nullptr) {
        currentCore->active = false;
        if (!n.empty() && (n[0] != '/' || currentCore->includeNode) && n[0] != '=' && n != "..." && n.substr(0, 3) != "...") {
            if (fakeName == nullptr) {
                Nodes& nodes = NameClient::getNameClient().getNodes();
                std::string node_name = nodes.getActiveName();
                if (!node_name.empty()) {
                    n = (n[0] == '/' ? "" : "/") + n + "@" + node_name;
                }
            }
        }
    }
    if (!n.empty() && n[0] != '/' && n[0] != '=' && n != "..." && n.substr(0, 3) != "...") {
        if (fakeName == nullptr) {
            yCError(PORT, "Port name '%s' needs to start with a '/' character", n.c_str());
            return false;
        }
    }
    if (!n.empty() && n != "..." && n[0] != '=' && n.substr(0, 3) != "...") {
        if (fakeName == nullptr) {
            std::string prefix = yarp::conf::environment::get_string("YARP_PORT_PREFIX");
            if (!prefix.empty()) {
                n = prefix + n;
                contact2.setName(n);
            }
        }
    }
    if (currentCore != nullptr) {
        NestedContact nc;
        nc.fromString(n);
        if (!nc.getNestedName().empty()) {
            if (nc.getCategory().empty()) {
                // we need to add in a category
                std::string cat;
                if (currentCore->commitToRead) {
                    cat = "-";
                } else if (currentCore->commitToWrite) {
                    cat = "+";
                }
                if (!cat.empty()) {
                    if (currentCore->commitToRpc) {
                        cat += "1";
                    }
                    contact2.setName(nc.getNestedName() + cat + "@" + nc.getNodeName());
                } else {
                    yCError(PORT, "Error: Port '%s' is not committed to being either an input or output port.", n.c_str());
                    yCError(PORT, "YARP does not mind, but we are trying to register with a name server that does.");
                    yCError(PORT, "You can call Port::setWriteOnly() or Port::setReadOnly(), OR rename the port.");
                    NestedContact nc2 = nc;
                    nc2.setCategoryWrite();
                    yCError(PORT, "For an output port, call it: %s (+ adds data)", nc2.toString().c_str());
                    nc2.setCategoryRead();
                    yCError(PORT, "For an input port, call it: %s (- takes data)", nc2.toString().c_str());
                    return false;
                }
            }
        }
    }

    // Allow for open() to be called safely many times on the same Port
    if ((currentCore != nullptr) && currentCore->isOpened()) {
        auto* newCore = new PortCoreAdapter(*this);
        yCAssert(PORT, newCore != nullptr);
        // copy state that should survive in a new open()
        if (currentCore->checkPortReader() != nullptr) {
            newCore->configReader(*(currentCore->checkPortReader()));
        }
        if (currentCore->checkAdminPortReader() != nullptr) {
            newCore->configAdminReader(*(currentCore->checkAdminPortReader()));
        }
        if (currentCore->checkReadCreator() != nullptr) {
            newCore->configReadCreator(*(currentCore->checkReadCreator()));
        }
        if (currentCore->checkWaitAfterSend() >= 0) {
            newCore->configWaitAfterSend(currentCore->checkWaitAfterSend() != 0);
        }
        if (currentCore->haveCallbackLock) {
            newCore->configCallbackLock(currentCore->recCallbackLock);
        }
        close();
        if (owned) {
            delete (static_cast<PortCoreAdapter*>(implementation));
        }
        implementation = newCore;
        owned = true;
        currentCore = newCore;
        currentCore->active = false;
    }

    PortCoreAdapter& core = IMPL();

    core.openable();

    if (NetworkBase::localNetworkAllocation() && contact2.getPort() <= 0) {
        yCDebug(PORT, "local network allocation needed");
        local = true;
    }

    bool success = true;
    Contact address(contact2.getName(),
                    contact2.getCarrier(),
                    contact2.getHost(),
                    contact2.getPort());
    address.setNestedContact(contact2.getNested());

    core.setReadHandler(core);
    if (contact2.getPort() > 0 && !contact2.getHost().empty()) {
        registerName = false;
    }

    std::string ntyp = getType().getNameOnWire();
    if (ntyp.empty()) {
        NestedContact nc;
        nc.fromString(n);
        if (!nc.getTypeName().empty()) {
            ntyp = nc.getTypeName();
        }
    }
    if (ntyp.empty()) {
        ntyp = getType().getName();
    }
    if (!ntyp.empty()) {
        NestedContact nc;
        nc.fromString(contact2.getName());
        nc.setTypeName(ntyp);
        contact2.setNestedContact(nc);
        if (getType().getNameOnWire() != ntyp) {
            core.promiseType(Type::byNameOnWire(ntyp.c_str()));
        }
    }

    if (registerName && !local) {
        address = NetworkBase::registerContact(contact2);
    }

    core.setControlRegistration(registerName);
    success = (address.isValid() || local) && (fakeName == nullptr);

    if (success) {
        // create a node if needed
        Nodes& nodes = NameClient::getNameClient().getNodes();
        nodes.prepare(address.getRegName());
    }

    // If we are a service client, go ahead and connect
    if (success) {
        NestedContact nc;
        nc.fromString(address.getName());
        if (!nc.getNestedName().empty()) {
            if (nc.getCategory() == "+1") {
                addOutput(nc.getNestedName());
            }
        }
    }

    std::string blame = "invalid address";
    if (success) {
        success = core.listen(address, registerName);
        blame = "address conflict";
        if (success) {
            success = core.start();
            blame = "manager did not start";
        }
    }
    if (success) {
        address = core.getAddress();
        if (registerName && local) {
            contact2.setSocket(address.getCarrier(),
                               address.getHost(),
                               address.getPort());
            contact2.setName(address.getRegName());
            Contact newName = NetworkBase::registerContact(contact2);
            core.resetPortName(newName.getName());
            address = core.getAddress();
        } else if (core.getAddress().getRegName().empty() && !registerName) {
            core.resetPortName(core.getAddress().toURI(false));
            core.setName(core.getAddress().getRegName());
        }

        if (address.getRegName().empty()) {
            yCInfo(PORT,
                   "Anonymous port active at %s",
                   address.toURI().c_str());
        } else {
            yCInfo(PORT,
                   "Port %s active at %s",
                   address.getRegName().c_str(),
                   address.toURI().c_str());
        }
    }

    if (fakeName != nullptr) {
        success = core.manualStart(fakeName);
        blame = "unmanaged port failed to start";
    }

    if (!success) {
        yCError(PORT, "Port %s failed to activate%s%s (%s)",
                (address.isValid() ? (address.getRegName().c_str()) : (contact2.getName().c_str())),
                (address.isValid() ? " at " : ""),
                (address.isValid() ? address.toURI().c_str() : ""),
                blame.c_str());
    }

    if (success) {
        // create a node if needed
        Nodes& nodes = NameClient::getNameClient().getNodes();
        nodes.add(*this);
    }

    if (success && currentCore != nullptr) {
        currentCore->active = true;
    }
    return success;
}

bool Port::addOutput(const std::string& name)
{
    return addOutput(Contact(name));
}

bool Port::addOutput(const std::string& name, const std::string& carrier)
{
    return addOutput(Contact(name, carrier));
}

void Port::close()
{
    if (!owned) {
        return;
    }

    Nodes& nodes = NameClient::getNameClient().getNodes();
    nodes.remove(*this);

    PortCoreAdapter& core = IMPL();
    core.finishReading();
    core.finishWriting();
    core.close();
    core.join();
    core.active = false;

    // In fact, open flag means "ever opened", so don't reset it
    // core.setOpened(false);
}

void Port::interrupt()
{
    Nodes& nodes = NameClient::getNameClient().getNodes();
    nodes.remove(*this);

    PortCoreAdapter& core = IMPL();
    core.interrupt();
}

void Port::resume()
{
    PortCoreAdapter& core = IMPL();
    if (!core.isInterrupted()) {
        // prevent resuming when the port is not interrupted
        return;
    }
    core.resumeFull();
    Nodes& nodes = NameClient::getNameClient().getNodes();
    nodes.add(*this);
}


Contact Port::where() const
{
    PortCoreAdapter& core = IMPL();
    return core.getAddress();
}


bool Port::addOutput(const Contact& contact)
{
    PortCoreAdapter& core = IMPL();
    if (core.commitToRead) {
        return false;
    }
    if (core.isInterrupted()) {
        return false;
    }
    core.alertOnWrite();
    std::string name;
    if (contact.getPort() <= 0) {
        name = contact.toString();
    } else {
        name = contact.toURI();
    }
    if (!core.isListening()) {
        return core.addOutput(name, nullptr, nullptr, true);
    }
    Contact me = where();
    return NetworkBase::connect(me.getName(), name);
}


bool Port::write(const PortWriter& writer, const PortWriter* callback) const
{
    PortCoreAdapter& core = IMPL();
    if (core.isInterrupted()) {
        return false;
    }
    core.alertOnWrite();
    bool result = false;
    //WritableAdapter adapter(writer);
    result = core.send(writer, nullptr, callback);
    //writer.onCompletion();
    if (!result) {
        if (callback != nullptr) {
            callback->onCompletion();
        } else {
            writer.onCompletion();
        }
        // leave result false
    }
    return result;
}

bool Port::write(const PortWriter& writer,
                 PortReader& reader,
                 const PortWriter* callback) const
{
    PortCoreAdapter& core = IMPL();
    if (core.isInterrupted()) {
        return false;
    }
    core.alertOnRpc();
    core.alertOnWrite();
    bool result = false;
    result = core.send(writer, &reader, callback);
    if (!result) {
        if (callback != nullptr) {
            callback->onCompletion();
        } else {
            writer.onCompletion();
        }
        // leave result false
    }
    return result;
}

bool Port::read(PortReader& reader, bool willReply)
{
    if (!isOpen()) {
        return false;
    }
    PortCoreAdapter& core = IMPL();
    if (willReply) {
        core.alertOnRpc();
    }
    core.alertOnRead();
    if (core.isInterrupted()) {
        return false;
    }
    return core.read(reader, willReply);
}


bool Port::reply(PortWriter& writer)
{
    PortCoreAdapter& core = IMPL();
    return core.reply(writer, false, core.isInterrupted());
}

bool Port::replyAndDrop(PortWriter& writer)
{
    PortCoreAdapter& core = IMPL();
    return core.reply(writer, true, core.isInterrupted());
}


void Port::setReader(PortReader& reader)
{
    PortCoreAdapter& core = IMPL();
    core.alertOnRead();
    core.configReader(reader);
}

void Port::setAdminReader(PortReader& reader)
{
    PortCoreAdapter& core = IMPL();
    core.configAdminReader(reader);
}


void Port::setReaderCreator(PortReaderCreator& creator)
{
    PortCoreAdapter& core = IMPL();
    core.alertOnRead();
    core.configReadCreator(creator);
}


void Port::enableBackgroundWrite(bool backgroundFlag)
{
    PortCoreAdapter& core = IMPL();
    core.configWaitAfterSend(!backgroundFlag);
}


bool Port::isWriting()
{
    PortCoreAdapter& core = IMPL();
    return core.isWriting();
}


bool Port::setEnvelope(PortWriter& envelope)
{
    PortCoreAdapter& core = IMPL();
    return core.setEnvelope(envelope);
}


bool Port::getEnvelope(PortReader& envelope)
{
    PortCoreAdapter& core = IMPL();
    return core.getEnvelope(envelope);
}

int Port::getInputCount()
{
    PortCoreAdapter& core = IMPL();
    core.alertOnRead();
    return core.getInputCount();
}

int Port::getOutputCount()
{
    PortCoreAdapter& core = IMPL();
    core.alertOnWrite();
    return core.getOutputCount();
}

void Port::getReport(PortReport& reporter)
{
    PortCoreAdapter& core = IMPL();
    core.describe(reporter);
}


void Port::setReporter(PortReport& reporter)
{
    PortCoreAdapter& core = IMPL();
    core.setReportCallback(&reporter);
}


void Port::resetReporter()
{
    PortCoreAdapter& core = IMPL();
    core.resetReportCallback();
}


void Port::setAdminMode(bool adminMode)
{
    if (adminMode) {
        Bottle b("__ADMIN");
        setEnvelope(b);
    } else {
        Bottle b;
        setEnvelope(b);
    }
}


#define SET_FLAG(implementation, mask, val) \
    IMPL().setFlags((IMPL().getFlags() & (~(mask))) + ((val) ? (mask) : 0))

void Port::setInputMode(bool expectInput)
{
    if (!expectInput) {
        IMPL().setWriteOnly();
    }
    SET_FLAG(implementation, PORTCORE_IS_INPUT, expectInput);
}

void Port::setOutputMode(bool expectOutput)
{
    if (!expectOutput) {
        IMPL().setReadOnly();
    }
    SET_FLAG(implementation, PORTCORE_IS_OUTPUT, expectOutput);
}

void Port::setRpcMode(bool expectRpc)
{
    if (expectRpc) {
        IMPL().setRpc();
    }
    SET_FLAG(implementation, PORTCORE_IS_RPC, expectRpc);
}

bool Port::setTimeout(float timeout)
{
    IMPL().setTimeout(timeout);
    return true;
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
void Port::setVerbosity(int level)
{
    YARP_UNUSED(level);
}

int Port::getVerbosity()
{
    return 0;
}
#endif

Type Port::getType()
{
    return IMPL().getType();
}

void Port::promiseType(const Type& typ)
{
    IMPL().promiseType(typ);
}

Property* Port::acquireProperties(bool readOnly)
{
    return IMPL().acquireProperties(readOnly);
}

void Port::releaseProperties(Property* prop)
{
    IMPL().releaseProperties(prop);
}

void Port::includeNodeInName(bool flag)
{
    IMPL().includeNodeInName(flag);
}

bool Port::isOpen() const
{
    if (implementation == nullptr) {
        return false;
    }
    return IMPL().active;
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.3
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
bool Port::setCallbackLock(yarp::os::Mutex* mutex)
{
    return IMPL().configCallbackLock(mutex);
}
YARP_WARNING_POP
#endif

bool Port::setCallbackLock(std::mutex* mutex)
{
    return IMPL().configCallbackLock(mutex);
}

bool Port::removeCallbackLock()
{
    return IMPL().unconfigCallbackLock();
}

bool Port::lockCallback()
{
    if (!IMPL().lockCallback()) {
        yCError(PORT,"Cannot do lockCallback() without setCallbackLock() before opening port");
    }
    return true;
}

bool Port::tryLockCallback()
{
    return IMPL().tryLockCallback();
}

void Port::unlockCallback()
{
    IMPL().unlockCallback();
}

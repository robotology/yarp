/*
 * Copyright (C) 2006 RobotCub Consortium
 * Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick <paulfitz@alum.mit.edu>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/Port.h>

#include <yarp/conf/system.h>
#include <yarp/os/Portable.h>
#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Network.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/PortCoreAdapter.h>
#include <yarp/os/impl/SemaphoreImpl.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/NameConfig.h>

using namespace yarp::os::impl;
using namespace yarp::os;



void *Port::needImplementation() const {
    if (implementation) return implementation;
    Port *self = (Port *)this;
    self->implementation = new yarp::os::impl::PortCoreAdapter(*self);
    yAssert(self->implementation!=YARP_NULLPTR);
    self->owned = true;
    return self->implementation;
}

// implementation is a PortCoreAdapter
#define IMPL() (*((yarp::os::impl::PortCoreAdapter*)(needImplementation())))

Port::Port() {
    implementation = YARP_NULLPTR;
    owned = false;
}

bool Port::openFake(const ConstString& name) {
    return open(Contact(name), false, name.c_str());
}

bool Port::open(const ConstString& name) {
    return open(Contact(name));
}

bool Port::open(const Contact& contact, bool registerName) {
    return open(contact,registerName,YARP_NULLPTR);
}

bool Port::open(const Contact& contact, bool registerName,
                const char *fakeName) {
    Contact contact2 = contact;

    if (!NetworkBase::initialized()) {
        YARP_ERROR(Logger::get(), "YARP not initialized; create a yarp::os::Network object before using ports");
        return false;
    }

    ConstString n = contact2.getName();

    NameConfig conf;
    ConstString nenv = ConstString("YARP_RENAME") + conf.getSafeString(n);
    ConstString rename = NetworkBase::getEnvironment(nenv.c_str());
    if (rename!="") {
        n = rename;
        contact2.setName(n);
    }

    bool local = false;
    if (n == "" && contact2.getPort()<=0) {
        local = true;
        registerName = false;
        n = "...";
    }

    NestedContact nc(n);
    if (nc.getNestedName()!="") {
        if (nc.getNodeName() == "") {
            Nodes& nodes = NameClient::getNameClient().getNodes();
            nodes.requireActiveName();
            ConstString node_name = nodes.getActiveName();
            if (node_name!="") {
                n = n + node_name;
            }
        }
    }

    PortCoreAdapter *currentCore = &(IMPL());
    if (currentCore!=YARP_NULLPTR) {
        currentCore->active = false;
        if (n!="" && (n[0]!='/'||currentCore->includeNode) && n[0]!='=' && n!="..." && n.substr(0,3)!="...") {
            if (fakeName==YARP_NULLPTR) {
                Nodes& nodes = NameClient::getNameClient().getNodes();
                ConstString node_name = nodes.getActiveName();
                if (node_name!="") {
                    n = (n[0]=='/'?"":"/") + n + "@" + node_name;
                }
            }
        }
    }
    if (n!="" && n[0]!='/'  && n[0]!='=' && n!="..." && n.substr(0,3)!="...") {
        if (fakeName==YARP_NULLPTR) {
            YARP_SPRINTF1(Logger::get(),error,
                          "Port name '%s' needs to start with a '/' character",
                          n.c_str());
            return false;
        }
    }
    if (n!="" && n!="..." && n[0]!='=' && n.substr(0,3)!="...") {
        if (fakeName==YARP_NULLPTR) {
            ConstString prefix = NetworkBase::getEnvironment("YARP_PORT_PREFIX");
            if (prefix!="") {
                n = prefix + n;
                contact2.setName(n);
            }
        }
    }
    if (currentCore!=YARP_NULLPTR) {
        NestedContact nc;
        nc.fromString(n);
        if (nc.getNestedName()!="") {
            if (nc.getCategory()=="") {
                // we need to add in a category
                ConstString cat = "";
                if (currentCore->commitToRead) {
                    cat = "-";
                } else if (currentCore->commitToWrite) {
                    cat = "+";
                }
                if (cat!="") {
                    if (currentCore->commitToRpc) {
                        cat += "1";
                    }
                    contact2.setName(nc.getNestedName() +
                                     cat +
                                     "@" +
                                     nc.getNodeName());
                } else {
                    YARP_SPRINTF1(Logger::get(),error,
                                  "Error: Port '%s' is not committed to being either an input or output port.",
                                  n.c_str());
                    YARP_SPRINTF0(Logger::get(),error,
                                  "YARP does not mind, but we are trying to register with a name server that does.");
                    YARP_SPRINTF0(Logger::get(),error,
                                  "You can call Port::setWriteOnly() or Port::setReadOnly(), OR rename the port.");
                    NestedContact nc2 = nc;
                    nc2.setCategoryWrite();
                    YARP_SPRINTF1(Logger::get(),error,
                                  "For an output port, call it: %s (+ adds data)",
                                  nc2.toString().c_str());
                    nc2.setCategoryRead();
                    YARP_SPRINTF1(Logger::get(),error,
                                  "For an input port, call it: %s (- takes data)",
                                  nc2.toString().c_str());
                    return false;
                }
            }
        }
    }

    // Allow for open() to be called safely many times on the same Port
    if (currentCore->isOpened()) {
        PortCoreAdapter *newCore = new PortCoreAdapter(*this);
        yAssert(newCore!=YARP_NULLPTR);
        // copy state that should survive in a new open()
        if (currentCore->checkPortReader()!=YARP_NULLPTR) {
            newCore->configReader(*(currentCore->checkPortReader()));
        }
        if (currentCore->checkAdminPortReader()!=YARP_NULLPTR) {
            newCore->configAdminReader(*(currentCore->checkAdminPortReader()));
        }
        if (currentCore->checkReadCreator()!=YARP_NULLPTR) {
            newCore->configReadCreator(*(currentCore->checkReadCreator()));
        }
        if (currentCore->checkWaitAfterSend()>=0) {
            newCore->configWaitAfterSend(currentCore->checkWaitAfterSend());
        }
        if (currentCore->haveCallbackLock) {
            newCore->configCallbackLock(currentCore->recCallbackLock);
        }
        close();
        if (owned) delete ((PortCoreAdapter*)implementation);
        implementation = newCore;
        owned = true;
        currentCore = newCore;
        currentCore->active = false;
    }

    PortCoreAdapter& core = IMPL();

    core.openable();

    if (NetworkBase::localNetworkAllocation()&&contact2.getPort()<=0) {
        YARP_DEBUG(Logger::get(),"local network allocation needed");
        local = true;
    }

    bool success = true;
    Contact address(contact2.getName(),
                    contact2.getCarrier(),
                    contact2.getHost(),
                    contact2.getPort());
    address.setNestedContact(contact2.getNested());

    core.setReadHandler(core);
    if (contact2.getPort()>0 && contact2.getHost()!="") {
        registerName = false;
    }

    ConstString ntyp = getType().getNameOnWire();
    if (ntyp=="") {
        NestedContact nc;
        nc.fromString(n);
        if (nc.getTypeName()!="") ntyp = nc.getTypeName();
    }
    if (ntyp=="") {
        ntyp = getType().getName();
    }
    if (ntyp!="") {
        NestedContact nc;
        nc.fromString(contact2.getName());
        nc.setTypeName(ntyp);
        contact2.setNestedContact(nc);
        if (getType().getNameOnWire()!=ntyp) {
            core.promiseType(Type::byNameOnWire(ntyp.c_str()));
        }
    }

    if (registerName&&!local) {
        address = NetworkBase::registerContact(contact2);
    }

    core.setControlRegistration(registerName);
    success = (address.isValid()||local)&&(fakeName==YARP_NULLPTR);

    if (success) {
        // create a node if needed
        Nodes& nodes = NameClient::getNameClient().getNodes();
        nodes.prepare(address.getRegName().c_str());
    }

    // If we are a service client, go ahead and connect
    if (success) {
        NestedContact nc;
        nc.fromString(address.getName());
        if (nc.getNestedName()!="") {
            if (nc.getCategory() == "+1") {
                addOutput(nc.getNestedName());
            }
        }
    }

    ConstString blame = "invalid address";
    if (success) {
        success = core.listen(address,registerName);
        blame = "address conflict";
        if (success) {
            success = core.start();
            blame = "manager did not start";
        }
    }
    if (success) {
        address = core.getAddress();
        if (registerName&&local) {
            contact2.setSocket(address.getCarrier(),
                               address.getHost(),
                               address.getPort());
            contact2.setName(address.getRegName().c_str());
            Contact newName = NetworkBase::registerContact(contact2);
            core.resetPortName(newName.getName());
            address = core.getAddress();
        } else if (core.getAddress().getRegName()=="" && !registerName) {
            core.resetPortName(core.getAddress().toURI(false));
            core.setName(core.getAddress().getRegName());
        }

        if (core.getVerbosity()>=1) {
            if (address.getRegName()=="") {
                YARP_INFO(Logger::get(),
                          ConstString("Anonymous port active at ") +
                          address.toURI());
            } else {
                YARP_INFO(Logger::get(),
                          ConstString("Port ") +
                          address.getRegName() +
                          " active at " +
                          address.toURI());
            }
        }
    }

    if (fakeName!=YARP_NULLPTR) {
        success = core.manualStart(fakeName);
        blame = "unmanaged port failed to start";
    }

    if (!success) {
        YARP_ERROR(Logger::get(),
                   ConstString("Port ") +
                   (address.isValid()?(address.getRegName().c_str()):(contact2.getName().c_str())) +
                   " failed to activate" +
                   (address.isValid()?" at ":"") +
                   (address.isValid()?address.toURI():ConstString("")) +
                   " (" +
                   blame.c_str() +
                   ")");
    }

    if (success) {
        // create a node if needed
        Nodes& nodes = NameClient::getNameClient().getNodes();
        nodes.add(*this);
    }

    if (success && currentCore!=YARP_NULLPTR) currentCore->active = true;
    return success;
}

bool Port::addOutput(const ConstString& name) {
    return addOutput(Contact(name));
}

bool Port::addOutput(const ConstString& name, const ConstString& carrier) {
    return addOutput(Contact(name, carrier));
}

void Port::close() {
    if (!owned) return;
    if (!NameClient::isClosed()) {
        Nodes& nodes = NameClient::getNameClient().getNodes();
        nodes.remove(*this);
    }

    PortCoreAdapter& core = IMPL();
    core.finishReading();
    core.finishWriting();
    core.close();
    core.join();
    core.active = false;

    // In fact, open flag means "ever opened", so don't reset it
    // core.setOpened(false);
}

void Port::interrupt() {
    Nodes& nodes = NameClient::getNameClient().getNodes();
    nodes.remove(*this);

    PortCoreAdapter& core = IMPL();
    core.interrupt();
}

void Port::resume() {
    PortCoreAdapter& core = IMPL();
    core.resumeFull();
    Nodes& nodes = NameClient::getNameClient().getNodes();
    nodes.add(*this);
}



Port::~Port() {
    if (implementation!=YARP_NULLPTR) {
        close();
        if (owned) delete ((PortCoreAdapter*)implementation);
        implementation = YARP_NULLPTR;
        owned = false;
    }
}


Contact Port::where() const {
    PortCoreAdapter& core = IMPL();
    return core.getAddress();
}


bool Port::addOutput(const Contact& contact) {
    PortCoreAdapter& core = IMPL();
    if (core.commitToRead) return false;
    if (core.isInterrupted()) return false;
    core.alertOnWrite();
    ConstString name;
    if (contact.getPort()<=0) {
        name = contact.toString();
    } else {
        name = contact.toURI();
    }
    if (!core.isListening()) {
        return core.addOutput(name.c_str(), YARP_NULLPTR, YARP_NULLPTR, true);
    }
    Contact me = where();
    return NetworkBase::connect(me.getName().c_str(),
                                name.c_str());
}


/**
 * write something to the port
 */
bool Port::write(PortWriter& writer, PortWriter *callback) const {
    PortCoreAdapter& core = IMPL();
    if (core.isInterrupted()) return false;
    core.alertOnWrite();
    bool result = false;
    //WritableAdapter adapter(writer);
    result = core.send(writer,YARP_NULLPTR,callback);
    //writer.onCompletion();
    if (!result) {
        //YARP_DEBUG(Logger::get(), e.toString() + " <<<< Port::write saw this");
        if (callback!=YARP_NULLPTR) {
            callback->onCompletion();
        } else {
            writer.onCompletion();
        }
        // leave result false
    }
    return result;
}

/**
 * write something to the port
 */
bool Port::write(PortWriter& writer, PortReader& reader,
                 PortWriter *callback) const {
    PortCoreAdapter& core = IMPL();
    if (core.isInterrupted()) return false;
    core.alertOnRpc();
    core.alertOnWrite();
    bool result = false;
    result = core.send(writer,&reader,callback);
    if (!result) {
        //YARP_DEBUG(Logger::get(), e.toString() + " <<<< Port::write saw this");
        if (callback!=YARP_NULLPTR) {
            callback->onCompletion();
        } else {
            writer.onCompletion();
        }
        // leave result false
    }
    return result;
}

/**
 * read something from the port
 */
bool Port::read(PortReader& reader, bool willReply) {
    if(!isOpen()) return false;
    PortCoreAdapter& core = IMPL();
    if (willReply) core.alertOnRpc();
    core.alertOnRead();
    if (core.isInterrupted()) return false;
    return core.read(reader,willReply);
}



bool Port::reply(PortWriter& writer) {
    PortCoreAdapter& core = IMPL();
    return core.reply(writer,false,core.isInterrupted());
}

bool Port::replyAndDrop(PortWriter& writer) {
    PortCoreAdapter& core = IMPL();
    return core.reply(writer,true,core.isInterrupted());
}

/**
 * set an external writer for port data
 */
//void Port::setWriter(PortWriter& writer) {
//  YARP_ERROR(Logger::get(),"Port::setWriter not implemented");
//}

void Port::setReader(PortReader& reader) {
    PortCoreAdapter& core = IMPL();
    core.alertOnRead();
    core.configReader(reader);
}

void Port::setAdminReader(PortReader& reader) {
    PortCoreAdapter& core = IMPL();
    core.configAdminReader(reader);
}


void Port::setReaderCreator(PortReaderCreator& creator) {
    PortCoreAdapter& core = IMPL();
    core.alertOnRead();
    core.configReadCreator(creator);
}


void Port::enableBackgroundWrite(bool backgroundFlag) {
    PortCoreAdapter& core = IMPL();
    core.configWaitAfterSend(!backgroundFlag);
}


bool Port::isWriting() {
    PortCoreAdapter& core = IMPL();
    return core.isWriting();
}



bool Port::setEnvelope(PortWriter& envelope) {
    PortCoreAdapter& core = IMPL();
    return core.setEnvelope(envelope);
}


bool Port::getEnvelope(PortReader& envelope) {
    PortCoreAdapter& core = IMPL();
    return core.getEnvelope(envelope);
}

int Port::getInputCount() {
    PortCoreAdapter& core = IMPL();
    core.alertOnRead();
    return core.getInputCount();
}

int Port::getOutputCount() {
    PortCoreAdapter& core = IMPL();
    core.alertOnWrite();
    return core.getOutputCount();
}

void Port::getReport(PortReport& reporter) {
    PortCoreAdapter& core = IMPL();
    core.describe(reporter);
}


void Port::setReporter(PortReport& reporter) {
    PortCoreAdapter& core = IMPL();
    core.setReportCallback(&reporter);
}


void Port::resetReporter() {
    PortCoreAdapter& core = IMPL();
    core.resetReportCallback();
}


void Port::setAdminMode(bool adminMode) {
    if (adminMode) {
        Bottle b("__ADMIN");
        setEnvelope(b);
    } else {
        Bottle b;
        setEnvelope(b);
    }
}


#define SET_FLAG(implementation,mask,val) \
  IMPL().setFlags((IMPL().getFlags() & \
  (~mask)) + (val?mask:0))

void Port::setInputMode(bool expectInput) {
    if (expectInput==false) {
        IMPL().setWriteOnly();
    }
    SET_FLAG(implementation,PORTCORE_IS_INPUT,expectInput);
}

void Port::setOutputMode(bool expectOutput) {
    if (expectOutput==false) {
        IMPL().setReadOnly();
    }
    SET_FLAG(implementation,PORTCORE_IS_OUTPUT,expectOutput);
}

void Port::setRpcMode(bool expectRpc) {
    if (expectRpc==true) {
        IMPL().setRpc();
    }
    SET_FLAG(implementation,PORTCORE_IS_RPC,expectRpc);
}

bool Port::setTimeout(float timeout) {
    IMPL().setTimeout(timeout);
    return true;
}

void Port::setVerbosity(int level) {
    IMPL().setVerbosity(level);
}

int Port::getVerbosity() {
    return IMPL().getVerbosity();
}

Type Port::getType() {
    return IMPL().getType();
}

void Port::promiseType(const Type& typ) {
    IMPL().promiseType(typ);
}

Property *Port::acquireProperties(bool readOnly) {
    return IMPL().acquireProperties(readOnly);
}

void Port::releaseProperties(Property *prop) {
    IMPL().releaseProperties(prop);
}

void Port::includeNodeInName(bool flag) {
    IMPL().includeNodeInName(flag);
}

bool Port::sharedOpen(Port& port) {
    close();
    if (owned) delete ((PortCoreAdapter*)implementation);
    implementation = port.implementation;
    owned = false;
    return true;
}

bool Port::isOpen() const {
    if (!implementation) return false;
    return IMPL().active;
}

bool Port::setCallbackLock(yarp::os::Mutex *mutex) {
    return IMPL().configCallbackLock(mutex);
}

bool Port::removeCallbackLock() {
    return IMPL().unconfigCallbackLock();
}

bool Port::lockCallback() {
    if (!IMPL().lockCallback()) {
        fprintf(stderr,"Cannot do lockCallback() without setCallbackLock() before opening port\n");
    }
    return true;
}

bool Port::tryLockCallback() {
    return IMPL().tryLockCallback();
}

void Port::unlockCallback() {
    return IMPL().unlockCallback();
}

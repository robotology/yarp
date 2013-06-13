// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/conf/system.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Port.h>
#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Network.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/SemaphoreImpl.h>

using namespace yarp::os::impl;
using namespace yarp::os;


class PortCoreAdapter : public PortCore {
private:
    Port& owner;
    SemaphoreImpl stateMutex;
    PortReader *readDelegate;
    PortWriter *writeDelegate;
    //PortReaderCreator *readCreatorDelegate;
    bool readResult, readActive, readBackground, willReply, closed, opened;
    bool replyDue;
    bool dropDue;
    SemaphoreImpl produce, consume, readBlock;
    PortReaderCreator *recReadCreator;
    int recWaitAfterSend;
public:
    PortCoreAdapter(Port& owner) :
        owner(owner), stateMutex(1), readDelegate(NULL), writeDelegate(NULL),
        readResult(false),
        readActive(false),
        readBackground(false),
        willReply(false),
        closed(false),
        opened(false),
        replyDue(false),
        dropDue(false),
        produce(0), consume(0), readBlock(1),
        recReadCreator(NULL),
        recWaitAfterSend(-1)
    {
    }

    void openable() {
        stateMutex.wait();
        closed = false;
        opened = true;
        stateMutex.post();
    }

    void finishReading() {
        if (!readBackground) {
            stateMutex.wait();
            closed = true;
            consume.post();
            consume.post();
            stateMutex.post();
        }
    }

    void finishWriting() {
        if (isWriting()) {
            double start = Time::now();
            double pause = 0.01;
            do {
                Time::delay(pause);
                pause *= 2;
            } while (isWriting() && (Time::now()-start<3));
            if (isWriting()) {
                YARP_ERROR(Logger::get(), "Closing port that was sending data (slowly)");
            }
        }
    }

    virtual bool read(ConnectionReader& reader) {
        // called by comms code
        readBlock.wait();

        if (!reader.isValid()) {
            // termination
            stateMutex.wait();
            if (readDelegate!=NULL) {
                readResult = readDelegate->read(reader);
            }
            stateMutex.post();
            produce.post();
            readBlock.post();
            return false;
        }

        // wait for happy consumer - don't want to miss a packet
        if (!readBackground) {
            consume.wait();
        }

        if (closed) {
            //throw IOException("Port::read shutting down");
            YARP_DEBUG(Logger::get(),"Port::read shutting down");
            readBlock.post();
            return false;
        }

        stateMutex.wait();
        readResult = false;
        if (readDelegate!=NULL) {
            readResult = readDelegate->read(reader);
        } else {
            // read and ignore
            YARP_DEBUG(Logger::get(),"data received in Port, no reader for it");
            Bottle b;
            b.read(reader);
        }
        if (!readBackground) {
            readDelegate = NULL;
            writeDelegate = NULL;
        }
        bool result = readResult;
        stateMutex.post();
        if (!readBackground) {
            produce.post();
        }
        if (result&&willReply) {
            consume.wait();
            if (closed) {
                YARP_DEBUG(Logger::get(),"Port::read shutting down");
                readBlock.post();
                return false;
                //throw IOException("Port::read shutting down");
            }
            if (writeDelegate!=NULL) {
                stateMutex.wait();
                ConnectionWriter *writer = reader.getWriter();
                if (writer!=NULL) {
                    result = readResult = writeDelegate->write(*writer);
                }
                stateMutex.post();
            }
            if (dropDue) {
                reader.requestDrop();
            }
            produce.post();
        }
        readBlock.post();
        return result;
    }

    bool read(PortReader& reader, bool willReply = false) {
        // called by user

        // user claimed they would reply to last read, but then
        // decided not to.
        if (replyDue) {
            Bottle emptyMessage;
            reply(emptyMessage,false,false);
            replyDue = false;
            dropDue = false;
        }
        if (willReply) {
            replyDue = true;
        }

        stateMutex.wait();
        readActive = true;
        readDelegate = &reader;
        writeDelegate = NULL;
        this->willReply = willReply;
        consume.post(); // happy consumer
        stateMutex.post();

        produce.wait();
        stateMutex.wait();
        if (!readBackground) {
            readDelegate = NULL;
        }
        stateMutex.post();
        bool result = readResult;
        return result;
    }

    bool reply(PortWriter& writer, bool drop, bool /*interrupted*/) {
        // send reply even if interrupt has happened in interim
        if (!replyDue) return false;
        replyDue = false;
        dropDue = drop;
        writeDelegate = &writer;
        consume.post();
        produce.wait();
        bool result = readResult;
        return result;
    }

    /*
      Configuration of a port that should be remembered
      between opens and closes
    */

    void configReader(PortReader& reader) {
        stateMutex.wait();
        readActive = true;
        readBackground = true;
        readDelegate = &reader;
        consume.post(); // just do this once
        stateMutex.post();
    }

    void configReadCreator(PortReaderCreator& creator) {
        recReadCreator = &creator;
        setReadCreator(creator);
    }

    void configWaitAfterSend(bool waitAfterSend) {
        if (waitAfterSend&&isManual()) {
            YARP_ERROR(Logger::get(),
                       "Cannot use background-mode writes on a fake port");
        }
        recWaitAfterSend = waitAfterSend?1:0;
        setWaitAfterSend(waitAfterSend);
    }

    PortReader *checkPortReader() {
        return readDelegate;
    }

    PortReaderCreator *checkReadCreator() {
        return recReadCreator;
    }

    int checkWaitAfterSend() {
        return recWaitAfterSend;
    }


    bool isOpened() {
        return opened;
    }

    void setOpen(bool opened) {
        this->opened = opened;
    }
};

// implementation is a PortCoreAdapter
#define HELPER(x) (*((PortCoreAdapter*)(x)))


Port::Port() {
    implementation = new PortCoreAdapter(*this);
    YARP_ASSERT(implementation!=NULL);
}


bool Port::openFake(const char *name) {
    return open(Contact::byName(name),false,name);
}

bool Port::open(const char *name) {
    return open(Contact::fromString(name));
}

bool Port::open(const Contact& contact, bool registerName) {
    return open(contact,registerName,NULL);
}

bool Port::open(const Contact& contact, bool registerName,
                const char *fakeName) {
    Contact contact2 = contact;

    if (!NetworkBase::initialized()) {
        YARP_ERROR(Logger::get(), "YARP not initialized; create a yarp::os::Network object before using ports");
        return false;
    }

    ConstString n = contact2.getName();
    if (n!="..." && n!="" && n[0]!='/') {
        if (fakeName==NULL) {
            YARP_SPRINTF1(Logger::get(),error,
                          "Port name '%s' needs to start with a '/' character",
                          n.c_str());
            return false;
        }
    }
    if (n!="..." && n!="") {
        if (fakeName==NULL) {
            ConstString prefix = NetworkBase::getEnvironment("YARP_PORT_PREFIX");
            if (prefix!="") {
                n = prefix + n;
                contact2 = contact2.addName(n);
            }
        }
    }

    // Allow for open() to be called safely many times on the same Port
    PortCoreAdapter *currentCore = &(HELPER(implementation));
    if (currentCore->isOpened()) {
        PortCoreAdapter *newCore = new PortCoreAdapter(*this);
        YARP_ASSERT(newCore!=NULL);
        // copy state that should survive in a new open()
        if (currentCore->checkPortReader()!=NULL) {
            newCore->configReader(*(currentCore->checkPortReader()));
        }
        if (currentCore->checkReadCreator()!=NULL) {
            newCore->configReadCreator(*(currentCore->checkReadCreator()));
        }
        if (currentCore->checkWaitAfterSend()>=0) {
            newCore->configWaitAfterSend(currentCore->checkWaitAfterSend());
        }
        close();
        delete ((PortCoreAdapter*)implementation);
        implementation = newCore;
    }

    PortCoreAdapter& core = HELPER(implementation);

    core.openable();

    bool local = false;
    if (NetworkBase::localNetworkAllocation()&&contact2.getPort()<=0) {
        YARP_DEBUG(Logger::get(),"local network allocation needed");
        local = true;
    }

    bool success = true;
    Address caddress(contact2.getHost().c_str(),
                     contact2.getPort(),
                     contact2.getCarrier().c_str(),
                     contact2.getName().c_str());
    Address address = caddress;

    core.setReadHandler(core);
    if (contact2.getPort()>0 && contact2.getHost()!="") {
        registerName = false;
    }
    if (registerName&&!local) {
        Contact contactFull = NetworkBase::registerContact(contact2);
        address = Address::fromContact(contactFull);
    }

    core.setControlRegistration(registerName);
    success = (address.isValid()||local)&&(fakeName==NULL);

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
            contact2 = contact2.addSocket(address.getCarrierName().c_str(),
                                          address.getName().c_str(),
                                          address.getPort());
            contact2 = contact2.addName(address.getRegName().c_str());
            Contact newName = NetworkBase::registerContact(contact2);
            core.resetPortName(newName.getName());
            address = core.getAddress();
        }

        if (core.getVerbosity()>=1) {
            YARP_INFO(Logger::get(),
                      String("Port ") +
                      address.getRegName() +
                      " active at " +
                      address.toString());
        }
    }

    if (fakeName!=NULL) {
        success = core.manualStart(fakeName);
        blame = "unmanaged port failed to start";
    }

    if (!success) {
        YARP_ERROR(Logger::get(),
                   String("Port ") +
                   (address.isValid()?(address.getRegName().c_str()):(contact2.getName().c_str())) +
                   " failed to activate" +
                   (address.isValid()?" at ":"") +
                   (address.isValid()?address.toString():String("")) +
                   " (" +
                   blame.c_str() +
                   ")");
    }
    return success;
}

bool Port::addOutput(const char *name) {
    return addOutput(Contact::byName(name));
}

bool Port::addOutput(const char *name, const char *carrier) {
    return addOutput(Contact::byName(name).addCarrier(carrier));
}

void Port::close() {
    PortCoreAdapter& core = HELPER(implementation);
    core.finishReading();
    core.finishWriting();
    core.close();
    core.join();

    // In fact, open flag means "ever opened", so don't reset it
    // core.setOpened(false);
}

void Port::interrupt() {
    PortCoreAdapter& core = HELPER(implementation);
    core.interrupt();
}

void Port::resume() {
    PortCoreAdapter& core = HELPER(implementation);
    core.resume();
}



Port::~Port() {
    if (implementation!=NULL) {
        close();
        delete ((PortCoreAdapter*)implementation);
        implementation = NULL;
    }
}


Contact Port::where() const {
    PortCoreAdapter& core = HELPER(implementation);
    Address address = core.getAddress();
    return address.toContact();
}


bool Port::addOutput(const Contact& contact) {
    PortCoreAdapter& core = HELPER(implementation);
    if (core.isInterrupted()) return false;
    if (!core.isListening()) {
        return core.addOutput(contact.toString().c_str(),NULL,NULL,true);
    }
    Contact me = where();
    return NetworkBase::connect(me.getName().c_str(),
                                contact.toString().c_str());
}


/**
 * write something to the port
 */
bool Port::write(PortWriter& writer, PortWriter *callback) {
    PortCoreAdapter& core = HELPER(implementation);
    if (core.isInterrupted()) return false;
    bool result = false;
    //WritableAdapter adapter(writer);
    result = core.send(writer,NULL,callback);
    //writer.onCompletion();
    if (!result) {
        //YARP_DEBUG(Logger::get(), e.toString() + " <<<< Port::write saw this");
        if (callback!=NULL) {
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
    PortCoreAdapter& core = HELPER(implementation);
    if (core.isInterrupted()) return false;
    bool result = false;
    result = core.send(writer,&reader,callback);
    if (!result) {
        //YARP_DEBUG(Logger::get(), e.toString() + " <<<< Port::write saw this");
        if (callback!=NULL) {
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
    PortCoreAdapter& core = HELPER(implementation);
    if (core.isInterrupted()) return false;
    return core.read(reader,willReply);
}



bool Port::reply(PortWriter& writer) {
    PortCoreAdapter& core = HELPER(implementation);
    return core.reply(writer,false,core.isInterrupted());
}

bool Port::replyAndDrop(PortWriter& writer) {
    PortCoreAdapter& core = HELPER(implementation);
    return core.reply(writer,true,core.isInterrupted());
}

/**
 * set an external writer for port data
 */
//void Port::setWriter(PortWriter& writer) {
//  YARP_ERROR(Logger::get(),"Port::setWriter not implemented");
//}

void Port::setReader(PortReader& reader) {
    PortCoreAdapter& core = HELPER(implementation);
    core.configReader(reader);
}

void Port::setReaderCreator(PortReaderCreator& creator) {
    PortCoreAdapter& core = HELPER(implementation);
    core.configReadCreator(creator);
}


void Port::enableBackgroundWrite(bool backgroundFlag) {
    PortCoreAdapter& core = HELPER(implementation);
    core.configWaitAfterSend(!backgroundFlag);
}


bool Port::isWriting() {
    PortCoreAdapter& core = HELPER(implementation);
    return core.isWriting();
}



bool Port::setEnvelope(PortWriter& envelope) {
    PortCoreAdapter& core = HELPER(implementation);
    return core.setEnvelope(envelope);
}


bool Port::getEnvelope(PortReader& envelope) {
    PortCoreAdapter& core = HELPER(implementation);
    return core.getEnvelope(envelope);
}

int Port::getInputCount() {
    PortCoreAdapter& core = HELPER(implementation);
    return core.getInputCount();
}

int Port::getOutputCount() {
    PortCoreAdapter& core = HELPER(implementation);
    return core.getOutputCount();
}

void Port::getReport(PortReport& reporter) {
    PortCoreAdapter& core = HELPER(implementation);
    core.describe(reporter);
}


void Port::setReporter(PortReport& reporter) {
    PortCoreAdapter& core = HELPER(implementation);
    core.setReportCallback(&reporter);
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
  HELPER(implementation).setFlags((HELPER(implementation).getFlags() & \
  (~mask)) + (val?mask:0))

void Port::setInputMode(bool expectInput) {
    SET_FLAG(implementation,PORTCORE_IS_INPUT,expectInput);
}

void Port::setOutputMode(bool expectOutput) {
    SET_FLAG(implementation,PORTCORE_IS_OUTPUT,expectOutput);
}

void Port::setRpcMode(bool expectRpc) {
    SET_FLAG(implementation,PORTCORE_IS_RPC,expectRpc);
}

bool Port::setTimeout(float timeout) {
    HELPER(implementation).setTimeout(timeout);
    return true;
}

void Port::setVerbosity(int level) {
    HELPER(implementation).setVerbosity(level);
}

int Port::getVerbosity() {
    return HELPER(implementation).getVerbosity();
}



// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Port.h>
#include <yarp/PortCore.h>
#include <yarp/Logger.h>
#include <yarp/NameClient.h>
#include <yarp/os/Contact.h>
#include <yarp/Companion.h>
#include <yarp/SemaphoreImpl.h>

using namespace yarp;
using namespace yarp::os;


class PortCoreAdapter : public PortCore {
private:
    Port& owner;
    SemaphoreImpl stateMutex;
    PortReader *readDelegate;
    bool readResult, readActive, readBackground;
    SemaphoreImpl produce, consume;
public:
    PortCoreAdapter(Port& owner) : 
        owner(owner), stateMutex(1), readDelegate(NULL), readResult(false),
        readActive(false),
        readBackground(false),
        produce(0), consume(0)
    {}

    virtual bool read(ConnectionReader& reader) {
        // called by comms code

        // wait for happy consumer - don't want to miss a packet
        if (!readBackground) {
            consume.wait();
        }
    
        stateMutex.wait();
        readResult = false;
        if (readDelegate!=NULL) {
            readResult = readDelegate->read(reader);
        }
        stateMutex.post();
        if (!readBackground) {
            produce.post();
        }
        return readResult;
    }

    bool read(PortReader& reader) {
        // called by user

        stateMutex.wait();
        readActive = true;
        readDelegate = &reader;
        consume.post(); // happy consumer
        stateMutex.post();

        produce.wait();
        bool result = readResult;
        return result;
    }

    void configReader(PortReader& reader) {
        stateMutex.wait();
        readActive = true;
        readBackground = true;
        readDelegate = &reader;
        consume.post(); // just do this once
        stateMutex.post();
    }
};

// implementation is a PortCoreAdapter
#define HELPER(x) (*((PortCoreAdapter*)(x)))


Port::Port() {
    implementation = new PortCoreAdapter(*this);
    YARP_ASSERT(implementation!=NULL);
}


bool Port::open(const char *name) {
    return open(Contact::byName(name));
}


bool Port::open(const Contact& contact, bool registerName) {

    bool success = true;
    Address caddress(contact.getHost().c_str(),
                     contact.getPort(),
                     contact.getCarrier().c_str(),
                     contact.getName().c_str());
    Address address = caddress;
    try {
        PortCoreAdapter& core = HELPER(implementation);
        core.setReadHandler(core);
        NameClient& nic = NameClient::getNameClient();
        if (registerName) {
            address = nic.registerName(contact.getName().c_str(),caddress);
        }
        success = address.isValid();

        if (success) {
            success = core.listen(address);
            if (success) {
                success = core.start();
            }
        }
    } catch (IOException e) {
        success = false;
        //YARP_DEBUG(Logger::get(),e.toString() + " <<< Port::register failed");
        YARP_ERROR(Logger::get(),String("port ") + contact.getName().c_str() + String(" failed to open: ") + e.toString() + " (" + address.toString() + ")");
    }
    if (!success) {
        close();
    }
    return success;
}


void Port::close() {
    PortCoreAdapter& core = HELPER(implementation);
    core.close();
    core.join();
}


Port::~Port() {
    if (implementation!=NULL) {
        close();
        delete ((PortCoreAdapter*)implementation);
        implementation = NULL;
    }
}


Contact Port::where() {
    PortCoreAdapter& core = HELPER(implementation);
    Address address = core.getAddress();
    return address.toContact();
}


bool Port::addOutput(const Contact& contact) {
    Contact me = where();
    return Companion::connect(me.getName().c_str(),
                              contact.toString().c_str());
}


/**
 * write something to the port
 */
bool Port::write(PortWriter& writer) {
    PortCoreAdapter& core = HELPER(implementation);
    bool result = false;
    try {
        //WritableAdapter adapter(writer);
        core.send(writer);
        //writer.onCompletion();
        result = true;
    } catch (IOException e) {
        YARP_DEBUG(Logger::get(), e.toString() + " <<<< Port::write saw this");
        writer.onCompletion();
        // leave result false
    }
    return false;
}

/**
 * write something to the port
 */
bool Port::write(PortWriter& writer, PortReader& reader) {
    PortCoreAdapter& core = HELPER(implementation);
    bool result = false;
    try {
        core.send(writer,&reader);
        result = true;
    } catch (IOException e) {
        YARP_DEBUG(Logger::get(), e.toString() + " <<<< Port::write saw this");
        writer.onCompletion();
        // leave result false
    }
    return false;
}

/**
 * read something from the port
 */
bool Port::read(PortReader& reader) {
    PortCoreAdapter& core = HELPER(implementation);
    return core.read(reader);
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


void Port::enableBackgroundWrite(bool backgroundFlag) {
    PortCoreAdapter& core = HELPER(implementation);
    core.setWaitAfterSend(!backgroundFlag);
}


bool Port::isWriting() {
    PortCoreAdapter& core = HELPER(implementation);
    return core.isWriting();
}




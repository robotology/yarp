// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Port.h>
#include <yarp/PortCore.h>
#include <yarp/Logger.h>
#include <yarp/NameClient.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Network.h>
#include <yarp/SemaphoreImpl.h>

using namespace yarp;
using namespace yarp::os;


class PortCoreAdapter : public PortCore {
private:
    Port& owner;
    SemaphoreImpl stateMutex;
    PortReader *readDelegate;
    PortWriter *writeDelegate;
    //PortReaderCreator *readCreatorDelegate;
    bool readResult, readActive, readBackground, willReply, closed;
    SemaphoreImpl produce, consume;
public:
    PortCoreAdapter(Port& owner) : 
        owner(owner), stateMutex(1), readDelegate(NULL), writeDelegate(NULL),
        readResult(false),
        readActive(false),
        readBackground(false),
        willReply(false),
        closed(false),
        produce(0), consume(0)
    {}

    void openable() {
        stateMutex.wait();
        closed = false;
        stateMutex.post();
    }

    void finishReading() {
        if (!readBackground) {
            stateMutex.wait();
            YARP_DEBUG(Logger::get(),"Port::read closing...");
            closed = true;
            consume.post();
            consume.post();
            stateMutex.post();
        }
    }

    virtual bool read(ConnectionReader& reader) {
        // called by comms code
        if (!reader.isValid()) {
            // termination
            stateMutex.wait();
            if (readDelegate!=NULL) {
                readResult = readDelegate->read(reader);
            }
            stateMutex.post();
            produce.post();
            return false;
        } 

        // wait for happy consumer - don't want to miss a packet
        if (!readBackground) {
            consume.wait();
        }

        if (closed) {
            throw IOException("Port::read shutting down");
        }

        stateMutex.wait();
        readResult = false;
        if (readDelegate!=NULL) {
            readResult = readDelegate->read(reader);
        }
        if (!readBackground) {
            readDelegate = NULL;
            writeDelegate = NULL;
        }
        stateMutex.post();
        if (!readBackground) {
            produce.post();
        }
        if (readResult&&willReply) {
            consume.wait();
            if (closed) {
                throw IOException("Port::read shutting down");
            }
            stateMutex.wait();
            ConnectionWriter *writer = reader.getWriter();
            if (writer!=NULL) {
                readResult = writeDelegate->write(*writer);
            }
            stateMutex.post();
            produce.post();
        }
        return readResult;
    }

    bool read(PortReader& reader, bool willReply = false) {
        // called by user

        stateMutex.wait();
        readActive = true;
        readDelegate = &reader;
        writeDelegate = NULL;
        this->willReply = willReply;
        consume.post(); // happy consumer
        stateMutex.post();

        produce.wait();
        bool result = readResult;
        return result;
    }

    bool reply(PortWriter& writer) {
        writeDelegate = &writer;
        consume.post();
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
    PortCoreAdapter& core = HELPER(implementation);
    core.openable();
    return open(Contact::byName(name));
}


bool Port::open(const Contact& contact, bool registerName) {
    PortCoreAdapter& core = HELPER(implementation);
    core.openable();

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
            YARP_INFO(Logger::get(),
                      String("Port ") +
                      address.getRegName() +
                      " listening at " +
                      address.toString());
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
    core.finishReading();
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
    return Network::connect(me.getName().c_str(),
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
    return result;
}

/**
 * write something to the port
 */
bool Port::write(PortWriter& writer, PortReader& reader) const {
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
    return result;
}

/**
 * read something from the port
 */
bool Port::read(PortReader& reader, bool willReply) {
    PortCoreAdapter& core = HELPER(implementation);
    return core.read(reader,willReply);
}



bool Port::reply(PortWriter& writer) {
    PortCoreAdapter& core = HELPER(implementation);
    return core.reply(writer);
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
    core.setReadCreator(creator);
}


void Port::enableBackgroundWrite(bool backgroundFlag) {
    PortCoreAdapter& core = HELPER(implementation);
    core.setWaitAfterSend(!backgroundFlag);
}


bool Port::isWriting() {
    PortCoreAdapter& core = HELPER(implementation);
    return core.isWriting();
}




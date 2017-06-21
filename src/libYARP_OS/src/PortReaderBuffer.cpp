/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/os/Portable.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Os.h>

#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/SemaphoreImpl.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/impl/StreamConnectionReader.h>

#include <yarp/os/impl/PortCorePacket.h>

#include <list>

using namespace yarp::os::impl;
using namespace yarp::os;



class PortReaderPacket {
public:
    PortReaderPacket *prev_, *next_;

    // if non-null, contains a buffer that the packet owns
    PortReader *reader;

    ConstString envelope;

    // if nun-null, refers to an external buffer
    // by convention, overrides reader
    PortReader *external;
    PortWriter *writer; // if a callback is needed

    PortReaderPacket() {
        prev_ = next_ = YARP_NULLPTR;
        reader = YARP_NULLPTR;
        external = YARP_NULLPTR;
        writer = YARP_NULLPTR;
        reset();
    }

    virtual ~PortReaderPacket() {
        resetExternal();
        reset();
    }

    void reset() {
        if (reader!=YARP_NULLPTR) {
            delete reader;
            reader = YARP_NULLPTR;
        }
        writer = YARP_NULLPTR;
        envelope = "";
    }

    PortReader *getReader() {
        return reader;
    }

    void setReader(PortReader *reader) {
        resetExternal();
        reset();
        this->reader = reader;
    }

    PortReader *getExternal() {
        return external;
    }

    void setExternal(PortReader *reader, PortWriter *writer) {
        resetExternal();
        this->external = reader;
        this->writer = writer;
    }

    void setEnvelope(const Bytes& bytes) {
        envelope = ConstString(bytes.get(), bytes.length());
        //envelope.set(bytes.get(), bytes.length(), 1);
    }

    void resetExternal() {
        if (writer!=YARP_NULLPTR) {
            writer->onCompletion();
            writer = YARP_NULLPTR;
        }
        external = YARP_NULLPTR;
    }
};


class PortReaderPool {
private:
    std::list<PortReaderPacket*> inactive;
    std::list<PortReaderPacket*> active;

public:

    int getCount() {
        return (int)active.size();
    }

    int getFree() {
        return (int)inactive.size();
    }

    PortReaderPacket *getInactivePacket() {
        if (inactive.empty()) {
            PortReaderPacket *obj = YARP_NULLPTR;
            obj = new PortReaderPacket();
            inactive.push_back(obj);
        }
        PortReaderPacket *next = inactive.front();
        yAssert(next!=YARP_NULLPTR);
        inactive.remove(next);
        return next;
    }

    PortReaderPacket *getActivePacket() {
        PortReaderPacket *next = YARP_NULLPTR;
        if (getCount()>=1) {
            next = active.front();
            yAssert(next!=YARP_NULLPTR);
            active.remove(next);
        }
        return next;
    }

    void addActivePacket(PortReaderPacket *packet) {
        if (packet!=YARP_NULLPTR) {
            active.push_back(packet);
        }
    }

    void addInactivePacket(PortReaderPacket *packet) {
        if (packet!=YARP_NULLPTR) {
            inactive.push_back(packet);
        }
    }

    void reset() {
        while (!active.empty()) {
            delete active.back();
            active.pop_back();
        }
        while (!inactive.empty()) {
            delete inactive.back();
            inactive.pop_back();
        }
    }
};



class PortReaderBufferBaseHelper {
private:

    PortReaderBufferBase& owner;
    PortReaderPacket *prev;

public:

    PortReaderPool pool;

    int ct;
    Port *port;
    SemaphoreImpl contentSema;
    SemaphoreImpl consumeSema;
    SemaphoreImpl stateSema;

    PortReaderBufferBaseHelper(PortReaderBufferBase& owner) :
        owner(owner), contentSema(0), consumeSema(0), stateSema(1) {
        prev = YARP_NULLPTR;
        port = YARP_NULLPTR;
        ct = 0;
    }

    virtual ~PortReaderBufferBaseHelper() {
        Port *closePort = YARP_NULLPTR;
        stateSema.wait();
        if (port!=YARP_NULLPTR) {
            closePort = port;
        }
        stateSema.post();
        if (closePort!=YARP_NULLPTR) {
            closePort->close();
        }
        stateSema.wait();
        clear();
        //stateSema.post();  // never give back mutex
    }

    void clear() {
        if (prev!=YARP_NULLPTR) {
            pool.addInactivePacket(prev);
            prev = YARP_NULLPTR;
        }
        pool.reset();
        ct = 0;
    }


    ConstString getName() {
        if (port!=YARP_NULLPTR) {
            return port->getName();
        }
        return "";
    }

    PortReaderPacket *get() {
        PortReaderPacket *result = YARP_NULLPTR;
        bool grab = true;
        if (pool.getFree()==0) {
            grab = false;
            int maxBuf = owner.getMaxBuffer();
            if (maxBuf==0 || (pool.getFree()+pool.getCount())<maxBuf) {
                grab = true;
            } else {
                // ok, can't get free, clean space.
                // here would be a good place to do buffer reuse.
            }
        }
        if (grab) {
            result = pool.getInactivePacket();
        }

        return result;
    }

    int checkContent() {
        return pool.getCount();
    }

    PortReaderPacket *getContent() {
        if (prev!=YARP_NULLPTR) {
            pool.addInactivePacket(prev);
            prev = YARP_NULLPTR;
        }
        if (pool.getCount()>=1) {
            prev = pool.getActivePacket();
            ct--;
        }
        return prev;
    }


    bool getEnvelope(PortReader& envelope) {
        if (prev==YARP_NULLPTR) {
            return false;
        }
        StringInputStream sis;
        sis.add(prev->envelope.c_str());
        sis.add("\r\n");
        StreamConnectionReader sbr;
        Route route;
        sbr.reset(sis, YARP_NULLPTR, route, 0, true);
        return envelope.read(sbr);
    }

    PortReaderPacket *dropContent() {
        // don't affect "prev"
        PortReaderPacket *drop = YARP_NULLPTR;

        if (pool.getCount()>=1) {
            drop = pool.getActivePacket();
            if (drop!=YARP_NULLPTR) {
                pool.addInactivePacket(drop);
            }
            ct--;
        }
        return drop;
    }

    void attach(Port& port) {
        this->port = &port;
        port.setReader(owner);
    }

    void *acquire() {
        if (prev!=YARP_NULLPTR) {
            void *result = prev;
            prev = YARP_NULLPTR;
            return result;
        }
        return YARP_NULLPTR;
    }

    void release(void *key) {
        if (key!=YARP_NULLPTR) {
            pool.addInactivePacket((PortReaderPacket*)key);
        }
    }
};



PortReaderBufferBaseCreator::~PortReaderBufferBaseCreator()
{
}

// implementation is a list
#define HELPER(x) (*((PortReaderBufferBaseHelper*)(x)))

PortReaderBufferBase::PortReaderBufferBase(unsigned int maxBuffer) :
        creator(YARP_NULLPTR),
        maxBuffer(maxBuffer),
        prune(false),
        allowReuse(true),
        implementation(YARP_NULLPTR),
        replier(YARP_NULLPTR),
        period(-1),
        last_recv(-1) {
    init();
}

PortReaderBufferBase::~PortReaderBufferBase() {
    if (implementation!=YARP_NULLPTR) {
        delete &HELPER(implementation);
        implementation = YARP_NULLPTR;
    }
}

void PortReaderBufferBase::init() {
    implementation = new PortReaderBufferBaseHelper(*this);
    yAssert(implementation!=YARP_NULLPTR);
}

yarp::os::PortReader *PortReaderBufferBase::create() {
    if (creator!=YARP_NULLPTR) {
        return creator->create();
    }
    return YARP_NULLPTR;
}

void PortReaderBufferBase::release(PortReader *completed) {
    //HELPER(implementation).stateSema.wait();
    //HELPER(implementation).configure(completed, true, false);
    //HELPER(implementation).stateSema.post();
    printf("release not implemented anymore; not needed\n");
    std::exit(1);
}

int PortReaderBufferBase::check() {
    HELPER(implementation).stateSema.wait();
    int count = HELPER(implementation).checkContent();
    HELPER(implementation).stateSema.post();
    return count;
}


void PortReaderBufferBase::interrupt() {
    // give read a chance
    HELPER(implementation).contentSema.post();
}

PortReader *PortReaderBufferBase::readBase(bool& missed, bool cleanup) {
    missed = false;
    if (period<0 || cleanup) {
        HELPER(implementation).contentSema.wait();
    } else {
        bool ok = false;
        double now = SystemClock::nowSystem();
        double target = now+period;
        if (last_recv>0) {
            target = last_recv+period;
        }
        double diff = target-now;
        if (diff>0) {
            ok = HELPER(implementation).contentSema.waitWithTimeout(diff);
        } else {
            ok = HELPER(implementation).contentSema.check();
            if (ok) HELPER(implementation).contentSema.wait();
        }
        if (!ok) {
            missed = true;
            if (last_recv>0) {
                last_recv += period;
            }
            return YARP_NULLPTR;
        }
        now = SystemClock::nowSystem();
        if (last_recv<0) {
            last_recv = now;
        } else {
            diff = target - now;
            if (diff>0) {
                SystemClock::delaySystem(diff);
            }
            last_recv = target;
        }
    }
    HELPER(implementation).stateSema.wait();
    PortReaderPacket *readerPacket = HELPER(implementation).getContent();
    PortReader *reader = YARP_NULLPTR;
    if (readerPacket!=YARP_NULLPTR) {
        PortReader *external = readerPacket->getExternal();
        if (external==YARP_NULLPTR) {
            reader = readerPacket->getReader();
        } else {
            reader = external;
        }
    }
    HELPER(implementation).stateSema.post();
    if (reader!=YARP_NULLPTR) {
        HELPER(implementation).consumeSema.post();
    }
    return reader;
}


bool PortReaderBufferBase::read(ConnectionReader& connection) {
    if (connection.getReference()!=YARP_NULLPTR) {
        //printf("REF %ld %d\n", (long int)connection.getReference(),
        //     connection.isValid());
        return acceptObjectBase(connection.getReference(), YARP_NULLPTR);
    }

    if (replier != YARP_NULLPTR) {
        if (connection.getWriter()) {
            return replier->read(connection);
        }
    }
    PortReaderPacket *reader = YARP_NULLPTR;
    while (reader==YARP_NULLPTR) {
        HELPER(implementation).stateSema.wait();
        reader = HELPER(implementation).get();
        if (reader->getReader()==YARP_NULLPTR) {
            PortReader *next = create();
            yAssert(next!=YARP_NULLPTR);
            reader->setReader(next);
        }
        HELPER(implementation).stateSema.post();
        if (reader==YARP_NULLPTR) {
            HELPER(implementation).consumeSema.wait();
        }
    }
    bool ok = false;
    if (connection.isValid()) {
        yAssert(reader->getReader()!=YARP_NULLPTR);
        ok = reader->getReader()->read(connection);
        reader->setEnvelope(connection.readEnvelope());
    } else {
        // this is a disconnection
        // don't talk to this port ever again
        HELPER(implementation).port = YARP_NULLPTR;
    }
    if (ok) {
        HELPER(implementation).stateSema.wait();
        bool pruned = false;
        if (HELPER(implementation).ct>0&&prune) {
            PortReaderPacket *readerPacket =
                HELPER(implementation).dropContent();
            pruned = (readerPacket!=YARP_NULLPTR);
        }
        //HELPER(implementation).configure(reader, false, true);
        HELPER(implementation).pool.addActivePacket(reader);
        HELPER(implementation).ct++;
        HELPER(implementation).stateSema.post();
        if (!pruned) {
            HELPER(implementation).contentSema.post();
        }
        //YARP_ERROR(Logger::get(), ">>>>>>>>>>>>>>>>> adding data");
    } else {
        HELPER(implementation).stateSema.wait();
        HELPER(implementation).pool.addInactivePacket(reader);
        HELPER(implementation).stateSema.post();
        //YARP_ERROR(Logger::get(), ">>>>>>>>>>>>>>>>> skipping data");

        // important to give reader a shot anyway, allowing proper closing
        YARP_DEBUG(Logger::get(), "giving PortReaderBuffer chance to close");
        HELPER(implementation).contentSema.post();
    }
    return ok;
}



void PortReaderBufferBase::setCreator(PortReaderBufferBaseCreator *creator) {
    this->creator = creator;
}

void PortReaderBufferBase::setReplier(yarp::os::PortReader& reader) {
    replier = &reader;
}

void PortReaderBufferBase::setPrune(bool flag) {
    prune = flag;
}

void PortReaderBufferBase::setAllowReuse(bool flag) {
    allowReuse = flag;
}

void PortReaderBufferBase::setTargetPeriod(double period) {
    this->period = period;
}

ConstString PortReaderBufferBase::getName() const {
    return HELPER(implementation).getName();
}

unsigned int PortReaderBufferBase::getMaxBuffer() {
    return maxBuffer;
}

bool PortReaderBufferBase::isClosed() {
    return HELPER(implementation).port==YARP_NULLPTR;
}

void PortReaderBufferBase::attachBase(Port& port) {
    HELPER(implementation).attach(port);
}

#ifndef YARP_NO_DEPRECATED
void PortReaderBufferBase::setAutoRelease(bool flag) {
    //HELPER(implementation).stateSema.wait();
    //HELPER(implementation).setAutoRelease(flag);
    //HELPER(implementation).stateSema.post();
    printf("setAutoRelease not implemented anymore; not needed\n");
    std::exit(1);
}
#endif // YARP_NO_DEPRECATED



/////////////////////
///
/// Careful!  merge with ::read -- very similar code
/// Until merge, don't change one without looking at other :-(

bool PortReaderBufferBase::acceptObjectBase(PortReader *obj,
                                            yarp::os::PortWriter *wrapper) {
    // getting an object here should be basically the same as
    // receiving from a Port -- except no need to create/read
    // the object

    PortReaderPacket *reader = YARP_NULLPTR;
    while (reader==YARP_NULLPTR) {
        HELPER(implementation).stateSema.wait();
        reader = HELPER(implementation).get();
        HELPER(implementation).stateSema.post();
        if (reader==YARP_NULLPTR) {
            HELPER(implementation).consumeSema.wait();
        }
    }
    bool ok = true;
    if (ok) {
        reader->setExternal(obj, wrapper);

        HELPER(implementation).stateSema.wait();
        bool pruned = false;
        if (HELPER(implementation).ct>0&&prune) {
            PortReaderPacket *readerPacket =
                HELPER(implementation).dropContent();
            pruned = (readerPacket!=YARP_NULLPTR);
        }
        //HELPER(implementation).configure(reader, false, true);
        HELPER(implementation).pool.addActivePacket(reader);
        HELPER(implementation).ct++;
        HELPER(implementation).stateSema.post();
        if (!pruned) {
            HELPER(implementation).contentSema.post();
        }
        //YARP_ERROR(Logger::get(), ">>>>>>>>>>>>>>>>> adding data");
    } else {
        HELPER(implementation).stateSema.wait();
        HELPER(implementation).pool.addInactivePacket(reader);
        HELPER(implementation).stateSema.post();
        //YARP_ERROR(Logger::get(), ">>>>>>>>>>>>>>>>> skipping data");

        // important to give reader a shot anyway, allowing proper closing
        YARP_DEBUG(Logger::get(), "giving PortReaderBuffer chance to close");
        HELPER(implementation).contentSema.post();
    }

    return true;
}



bool PortReaderBufferBase::forgetObjectBase(PortReader *obj,
                                            yarp::os::PortWriter *wrapper) {
    printf("Sorry, forgetting not implemented yet\n");
    return false;
}



void *PortReaderBufferBase::acquire() {
    return HELPER(implementation).acquire();
}

void PortReaderBufferBase::release(void *key) {
    HELPER(implementation).stateSema.wait();
    HELPER(implementation).release(key);
    HELPER(implementation).stateSema.post();
}


bool PortReaderBufferBase::getEnvelope(PortReader& envelope) {
    return HELPER(implementation).getEnvelope(envelope);
}

void PortReaderBufferBase::clear() {
    HELPER(implementation).clear();
}

void typedReaderMissingCallback() {
    YARP_ERROR(Logger::get(), "Missing or incorrectly typed onRead function");
}

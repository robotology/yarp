// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/Thread.h>

#include <yarp/Logger.h>
#include <yarp/SemaphoreImpl.h>

#include <ace/Vector_T.h>


using namespace yarp;
using namespace yarp::os;

/*
class PortReaderBufferThread : public Thread {
public:
    PortReader& reader;
    PortReaderBuffer& buffer;

    PortReaderBufferThread(PortReader& reader, PortReaderBuffer& buffer) : 
        reader(reader), buffer(buffer) {
    }

    virtual void run() {
        while (!isStopping()) {
            if (buffer.read()) {
            }
        }
    }
};
*/

class PortReaderBufferBaseHelper {
private:
    PortReaderBufferBase& owner;
    ACE_Vector<PortReader*> readers;
    ACE_Vector<bool> avail;
    ACE_Vector<bool> content;
    bool autoRelease;
    PortReader *prev;

public:
	Port *port;
    SemaphoreImpl contentSema;
    SemaphoreImpl consumeSema;
    SemaphoreImpl stateSema;

    PortReaderBufferBaseHelper(PortReaderBufferBase& owner) : 
        owner(owner), contentSema(0), consumeSema(0), stateSema(1) {
        autoRelease = true;
        prev = NULL;
		port = NULL;
    }

    virtual ~PortReaderBufferBaseHelper() {
        Port *closePort = NULL;
        stateSema.wait();
		if (port!=NULL) {
			closePort = port;
		}
        stateSema.post();
        if (closePort!=NULL) {
            closePort->close();
        }
        stateSema.wait();
        clear();
        //stateSema.post();  // never give back mutex
    }

    void setAutoRelease(bool flag) {
        autoRelease = flag;
    }

    void clear() {
        for (unsigned int i=0; i<readers.size(); i++) {
            delete readers[i];
        }
        readers.clear();  
        avail.clear();
        content.clear();
    }

    PortReader *get() {
        PortReader *result = getAvail();
        if (result == NULL) {
            unsigned int maxBuf = owner.getMaxBuffer();
            if (maxBuf==0 || content.size()<maxBuf) {
                result = add();
            } else {
                // ok, can't get free, clean space.
                // here would be a good place to do buffer reuse.
            }
        }
        return result;
    }

    PortReader *getAvail() {
        for (unsigned int i=0; i<readers.size(); i++) {
            if (avail[i]) {
                avail[i] = false;
                return readers[i];
            }
        }
        return NULL;
    }

    PortReader *getContent(bool flag) {
        if (autoRelease) {
            if (prev!=NULL) {
                configure(prev,true,false);
                prev = NULL;
            }
        }
        for (unsigned int i=0; i<readers.size(); i++) {
            if (content[i]) {
                content[i] = flag;
                if (flag==false) {
                    prev = readers[i];
                }
                return readers[i];
            }
        }
        return NULL;
    }

    PortReader *add() {
        PortReader *next = owner.create();
        YARP_ASSERT(next!=NULL);
        avail.push_back(false);
        content.push_back(false);
        readers.push_back(next);
        return next;
    }

    void configure(PortReader *reader, bool isAvail, bool isContent) {
        for (unsigned int i=0; i<readers.size(); i++) {
            if (readers[i] == reader) {
                avail[i] = isAvail;
                content[i] = isContent;
                break;
            }
        }
        if (isAvail&&!isContent) {
            if (reader == prev) {
                prev = NULL;
            }
        }
    }

	void attach(Port& port) {
		this->port = &port;
		port.setReader(owner);
	}
};


// implementation is a list
#define HELPER(x) (*((PortReaderBufferBaseHelper*)(x)))

void PortReaderBufferBase::init() {
    implementation = new PortReaderBufferBaseHelper(*this);
    YARP_ASSERT(implementation!=NULL);
}

PortReaderBufferBase::~PortReaderBufferBase() {
    if (implementation!=NULL) {
        delete &HELPER(implementation);
        implementation = NULL;
    }
}

void PortReaderBufferBase::release(PortReader *completed) {
    HELPER(implementation).stateSema.wait();
    HELPER(implementation).configure(completed,true,false);
    HELPER(implementation).stateSema.post();
}

bool PortReaderBufferBase::check() {
    HELPER(implementation).stateSema.wait();
    PortReader *reader = HELPER(implementation).getContent(true);
    HELPER(implementation).stateSema.post();
    return (reader!=NULL);
}

PortReader *PortReaderBufferBase::readBase() {
    HELPER(implementation).contentSema.wait();
    HELPER(implementation).stateSema.wait();
    PortReader *reader = HELPER(implementation).getContent(false);
    HELPER(implementation).stateSema.post();
    if (reader!=NULL) {
        HELPER(implementation).consumeSema.post();
    }
    return reader;
}


bool PortReaderBufferBase::read(ConnectionReader& connection) {
    PortReader *reader = NULL;
    while (reader==NULL) {
        HELPER(implementation).stateSema.wait();
        reader = HELPER(implementation).get();
        HELPER(implementation).stateSema.post();
        if (reader==NULL) {
            HELPER(implementation).consumeSema.wait();
        }
    }
    bool ok = false;
    if (connection.isValid()) {
        ok = reader->read(connection);
	} else {
		// this is a disconnection
		// don't talk to this port ever again
		HELPER(implementation).port = NULL;
	}
    if (ok) {
        HELPER(implementation).stateSema.wait();
        HELPER(implementation).configure(reader,false,true);
        HELPER(implementation).stateSema.post();
        HELPER(implementation).contentSema.post();
        //YARP_ERROR(Logger::get(),">>>>>>>>>>>>>>>>> adding data");
    } else {
        HELPER(implementation).stateSema.wait();
        HELPER(implementation).configure(reader,true,false);
        HELPER(implementation).stateSema.post();
        //YARP_ERROR(Logger::get(),">>>>>>>>>>>>>>>>> skipping data");

        // important to give reader a shot anyway, allowing proper closing
        YARP_DEBUG(Logger::get(), "giving PortReaderBuffer chance to close");
        HELPER(implementation).contentSema.post();
    }
    return ok;
}


void PortReaderBufferBase::setAutoRelease(bool flag) {
    HELPER(implementation).stateSema.wait();
    HELPER(implementation).setAutoRelease(flag);
    HELPER(implementation).stateSema.post();
}

void PortReaderBufferBase::attachBase(yarp::os::Port& port) {
    HELPER(implementation).attach(port);
}

bool PortReaderBufferBase::isClosed() {
	return HELPER(implementation).port==NULL;
}





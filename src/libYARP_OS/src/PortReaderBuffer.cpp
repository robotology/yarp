
#include <yarp/os/PortReaderBuffer.h>

#include <yarp/Logger.h>
#include <yarp/SemaphoreImpl.h>

#include <ace/Vector_T.h>


using namespace yarp;
using namespace yarp::os;


class PortReaderBufferBaseHelper {
private:
  PortReaderBufferBase& owner;
  ACE_Vector<PortReader*> readers;
  ACE_Vector<bool> avail;
  ACE_Vector<bool> content;
  bool autoRelease;
  PortReader *prev;

public:
  SemaphoreImpl contentSema;
  SemaphoreImpl consumeSema;
  SemaphoreImpl stateSema;

  PortReaderBufferBaseHelper(PortReaderBufferBase& owner) : 
    owner(owner), contentSema(0), consumeSema(0), stateSema(1) {
    autoRelease = true;
    prev = NULL;
  }

  virtual ~PortReaderBufferBaseHelper() {
    clear();
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
  bool ok = reader->read(connection);
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
  }
  return ok;
}


void PortReaderBufferBase::setAutoRelease(bool flag) {
  HELPER(implementation).stateSema.wait();
  HELPER(implementation).setAutoRelease(flag);
  HELPER(implementation).stateSema.post();
}

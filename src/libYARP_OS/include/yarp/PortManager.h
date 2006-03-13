#ifndef _YARP2_PORTMANAGER_
#define _YARP2_PORTMANAGER_

#include <yarp/String.h>
#include <yarp/OutputStream.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/Logger.h>

#include <ace/OS_NS_stdio.h>

namespace yarp {
  class PortManager;
}

/**
 * Specification of minimal operations a port must support.
 */
class yarp::PortManager {
public:
  PortManager() {
    os = NULL;
    name = "null";
  }

  virtual ~PortManager() {
  }

  void setName(const String& name) {
    this->name = name;
  }

  virtual void addOutput(const String& dest, void *id, OutputStream *os) {
    ACE_OS::printf("ADDOUTPUT\n");
    ACE_DEBUG((LM_ERROR,"PortManager for [%s] asked to addOutput [%s]\n",
	       getName().c_str(),
	       dest.c_str()));
  }

  virtual void removeInput(const String& src, void *id, OutputStream *os) {
    ACE_DEBUG((LM_ERROR,"PortManager for [%s] asked to removeInput [%s]\n",
	       getName().c_str(),
	       src.c_str()));
  }

  virtual void removeOutput(const String& dest, void *id, OutputStream *os) {
    ACE_DEBUG((LM_ERROR,"PortManager for [%s] asked to removeOutput [%s]\n",
	       getName().c_str(),
	       dest.c_str()));
  }
  
  virtual void describe(void *id, OutputStream *os) {
    ACE_DEBUG((LM_ERROR,"PortManager for [%s] asked to describe itself\n",
	       getName().c_str()));
  }

  virtual void readBlock(ConnectionReader& reader, void *id, OutputStream *os) {
    ACE_DEBUG((LM_ERROR,"PortManager for [%s] asked to deal with data\n",
	       getName().c_str()));
  }

  virtual String getName() {
    return String(name);
  }

protected:
  bool hasOutput() {
    return os!=NULL;
  }

  OutputStream& getOutputStream() {
    YARP_ASSERT(os!=NULL);
    return *os;
  }

private:
  OutputStream *os;
  String name;
};

#endif


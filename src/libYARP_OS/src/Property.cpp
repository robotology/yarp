// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Property.h>
#include <yarp/Logger.h>

#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>

using namespace yarp;
using namespace yarp::os;

class PropertyHelper {
public:
  ACE_Hash_Map_Manager<String,String,ACE_Null_Mutex> data;

  void put(const char *key, const char *val) {
    data.rebind(String(key),String(val));
  }

  bool check(const char *key) {
    String out;
    int result = data.find(String(key),out);
    return (result!=-1);
  }

  ConstString get(const char *key) {
    String out;
    int result = data.find(String(key),out);
    if (result!=-1) {
      return ConstString(out.c_str());
    }
    return ConstString("");
  }
};


// implementation is a PropertyHelper
#define HELPER(x) (*((PropertyHelper*)(x)))


Property::Property() {
  implementation = new PropertyHelper;
  YARP_ASSERT(implementation!=NULL);
}


Property::~Property() {
  if (implementation!=NULL) {
    delete &HELPER(implementation);
    implementation = NULL;
  }
}


void Property::put(const char *key, const char *val) {
  HELPER(implementation).put(key,val);
}


bool Property::check(const char *key) {
  return HELPER(implementation).check(key);
}


ConstString Property::get(const char *key) {
  return HELPER(implementation).get(key);
}



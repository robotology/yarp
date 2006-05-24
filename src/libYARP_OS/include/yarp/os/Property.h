// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_PROPERTY_
#define _YARP2_PROPERTY_

#include <yarp/os/ConstString.h>

namespace yarp {
  namespace os {
    class Property;
  }
}

class yarp::os::Property {

public:
    Property();

    virtual ~Property();

    void put(const char *key, const char *val);
    bool check(const char *key);
    ConstString get(const char *key);

private:
    void *implementation;

};

#endif


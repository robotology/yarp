// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_PROPERTY_
#define _YARP2_PROPERTY_

#include <yarp/os/ConstString.h>
#include <yarp/os/Bottle.h>

namespace yarp {
    namespace os {
        class Property;
    }
}

class yarp::os::Property {

public:
    Property();

    virtual ~Property();

    bool check(const char *key) const;

    void put(const char *key, const char *val);

    BottleBit& get(const char *key) const;
    Bottle *getList(const char *key) const;
    ConstString getString(const char *key) const;

    //Bottle& putBottle(const char *key, const Bottle& val);
    //Bottle& putBottle(const char *key);
    //Bottle *getBottle(const char *key) const;

    void clear();

    void fromString(const char *txt);
    
    void fromCommand(int argc, char *argv[]);

    ConstString toString() const;

private:
    void *implementation;

};

#endif


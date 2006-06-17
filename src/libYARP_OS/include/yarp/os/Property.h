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


class yarp::os::Property : public Searchable {

public:
    Property();

    virtual ~Property();

    bool check(const char *key) const;

    //virtual bool check(const char *key, BottleBit *&output);

    void put(const char *key, const char *val);

    void put(const char *key, BottleBit& bit);

    virtual BottleBit& find(const char *key) {
        return get(key);
    }

    virtual Bottle& findGroup(const char *key) {
        Bottle *result = getList(key);
        if (result!=((Bottle*)0)) { return *result; }
        return Bottle::getNull();
    }

    BottleBit& get(const char *key) const;
    Bottle *getList(const char *key) const;
    ConstString getString(const char *key) const;

    void clear();

    void fromString(const char *txt);
    
    void fromCommand(int argc, char *argv[], bool skipFirst=true);

    void fromConfigFile(const char *fname);

    void fromConfig(const char *txt);

    ConstString toString() const;

private:
    void *implementation;

};

#endif


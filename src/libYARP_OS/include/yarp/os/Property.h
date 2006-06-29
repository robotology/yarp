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


/**
 * \ingroup key_class
 *
 *
 * A class for storing options and configuration information.  
 * Use put() to add keyword/value pairs, and get() or check() 
 * to look them up afterwards.
 * It can
 * read from configuration files using the fromConfigFile() method,
 * and from command line options using the fromCommand() method, and
 * from any Searchable object (include Bottle objects) using the
 * fromString() method.
 * Property objects can be searched efficiently.
 *
 */
class yarp::os::Property : public Searchable {

public:
    using Searchable::check;

    Property();

    virtual ~Property();

    bool check(const char *key) const;

    void put(const char *key, const char *val);

    void put(const char *key, Value& bit);

    void unput(const char *key);

    // documented in Searchable
    virtual Value& find(const char *key) {
        return get(key);
    }

    // documented in Searchable
    virtual Bottle& findGroup(const char *key) {
        Bottle *result = getList(key);
        if (result!=((Bottle*)0)) { return *result; }
        return Bottle::getNullBottle();
    }

    Value& get(const char *key) const;
    Bottle *getList(const char *key) const;
    ConstString getString(const char *key) const;

    void clear();

    void fromString(const char *txt);
    
    void fromCommand(int argc, char *argv[], bool skipFirst=true);

    void fromConfigFile(const char *fname);

    void fromConfig(const char *txt);

    // documented in Searchable
    ConstString toString() const;

private:
    void *implementation;

};

#endif


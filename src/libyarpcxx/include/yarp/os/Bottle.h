// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef YARPCXX_Bottle_INC
#define YARPCXX_Bottle_INC

#include <yarp/yarpcxx.h>

namespace yarpcxx {
    namespace os {
        class Bottle;
    }
}

class yarpcxx::os::Bottle : public Portable {
public:
    Bottle() {
        yarpBottleInit(getHandle());
        getHandle()->client = this;
    }

    virtual ~Bottle() {
        // yarpPortableFini is enough
    }

    virtual bool write(ConnectionWriter& connection) {
        return !yarpBottleWrite(getHandle(),connection.getHandle());
    }

    virtual bool read(ConnectionReader& connection) { 
        return !yarpBottleRead(getHandle(),connection.getHandle());
    }

    void addInt(int x) {
        yarpBottleAddInt(getHandle(),x);
    }
    
    void addDouble(double x) {
        yarpBottleAddDouble(getHandle(),x);
    }
    
    void addString(const char *x) {
        yarpBottleAddString(getHandle(),x);
    }

    ConstString toString() {
        fflush(stdout);
        ConstString result;
        yarpBottleToString(getHandle(),result.getHandle());
        return result;
    }

};


namespace yarp {
    using namespace yarpcxx;
}


#endif

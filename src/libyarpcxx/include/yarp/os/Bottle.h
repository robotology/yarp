/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

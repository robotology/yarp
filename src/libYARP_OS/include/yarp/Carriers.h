// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_CARRIERS_
#define _YARP2_CARRIERS_

#include <yarp/String.h>
#include <yarp/Bytes.h>
#include <yarp/Address.h>
#include <yarp/Face.h>
#include <yarp/OutputProtocol.h>
#include <yarp/Carrier.h>

#include <ace/Vector_T.h>

namespace yarp {
    class Carriers;
}

/**
 * This class is the starting point for all communication.
 */
class yarp::Carriers {
public:
    static Carrier *chooseCarrier(const String& name);
    static Carrier *chooseCarrier(const Bytes& bytes);

    static Face *listen(const Address& address); // throws IOException
    static OutputProtocol *connect(const Address& address); // throws IOException

    // msvc seems to want the destructor public, even for static private instance
    virtual ~Carriers();

private:
    ACE_Vector<Carrier *> delegates;

    Carriers();
  
    static Carriers instance;
    static Carriers& getInstance() {
        return instance;
    }

    Carrier *chooseCarrier(const String * name, const Bytes * bytes);  
};


#endif



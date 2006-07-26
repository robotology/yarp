// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/NetType.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/NetFloat64.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::os;

class NetTypeTest : public UnitTest {
public:
    virtual String getName() { return "NetTypeTest"; }

    void checkCrc() {
        report(0,"checking cyclic redundancy check is plausible");

        char buf[] = { 1, 2, 3, 4, 5 };
        char buf2[] = { 1, 2, 3, 4, 5 };
        int len = 5;
    
        unsigned long ct1 = NetType::getCrc(buf,len);
        unsigned long ct2 = NetType::getCrc(buf2,len);
        checkEqual(ct1,ct2,"two identical sequences");
    
        buf[0] = 4;
        ct1 = NetType::getCrc(buf,len);
        checkTrue(ct1!=ct2,"two different sequences");

        buf2[0] = 4;
        ct2 = NetType::getCrc(buf2,len);
        checkTrue(ct1==ct2,"two identical sequences again");
    }

    void checkInt() {
        report(0,"checking integer representation");
        union {
            NetInt32 i;
            unsigned char c[4];
        } val;
        val.i = 258;
        checkEqual(val.c[0],2,"first byte ok");
        checkEqual(val.c[1],1,"second byte ok");
        checkEqual(val.c[2],0,"third byte ok");
        checkEqual(val.c[3],0,"fourth byte ok");
    }

    void checkFloat() {
        report(0,"checking floating point representation");
        union {
            double d;
            unsigned char c[8];
        } val;
        NetFloat64 d = 3.14159;
        memcpy((char*)(&val.d),(char*)&d,sizeof(double));
        unsigned char rpi[8] = {
            110, 134, 27, 240, 249, 33, 9, 64
        };
        for (int i=0; i<8; i++) {
            val.c[i] = rpi[i];
        }
        checkTrue((val.d>3.14),"pi lower bound");
        checkTrue((val.d<3.15),"pi upper bound");
    }

    virtual void runTests() {
        checkCrc();
        checkInt();
        checkFloat();
    }
};

static NetTypeTest theNetTypeTest;

UnitTest& getNetTypeTest() {
    return theNetTypeTest;
}

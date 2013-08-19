// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/NetType.h>
#include <yarp/os/NetInt16.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/NetFloat64.h>

#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/impl/PlatformStdlib.h>
//#include "TestList.h"

using namespace yarp::os::impl;
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
            YARP_INT32 i;
            unsigned char c[sizeof(YARP_INT32)];
        } val;
        NetInt32 i = 258;
        checkEqual(sizeof(YARP_INT32),4,"integer size is ok");
        memcpy((char*)(&val.i),(char*)&i,sizeof(YARP_INT32));
        checkEqual(val.c[0],2,"first byte ok");
        checkEqual(val.c[1],1,"second byte ok");
        checkEqual(val.c[2],0,"third byte ok");
        checkEqual(val.c[3],0,"fourth byte ok");
    }

    void checkInt16() {
        report(0,"checking 16-bit integer representation");
        union {
            YARP_INT16 i;
            unsigned char c[sizeof(YARP_INT16)];
        } val;
        NetInt16 i = 258;
        checkEqual(sizeof(YARP_INT16),2,"integer size is ok");
        memcpy((char*)(&val.i),(char*)&i,sizeof(YARP_INT16));
        checkEqual(val.c[0],2,"first byte ok");
        checkEqual(val.c[1],1,"second byte ok");
    }

    void checkFloat() {
        report(0,"checking floating point representation");
        NetFloat64 d = 99;
        checkEqual(sizeof(NetFloat64), 8, "NetFloat64 size is ok.");
        unsigned char rpi[8] = {
            110, 134, 27, 240, 249, 33, 9, 64
        };
        for (int i=0; i<8; i++) {
            ((unsigned char*)(&d))[i] = rpi[i];
        }
        checkTrue(((double)(d)>3.14),"pi lower bound");
        checkTrue(((double)(d)<3.15),"pi upper bound");
    }


    virtual void runTests() {
        checkCrc();
        checkInt();
        checkInt16();
        checkFloat();
    }
};

static NetTypeTest theNetTypeTest;

UnitTest& getNetTypeTest() {
    return theNetTypeTest;
}

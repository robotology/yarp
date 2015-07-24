// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/String.h>
#include <yarp/os/Vocab.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os;
using namespace yarp::os::impl;

class VocabTest : public UnitTest {
public:
    virtual String getName() { return "VocabTest"; }

    void checkConvert() {
        report(0,"checking vocabulary conversions");
        checkEqual(VOCAB2('h','i'),Vocab::encode("hi"),"encoding");
        checkEqual(Vocab::decode(Vocab::encode("hi")).c_str(),"hi","decoding");
        checkEqual(VOCAB4('h','i','g','h'),Vocab::encode("high"),"encoding");
        checkEqual(Vocab::decode(Vocab::encode("high")).c_str(),"high","decoding");
        report(0,"checking compile-time functions");
        NetInt32 code = Vocab::encode("stop");
        switch(code) {
        case VOCAB3('s','e','t'):
            report(1,"very strange error switching");
            break;
        case VOCAB4('s','t','o','p'):
            report(0,"good switch");
            break;
        default:
            report(1,"error switching");
            break;
        }
    }

    virtual void runTests() {
        checkConvert();
    }
};

static VocabTest theVocabTest;

UnitTest& getVocabTest() {
    return theVocabTest;
}

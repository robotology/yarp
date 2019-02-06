/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <string>
#include <yarp/os/Vocab.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os;
using namespace yarp::os::impl;

class VocabTest : public UnitTest {
public:
    virtual std::string getName() const override { return "VocabTest"; }

    void checkConvert() {
        report(0,"checking vocabulary conversions");
        checkEqual(yarp::os::createVocab('h','i'),Vocab::encode("hi"),"encoding");
        checkEqual(Vocab::decode(Vocab::encode("hi")).c_str(),"hi","decoding");
        checkEqual(yarp::os::createVocab('h','i','g','h'),Vocab::encode("high"),"encoding");
        checkEqual(Vocab::decode(Vocab::encode("high")).c_str(),"high","decoding");
        report(0,"checking compile-time functions");
        NetInt32 code = Vocab::encode("stop");
        switch(code) {
        case yarp::os::createVocab('s','e','t'):
            report(1,"very strange error switching");
            break;
        case yarp::os::createVocab('s','t','o','p'):
            report(0,"good switch");
            break;
        default:
            report(1,"error switching");
            break;
        }
    }

    virtual void runTests() override {
        checkConvert();
    }
};

static VocabTest theVocabTest;

UnitTest& getVocabTest() {
    return theVocabTest;
}

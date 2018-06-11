/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <string>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os;
using namespace yarp::os::impl;

class StringTest : public UnitTest {
public:
    virtual std::string getName() const override {
        return "StringTest";
    }
  
    void testNulls() {
        report(0,"testing null insertion");
        std::string s;
        s += 'h';
        s += '\0';
        s += 'd';
        checkEqual((int)s.length(),3,"length with internal null");
        checkEqual(s[1],'\0',"null is there");
        checkEqual(s[2],'d',"after null");
    }


    std::string mirrorString(const std::string& txt) {
        return txt;
    }

    void testBasics() {
        report(0,"testing basics");
        std::string s = "hello";
        s += " there";
        checkEqual(s.c_str(),"hello there","+= operator");
        checkEqual((s + " you").c_str(),"hello there you","+ operator");
        std::string s2(s.c_str());
        checkEqual(s2.c_str(),"hello there","via copy");
        checkEqual(mirrorString(s).c_str(),"hello there","via copy 2");
        checkEqual((s + ":" + s).c_str(),"hello there:hello there","concat");
    }

    virtual void runTests() override {
        testNulls();
        testBasics();
    }
};

static StringTest theStringTest;

UnitTest& getStringTest() {
    return theStringTest;
}

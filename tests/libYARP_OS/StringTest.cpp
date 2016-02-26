/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/String.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os::impl;

class StringTest : public UnitTest {
public:
    virtual String getName() {
        return "StringTest";
    }
  
    void testNulls() {
        report(0,"testing null insertion");
        String s;
        s += 'h';
        s += '\0';
        s += 'd';
        checkEqual((int)s.length(),3,"length with internal null");
        checkEqual(s[1],'\0',"null is there");
        checkEqual(s[2],'d',"after null");
    }


    String mirrorString(const String& txt) {
        return txt;
    }

    void testBasics() {
        report(0,"testing basics");
        String s = "hello";
        s += " there";
        checkEqual(s.c_str(),"hello there","+= operator");
        checkEqual((s + " you").c_str(),"hello there you","+ operator");
        String s2(s.c_str());
        checkEqual(s2.c_str(),"hello there","via copy");
        checkEqual(mirrorString(s).c_str(),"hello there","via copy 2");
        checkEqual((s + ":" + s).c_str(),"hello there:hello there","concat");
    }

    virtual void runTests() {
        testNulls();
        testBasics();
    }
};

static StringTest theStringTest;

UnitTest& getStringTest() {
    return theStringTest;
}

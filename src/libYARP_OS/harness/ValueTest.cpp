// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Value.h>

//#include "TestList.h"
#include <yarp/os/impl/UnitTest.h>

using namespace yarp::os::impl;
using namespace yarp::os;

class ValueTest : public UnitTest {
public:
    virtual String getName() { return "ValueTest"; }

    void checkCopy() {
        report(0,"check value copying");
        Value v;
        v.fromString("10");
        checkTrue(v.isInt(),"integer created");
        v.fromString("10.0");
        checkTrue(v.isDouble(),"floating point number created");
        v.fromString("(1 2 3)");
        checkTrue(v.isList(),"list created");

        Value v2(10);
        Value v3 = v2;
        checkEqual(v3.asInt(),10,"copy integer");
        Value v4 = v;
        checkTrue(v4.isList(),"list copied");
        checkEqual(v4.asList()->get(1).asInt(),2,"right integer present");

        Bottle b("(x 10) (y 42)");
        checkEqual(b.check("x",Value(5)).asInt(),10,"default not used");
        checkEqual(b.check("xx",Value(5)).asInt(),5,"default used");

        Value& vnull = Value::getNullValue();
        Value vnull2(vnull);
        checkTrue(vnull.isNull(), "null check 1 ok");
        checkTrue(vnull2.isNull(), "null check 2 ok");
    }

    void checkMixedCopy() {
        {
            Value a(0);
            Value b("zero");
            
            a = b;
            
            checkEqual(a.asString().c_str(),"zero","string asString ok");
            checkEqual(a.toString().c_str(),"zero","string toString ok");
        }


        {
            Value a("zero");
            Value b(0);

            a = b;

            checkEqual(a.asString().c_str(),"","int asString ok");
            checkEqual(a.toString().c_str(),"0","int toString ok");
            checkEqual(a.asInt(),0,"int asInt ok");
        }


        {
            Value a("zero");
            Value b(5);

            a = b;

            checkEqual(a.asString().c_str(),"","int asString ok");
            checkEqual(a.toString().c_str(),"5","int toString ok");
            checkEqual(a.asInt(),5,"int asInt ok");
        }
    }

    void checkReadWrite() {
        report(0,"check read/write");
        Value v(4.2);
        Bottle b;
        b.read(v);
        checkTrue(b.get(0).isDouble(),"structure ok");
        checkEqualish(b.get(0).asDouble(),4.2,"value ok");
        Value v2;
        b.write(v2);
        checkEqualish(v2.asDouble(),4.2,"value reread ok");
    }

    void checkAssignment() {
        report(0,"check assignment operator");
        {
            Value v;
            Bottle b("10 (1 2 3) 20");
            v = b.get(1);
            checkTrue(v.isList(),"type ok");
            checkEqual(v.asList()->size(),3,"length ok");
        }
        {
            Value v;
            Bottle b("(10 (1 2 3) 20)");
            v = b.get(0).asList()->get(1);
            checkTrue(v.isList(),"type ok");
            checkEqual(v.asList()->size(),3,"length ok");
        }
        {
            Value v(1);
            Bottle b("10 (1 2 3) 20");
            v = b.get(1);
            checkTrue(v.isList(),"type ok");
            checkEqual(v.asList()->size(),3,"length ok");
        }
    }

    virtual void runTests() {
        checkCopy();
        checkMixedCopy();
        checkReadWrite();
        checkAssignment();
    }
};

static ValueTest theValueTest;

UnitTest& getValueTest() {
    return theValueTest;
}

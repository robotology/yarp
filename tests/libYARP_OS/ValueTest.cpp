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

    void checkInt64() {
        report(0,"check int64");
        {
            Bottle b1, b2;
            b1.addInt64(42);
            b1.addInt(43);
            b2.read(b1);
            checkTrue(b2.get(0).isInt64(),"0 type ok");
            checkFalse(b2.get(0).isInt(),"0 type not 32 bit");
            checkEqual((int)b2.get(0).asInt64(),42,"0 value ok");
            checkTrue(b2.get(1).isInt(),"1 type is 32 bit");
            checkTrue(b2.get(1).isInt64(),"1 type ok");
            checkEqual(b2.get(1).asInt(),43,"1 value ok");
            checkEqual((int)b2.get(1).asInt64(),43,"1 value as 64-bit ok");
            checkEqual(b2.toString(),"42 43","string is ok");
            b1.fromString("64 128");
            checkEqual((int)b1.get(0).asInt64(),64,"32 bit reads ok as 64");
        }
    }

    void checkEqualityOperator() {
        report(0,"check equality operator");

        {
            Value v1(10);
            Value v2(10);
            checkTrue(v2.isInt(),"(ctor) type ok");
            checkTrue((v1==v2), "(ctor) operator== ok");
        }

        {
            Value v1(10);
            Value v2(v1);
            checkTrue(v2.isInt(),"(copy) type ok");
            checkTrue((v1==v2), "(copy) operator== ok");
        }

        {
            Value v1(10);
            Value v2;
            v2.fromString("10");
            checkTrue(v2.isInt(),"(\"10\") type ok");
            checkTrue((v1==v2), "(\"10\") operator== ok");

            v2.fromString("15");
            checkTrue(v2.isInt(),"(\"15\") type ok");
            checkTrue((v1!=v2), "(\"15\") operator!= ok");

            v2.fromString("10.0");
            checkTrue(v2.isDouble(),"(\"10.0\") type ok");
            checkTrue((v1!=v2), "(\"10.0\") operator!= ok"); // FIXME why not?
            checkTrue((v1.asInt()==v2.asInt()), "(\"10.0\") value ok");

            v2.fromString("(10)");
            checkTrue(v2.isList(),"(\"(10)\") type ok");
            checkTrue((v1==v2), "(\"(10)\") operator== ok");

            v2.fromString("(15)");
            checkTrue(v2.isList(),"(\"(15)\") type ok");
            checkTrue((v1!=v2), "(\"(15)\") operator!= ok");

            v2.fromString("(10 15)");
            checkTrue(v2.isList(),"(\"(10 15)\") type ok");
            checkTrue((v1!=v2), "(\"(10 15)\") operator!= ok");
            checkTrue((v1==v2.asList()->get(0)), "(\"(10 15)\") value ok");

            v2.fromString("(10.0)");
            checkTrue(v2.isList(),"(\"(10.0)\") type ok");
            checkTrue((v1!=v2), "(\"(10.0)\") operator!= ok");
            checkTrue((v1!=v2.asList()->get(0)), "(\"(10.0)\") value ok");  // FIXME why not?
            checkTrue((v1==v2.asList()->get(0).asInt()), "(\"(10.0)\") value ok");

            v2.fromString("\"10\"");
            checkTrue(v2.isString(),"(\"\\\"10\\\"\") type ok");
            checkTrue((v1==v2), "(\"\\\"10\\\"\") operator== ok");

            v2.fromString("\"ten\"");
            checkTrue(v2.isString(),"(\"ten\") type ok");
            checkTrue((v1!=v2), "(\"ten\") operator!= ok");

            v2.fromString("true");
            checkTrue(v2.isBool(),"(\"true\") type ok");
            checkTrue((v1!=v2), "(\"true\") operator!= ok");
        }

        {
            Value v1(10);
            Bottle b1;
            b1.addInt64(10);
            Value v2 = b1.get(0);
            checkTrue(v2.isInt64(),"(int64) type ok");
            checkTrue((v1==v2), "(int64) operator!= ok");
        }

        {
            Value v1(10);
            Value v2(10, true);
            checkTrue(v2.isVocab(),"(vocab) type ok");
            checkTrue((v1!=v2), "(vocab) operator!= ok");
        }

        {
            Value v1(10);
            int i = 10;
            Value v2(&i, sizeof(int));
            checkTrue(v2.isBlob(),"(blob) type ok");
            checkTrue((v1!=v2), "(blob) operator!= ok");
            checkTrue((v1==*(reinterpret_cast<const int*>(v2.asBlob()))), "(blob) value ok");
        }

        {
            Value v1(15);
            int i = 10;
            Value v2(&i, sizeof(int));
            checkTrue(v2.isBlob(),"(blob) type ok");
            checkTrue((v1!=v2), "(blob) operator!= ok");
            checkTrue((v1!=*(reinterpret_cast<const int*>(v2.asBlob()))), "(blob) value ok");
        }
    }

    virtual void runTests() {
        checkCopy();
        checkMixedCopy();
        checkReadWrite();
        checkAssignment();
        checkInt64();
        checkEqualityOperator();
    }
};

static ValueTest theValueTest;

UnitTest& getValueTest() {
    return theValueTest;
}

/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Value.h>

//#include "TestList.h"
#include <yarp/os/impl/UnitTest.h>


#define testBoolFromString(val) \
{ \
    Value v; \
    v.fromString(#val); \
    checkTrue(v.isBool(), #val " is a bool"); \
    checkFalse(v.isInt(), #val " is not an int"); \
    checkFalse(v.isDouble(), #val " is not a double"); \
    checkFalse(v.isString(), #val " is not a string"); \
    checkEqual(v.asBool(), val, #val " bool value ok"); \
}

#define testIntFromString(val) \
{ \
    Value v; \
    v.fromString(#val); \
    checkFalse(v.isBool(), #val " is not a bool"); \
    checkTrue(v.isInt(), #val " is an int"); \
    checkFalse(v.isDouble(), #val " is not a double"); \
    checkFalse(v.isString(), #val " is not a string"); \
    checkEqual(v.asInt(), val, #val " int value ok"); \
}

#define testDoubleFromString(val) \
{ \
    Value v; \
    v.fromString(#val); \
    checkFalse(v.isBool(), #val " is not a bool"); \
    checkFalse(v.isInt(), #val " is not an int"); \
    checkTrue(v.isDouble(), #val " is a double"); \
    checkFalse(v.isString(), #val " is not a string"); \
    checkEqual(v.asDouble(), val, #val " double value ok"); \
}

#define testStringFromString(val) \
{ \
    Value v; \
    v.fromString(val); \
    checkFalse(v.isBool(), val " is not a bool"); \
    checkFalse(v.isInt(), val " is not an int"); \
    checkFalse(v.isDouble(), val " is not a double"); \
    checkTrue(v.isString(), val " is a string"); \
    checkEqual(v.asString(), val, val " string value ok"); \
    checkEqual(v.toString(), val, val " string value ok"); \
}

using namespace yarp::os::impl;
using namespace yarp::os;

class ValueTest : public UnitTest {
public:
    virtual ConstString getName() override { return "ValueTest"; }

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

    void checkNumericFromString() {
        report(0,"check numeric types");

        testBoolFromString(true);
        testBoolFromString(false);

        testIntFromString(0);
        testIntFromString(1);
        testIntFromString(0x00);
        testIntFromString(0xFF);
//        testIntFromString(10l);

        testDoubleFromString(0.0);
        testDoubleFromString(1.0);
        testDoubleFromString(.0);
        testDoubleFromString(.1);
        testDoubleFromString(1e1);
        testDoubleFromString(1e-1);
        testDoubleFromString(-1e1);
        testDoubleFromString(0.1);
        testDoubleFromString(0.01);
        testDoubleFromString(0.001);
        testDoubleFromString(0.0001);
        testDoubleFromString(0.00001);
        testDoubleFromString(0.000001);
        testDoubleFromString(0.0000001);
        testDoubleFromString(0.00000001);
        testDoubleFromString(0.000000001);
        testDoubleFromString(0.0000000001);
        testDoubleFromString(0.00000000001);
        testDoubleFromString(0.000000000001);
        testDoubleFromString(0.0000000000001);
//        testDoubleFromString(0.1f);

        testStringFromString("foo");
        testStringFromString("e-e");
//        testStringFromString(".1.1");
//        testStringFromString(".1-1");
//        testStringFromString("-1-1");
//        testStringFromString("1-1");
        testStringFromString("1foo");
    }

    virtual void runTests() override {
        checkCopy();
        checkMixedCopy();
        checkReadWrite();
        checkAssignment();
        checkInt64();
        checkEqualityOperator();
        checkNumericFromString();
    }
};

static ValueTest theValueTest;

UnitTest& getValueTest() {
    return theValueTest;
}

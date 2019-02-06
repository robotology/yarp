/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Value.h>

//#include "TestList.h"
#include <yarp/os/impl/UnitTest.h>
#include <iostream>

#define testBoolFromString(val) \
{ \
    Value v; \
    v.fromString(#val); \
    checkTrue(v.isBool(), #val " is a bool"); \
    checkFalse(v.isInt32(), #val " is not an int"); \
    checkFalse(v.isFloat64(), #val " is not a double"); \
    checkFalse(v.isString(), #val " is not a string"); \
    checkEqual(v.asBool(), val, #val " bool value ok"); \
}

#define testIntFromString(val) \
{ \
    Value v; \
    v.fromString(#val); \
    checkFalse(v.isBool(), #val " is not a bool"); \
    checkTrue(v.isInt32(), #val " is an int"); \
    checkFalse(v.isFloat64(), #val " is not a double"); \
    checkFalse(v.isString(), #val " is not a string"); \
    checkEqual(v.asInt32(), val, #val " int value ok"); \
}

#define testDoubleFromString(val) \
{ \
    Value v; \
    v.fromString(#val); \
    checkFalse(v.isBool(), #val " is not a bool"); \
    checkFalse(v.isInt32(), #val " is not an int"); \
    checkTrue(v.isFloat64(), #val " is a double"); \
    checkFalse(v.isString(), #val " is not a string"); \
    checkEqual(v.asFloat64(), val, #val " double value ok"); \
}

#define testStringFromString(val) \
{ \
    Value v; \
    v.fromString(val); \
    checkFalse(v.isBool(), val " is not a bool"); \
    checkFalse(v.isInt32(), val " is not an int"); \
    checkFalse(v.isFloat64(), val " is not a double"); \
    checkTrue(v.isString(), val " is a string"); \
    checkEqual(v.asString(), val, val " string value ok"); \
    checkEqual(v.toString(), val, val " string value ok"); \
}

using namespace yarp::os::impl;
using namespace yarp::os;

class ValueTest : public UnitTest {
public:
    virtual std::string getName() const override { return "ValueTest"; }

    void checkCopy() {
        report(0,"check value copying");
        Value v;
        v.fromString("10");
        checkTrue(v.isInt32(),"integer created");
        v.fromString("10.0");
        checkTrue(v.isFloat64(),"floating point number created");
        v.fromString("(1 2 3)");
        checkTrue(v.isList(),"list created");

        Value v2(10);
        Value v3 = v2;
        checkEqual(v3.asInt32(),10,"copy integer");
        Value v4 = v;
        checkTrue(v4.isList(),"list copied");
        checkEqual(v4.asList()->get(1).asInt32(),2,"right integer present");

        Bottle b("(x 10) (y 42)");
        checkEqual(b.check("x",Value(5)).asInt32(),10,"default not used");
        checkEqual(b.check("xx",Value(5)).asInt32(),5,"default used");

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
            checkEqual(a.asInt32(),0,"int asInt32 ok");
        }


        {
            Value a("zero");
            Value b(5);

            a = b;

            checkEqual(a.asString().c_str(),"","int asString ok");
            checkEqual(a.toString().c_str(),"5","int toString ok");
            checkEqual(a.asInt32(),5,"int asInt32 ok");
        }
    }

    void checkReadWrite() {
        report(0,"check read/write");
        Value v(4.2);
        Bottle b;
        b.read(v);
        checkTrue(b.get(0).isFloat64(),"structure ok");
        checkEqualish(b.get(0).asFloat64(),4.2,"value ok");
        Value v2;
        b.write(v2);
        checkEqualish(v2.asFloat64(),4.2,"value reread ok");
    }

    void checkAssignment() {
        report(0,"check assignment operator");
        {
            Value v;
            Bottle b("10 (1 2 3) 20");
            v = b.get(1);
            checkTrue(v.isList(),"type ok");
            checkEqual(v.asList()->size(),(size_t) 3,"length ok");
        }
        {
            Value v;
            Bottle b("(10 (1 2 3) 20)");
            v = b.get(0).asList()->get(1);
            checkTrue(v.isList(),"type ok");
            checkEqual(v.asList()->size(),(size_t) 3,"length ok");
        }
        {
            Value v(1);
            Bottle b("10 (1 2 3) 20");
            v = b.get(1);
            checkTrue(v.isList(),"type ok");
            checkEqual(v.asList()->size(),(size_t) 3,"length ok");
        }
    }

    void checkInt64() {
        report(0,"check int64");
        {
            Bottle b1, b2;
            b1.addInt64(42);
            b1.addInt32(43);
            b2.read(b1);
            checkTrue(b2.get(0).isInt64(),"0 type is 64 bit");
            checkFalse(b2.get(0).isInt32(),"0 type is not 32 bit");
            checkEqual((int)b2.get(0).asInt64(),42,"0 value ok");
            checkTrue(b2.get(1).isInt32(),"1 type is 32 bit");
            checkFalse(b2.get(1).isInt64(),"1 type is not 64 bit");
            checkEqual(b2.get(1).asInt32(),43,"1 value ok");
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
            checkTrue(v2.isInt32(),"(ctor) type ok");
            checkTrue((v1==v2), "(ctor) operator== ok");
        }

        {
            Value v1(10);
            Value v2(v1);
            checkTrue(v2.isInt32(),"(copy) type ok");
            checkTrue((v1==v2), "(copy) operator== ok");
        }

        {
            Value v1(10);
            Value v2;
            v2.fromString("10");
            checkTrue(v2.isInt32(),"(\"10\") type ok");
            checkTrue((v1==v2), "(\"10\") operator== ok");

            v2.fromString("15");
            checkTrue(v2.isInt32(),"(\"15\") type ok");
            checkTrue((v1!=v2), "(\"15\") operator!= ok");

            v2.fromString("10.0");
            checkTrue(v2.isFloat64(),"(\"10.0\") type ok");
            checkTrue((v1!=v2), "(\"10.0\") operator!= ok"); // FIXME why not?
            checkTrue((v1.asInt32()==v2.asInt32()), "(\"10.0\") value ok");

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
            checkTrue((v1.asInt32()==v2.asList()->get(0).asInt32()), "(\"(10.0)\") value ok");

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
            checkTrue((v1.asInt32() == *(reinterpret_cast<const int*>(v2.asBlob()))), "(blob) value ok");
        }

        {
            Value v1(15);
            int i = 10;
            Value v2(&i, sizeof(int));
            checkTrue(v2.isBlob(),"(blob) type ok");
            checkTrue((v1!=v2), "(blob) operator!= ok");
            checkTrue((v1.asInt32() != *(reinterpret_cast<const int*>(v2.asBlob()))), "(blob) value ok");
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



#if !defined(_WIN32)
constexpr static bool colored_output = true;
#else
constexpr static bool colored_output = false;
#endif

#define xGREY    std::string(colored_output ? "\033[01;30m" : "")
#define xRED     std::string(colored_output ? "\033[01;31m" : "")
#define xGREEN   std::string(colored_output ? "\033[01;32m" : "")
#define xYELLOW  std::string(colored_output ? "\033[01;33m" : "")
#define xBLUE    std::string(colored_output ? "\033[01;34m" : "")
#define xMAGENTA std::string(colored_output ? "\033[01;35m" : "")
#define xCYAN    std::string(colored_output ? "\033[01;36m" : "")
#define xWHITE   std::string(colored_output ? "\033[01;37m" : "")
#define xCLEAR   std::string(colored_output ? "\033[00m" : "")

#define GREY(x)    (xGREY    + #x + xCLEAR)
#define RED(x)     (xRED     + #x + xCLEAR)
#define GREEN(x)   (xGREEN   + #x + xCLEAR)
#define YELLOW(x)  (xYELLOW  + #x + xCLEAR)
#define BLUE(x)    (xBLUE    + #x + xCLEAR)
#define MAGENTA(x) (xMAGENTA + #x + xCLEAR)
#define CYAN(x)    (xCYAN    + #x + xCLEAR)
#define WHITE(x)   (xWHITE   + #x + xCLEAR)

# define PRINT_BOOL(x) (x ? GREEN(true) : RED(false))
# define PRINT_IF(cond, x, y) (cond ? xWHITE + std::to_string(x) + xCLEAR : GREY(y))

#define printIsType(T, x) \
{ \
    yarp::os::Value* v = yarp::os::Value::make ## T(0); \
    std::cout << YELLOW(T) << " \t" << xGREY << sizeof(x) * 8 << "-bit" << xCLEAR << "\t\t" \
              << PRINT_BOOL(v->isBool())    << "\t\t" \
              << PRINT_BOOL(v->isInt8())    << "\t\t" \
              << PRINT_BOOL(v->isInt16())   << "\t\t" \
              << PRINT_BOOL(v->isInt32())   << "\t\t" \
              << PRINT_BOOL(v->isInt())     << "\t\t" \
              << PRINT_BOOL(v->isInt64())   << "\t\t" \
              << PRINT_BOOL(v->isFloat32()) << "\t\t" \
              << PRINT_BOOL(v->isFloat64()) << "\t\t" \
              << PRINT_BOOL(v->isDouble())  << "\t\t" \
              << PRINT_BOOL(v->isString())  << "\n"; \
    delete v; \
}

#define printAsType(T, val) \
{ \
    yarp::os::Value* v = yarp::os::Value::make ## T(val); \
    std::cout << YELLOW(T) << " \t" << xGREY << #val << xCLEAR << "\t\t" \
              << PRINT_BOOL(v->asBool()) << "\t\t" \
              << std::to_string(v->asInt8())    << "\t\t" \
              << std::to_string(v->asInt16())   << "\t\t" \
              << std::to_string(v->asInt32())   << "\t\t" \
              << std::to_string(v->asInt())     << "\t\t" \
              << std::to_string(v->asInt64())   << "\t\t" \
              << std::to_string(v->asFloat32()) << "\t" \
              << std::to_string(v->asFloat64()) << "\t" \
              << std::to_string(v->asDouble())  << "\t" \
              << GREY([) << xWHITE << v->asString() << xCLEAR << GREY(]) << "\t\t" \
              << GREY([) << xWHITE << v->toString() << xCLEAR << GREY(]) << "\n"; \
    delete v; \
}

    // Not a real test, just an useful printed table, that allows to check the
    // result of all the calls.
    void printValueTable()
    {

// This method tests all the unsafe methods, therefore the warning is disabled
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING

        std::cout << "\n\t\t\t\t"
                  << CYAN(isBool)    << "\t\t"
                  << CYAN(isInt8)    << "\t\t"
                  << CYAN(isInt16)   << "\t\t"
                  << CYAN(isInt32)   << "\t\t"
                  << CYAN(isInt)     << "\t\t"
                  << CYAN(isInt64)   << "\t\t"
                  << CYAN(isFloat32) << "\t"
                  << CYAN(isFloat64) << "\t"
                  << CYAN(isDouble)  << "\t"
                  << CYAN(isString)  << "\n";

        printIsType(Int8, std::int8_t);
        printIsType(Int16, std::int16_t);
        printIsType(Int32, std::int32_t);
        printIsType(Int, int);
        printIsType(Int64, std::int64_t);
        printIsType(Float32, yarp::conf::float32_t);
        printIsType(Float64, yarp::conf::float64_t);
        printIsType(Double, double);


        std::cout << "\n\t\t\t\t"
                  << BLUE(asBool)    << "\t\t"
                  << BLUE(asInt8)    << "\t\t"
                  << BLUE(asInt16)   << "\t\t"
                  << BLUE(asInt32)   << "\t\t"
                  << BLUE(asInt)     << "\t\t"
                  << BLUE(asInt64)   << "\t\t"
                  << BLUE(asFloat32) << "\t"
                  << BLUE(asFloat64) << "\t"
                  << BLUE(asDouble)  << "\t"
                  << BLUE(asString)  << "\t"
                  << MAGENTA(toString)  << "\n";

        printAsType(Int8, 8);
        printAsType(Int16, 16);
        printAsType(Int32, 32);
        printAsType(Int, 32);
        printAsType(Int64, 64);
        printAsType(Float32, 32.01f);
        printAsType(Float64, 64.01);
        printAsType(Double, 64.01);


        std::cout << "\n\t\t\t\t"
                  << BLUE(asBool)    << "\t\t"
                  << BLUE(asInt8)    << "\t\t"
                  << BLUE(asInt16)   << "\t\t"
                  << BLUE(asInt32)   << "\t\t"
                  << BLUE(asInt)     << "\t\t"
                  << BLUE(asInt64)   << "\t\t"
                  << BLUE(asFloat32) << "\t"
                  << BLUE(asFloat64) << "\t"
                  << BLUE(asDouble)  << "\t"
                  << BLUE(asString)  << "\t"
                  << MAGENTA(toString)  << "\n";

        printAsType(Int8, 0);
        printAsType(Int16, 0);
        printAsType(Int32, 0);
        printAsType(Int, 0);
        printAsType(Int64, 0);
        printAsType(Float32, 0.0f);
        printAsType(Float64, 0.0);
        printAsType(Double, 0.0);

        // Values that might not behave as expected:
        std::cout << "\n\t\t\t\t"
                  << BLUE(asBool)    << "\t\t"
                  << BLUE(asInt8)    << "\t\t"
                  << BLUE(asInt16)   << "\t\t"
                  << BLUE(asInt32)   << "\t\t"
                  << BLUE(asInt)     << "\t\t"
                  << BLUE(asInt64)   << "\t\t"
                  << BLUE(asFloat32) << "\t"
                  << BLUE(asFloat64) << "\t"
                  << BLUE(asDouble)  << "\t"
                  << BLUE(asString)  << "\t"
                  << MAGENTA(toString)  << "\n";

        printAsType(Int16, 127);
        printAsType(Int16, 128);
        printAsType(Int16, 256);
        printAsType(Int16, 257);
        printAsType(Int32, 32767);
        printAsType(Int32, 32768);
        printAsType(Int32, 65535);
        printAsType(Float32, 31.99f);
        printAsType(Float64, 63.99);
        printAsType(Double, 63.99);

        std::cout << std::flush;
YARP_WARNING_POP
    }

    virtual void runTests() override {
        checkCopy();
        checkMixedCopy();
        checkReadWrite();
        checkAssignment();
        checkInt64();
        checkEqualityOperator();
        checkNumericFromString();

        printValueTable();
    }
};

static ValueTest theValueTest;

UnitTest& getValueTest() {
    return theValueTest;
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Value.h>

#include <iostream>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;


TEST_CASE("os::ValueTest", "[yarp::os]")
{
    SECTION("Check value copying")
    {
        Value v;
        v.fromString("10");
        CHECK(v.isInt32()); // integer created
        v.fromString("10.0");
        CHECK(v.isFloat64()); // floating point number created
        v.fromString("(1 2 3)");
        CHECK(v.isList()); // list created

        Value v2(10);
        Value v3 = v2;
        CHECK(v3.asInt32() == 10); // copy integer
        Value v4 = v;
        CHECK(v4.isList()); // list copied
        CHECK(v4.asList()->get(1).asInt32() == 2); // right integer present

        Bottle b("(x 10) (y 42)");
        CHECK(b.check("x", Value(5)).asInt32() == 10); // default not used
        CHECK(b.check("xx", Value(5)).asInt32() == 5); // default used

        Value& vnull = Value::getNullValue();
        Value vnull2(vnull);
        CHECK(vnull.isNull()); // null check 1 ok
        CHECK(vnull2.isNull()); // null check 2 ok
    }

    SECTION("Check mixed copy (test 1)")
    {
        Value a(0);
        Value b("zero");

        a = b;

        CHECK(a.asString() == "zero"); // string asString ok
        CHECK(a.toString() == "zero"); // string toString ok
    }

    SECTION("Check mixed copy (test 2)")
    {
        Value a("zero");
        Value b(0);

        a = b;

        CHECK(a.asString() == ""); // int asString ok
        CHECK(a.toString() == "0"); // int toString ok
        CHECK(a.asInt32() == 0); // int asInt32 ok
    }


    SECTION("Check mixed copy (test 3)")
    {
        Value a("zero");
        Value b(5);

        a = b;

        CHECK(a.asString() == ""); // int asString ok
        CHECK(a.toString() == "5"); // int toString ok
        CHECK(a.asInt32() == 5); // int asInt32 ok
    }

    SECTION("Check read/write")
    {
        Value v(4.2);
        Bottle b;
        b.read(v);
        CHECK(b.get(0).isFloat64()); // structure ok
        CHECK(b.get(0).asFloat64() == Approx(4.2)); // value ok
        Value v2;
        b.write(v2);
        CHECK(v2.asFloat64() == Approx(4.2)); // value reread ok
    }

    SECTION("Check assignment operator (test 1)")
    {
        Value v;
        Bottle b("10 (1 2 3) 20");
        v = b.get(1);
        CHECK(v.isList()); // type ok
        CHECK(v.asList()->size() == (size_t) 3); // length ok
    }

    SECTION("Check assignment operator (test 2)")
    {
        Value v;
        Bottle b("(10 (1 2 3) 20)");
        v = b.get(0).asList()->get(1);
        CHECK(v.isList()); // type ok
        CHECK(v.asList()->size() == (size_t) 3); // length ok
    }

    SECTION("Check assignment operator (test 3)")
    {
        Value v(1);
        Bottle b("10 (1 2 3) 20");
        v = b.get(1);
        CHECK(v.isList()); // type ok
        CHECK(v.asList()->size() == (size_t) 3); // length ok
    }

    SECTION("Check int64")
    {
        Bottle b1, b2;
        b1.addInt64(42);
        b1.addInt32(43);
        b2.read(b1);
        CHECK(b2.get(0).isInt64()); // 0 type is 64 bit
        CHECK_FALSE(b2.get(0).isInt32()); // 0 type is not 32 bit
        CHECK((int)b2.get(0).asInt64() == 42); // 0 value ok
        CHECK(b2.get(1).isInt32()); // 1 type is 32 bit
        CHECK_FALSE(b2.get(1).isInt64()); // 1 type is not 64 bit
        CHECK(b2.get(1).asInt32() == 43); // 1 value ok
        CHECK((int)b2.get(1).asInt64() == 43); // 1 value as 64-bit ok
        CHECK(b2.toString() == "42 43"); // string is ok
        b1.fromString("64 128");
        CHECK((int)b1.get(0).asInt64() == 64); // 32 bit reads ok as 64
    }

    SECTION("Check equality operator (test 1)")
    {
        Value v1(10);
        Value v2(10);
        CHECK(v2.isInt32()); // (ctor) type ok
        CHECK(v1 == v2); // (ctor) operator== ok
    }

    SECTION("Check equality operator (test 2)")
    {
        Value v1(10);
        Value v2(v1);
        CHECK(v2.isInt32()); // (copy) type ok
        CHECK(v1 == v2); // (copy) operator== ok
    }

    SECTION("Check equality operator (test 3)")
    {
        Value v1(10);
        Value v2;
        v2.fromString("10");
        CHECK(v2.isInt32()); // [10] type ok
        CHECK(v1 == v2); // [10] operator== ok

        v2.fromString("15");
        CHECK(v2.isInt32()); // [15] type ok
        CHECK(v1 != v2); // [15] operator!= ok

        v2.fromString("10.0");
        CHECK(v2.isFloat64()); // [10.0] type ok
        CHECK(v1 != v2); // [10.0] operator!= ok // FIXME why not?
        CHECK(v1.asInt32() == v2.asInt32()); // [10.0] value ok

        v2.fromString("(10)");
        CHECK(v2.isList()); // [(10)] type ok
        CHECK(v1 == v2); // [(10)] operator== ok

        v2.fromString("(15)");
        CHECK(v2.isList()); // [(15)] type ok
        CHECK(v1 != v2); // [(15)] operator!= ok

        v2.fromString("(10 15)");
        CHECK(v2.isList()); // [(10 15)] type ok
        CHECK(v1 != v2); // [(10 15)] operator!= ok
        CHECK(v1 == v2.asList()->get(0)); // [(10 15)] value ok

        v2.fromString("(10.0)");
        CHECK(v2.isList()); // [(10.0)] type ok
        CHECK(v1 != v2); // [(10.0)] operator!= ok
        CHECK(v1 != v2.asList()->get(0)); // [(10.0)] value ok  // FIXME why not?
        CHECK(v1.asInt32() == v2.asList()->get(0).asInt32()); // [(10.0)] value ok

        v2.fromString("\"10\"");
        CHECK(v2.isString()); // ["10"] type ok
        CHECK(v1 == v2); // ["10"] operator== ok

        v2.fromString("\"ten\"");
        CHECK(v2.isString()); // [ten] type ok
        CHECK(v1 != v2); // [ten] operator!= ok

        v2.fromString("true");
        CHECK(v2.isBool()); // [true] type ok
        CHECK(v1 != v2); // [true] operator!= ok
    }

    SECTION("Check equality operator (test 4)")
    {
        Value v1(10);
        Bottle b1;
        b1.addInt64(10);
        Value v2 = b1.get(0);
        CHECK(v2.isInt64()); // (int64) type ok
        CHECK(v1 == v2); // (int64) operator!= ok
    }

    SECTION("Check equality operator (test 5)")
    {
        Value v1(10);
        Value v2(10, true);
        CHECK(v2.isVocab32()); // (vocab) type ok
        CHECK(v1 != v2); // (vocab) operator!= ok
    }

    SECTION("Check equality operator (test 6)")
    {
        Value v1(10);
        int i = 10;
        Value v2(&i, sizeof(int));
        CHECK(v2.isBlob()); // (blob) type ok
        CHECK(v1 != v2); // (blob) operator!= ok
        CHECK(v1.asInt32() == *(reinterpret_cast<const int*>(v2.asBlob()))); // (blob) value ok
    }

    SECTION("Check equality operator (test 7)")
    {
        Value v1(15);
        int i = 10;
        Value v2(&i, sizeof(int));
        CHECK(v2.isBlob()); // (blob) type ok
        CHECK(v1 != v2); // (blob) operator!= ok
        CHECK(v1.asInt32() != *(reinterpret_cast<const int*>(v2.asBlob()))); // (blob) value ok
    }


    // Check numeric types

#define SECTION_testBoolFromString(val) \
    SECTION("Test bool from string [" #val "]") \
    { \
        Value v; \
        v.fromString(#val); \
        CHECK(v.isBool()); /* #val is a bool */ \
        CHECK_FALSE(v.isInt32()); /* #val is not an int */ \
        CHECK_FALSE(v.isFloat64()); /* #val is not a double */ \
        CHECK_FALSE(v.isString()); /* #val is not a string */ \
        CHECK(v.asBool() == val); /* #val " bool value ok */ \
    }

    SECTION_testBoolFromString(true);
    SECTION_testBoolFromString(false);


#define SECTION_testIntFromString(val) \
    SECTION("Test int from string [" #val "]") \
    { \
        Value v; \
        v.fromString(#val); \
        CHECK_FALSE(v.isBool()); /* #val is not a bool */ \
        CHECK(v.isInt32()); /* #val is an int */ \
        CHECK_FALSE(v.isFloat64()); /* #val is not a double */ \
        CHECK_FALSE(v.isString()); /* #val is not a string */ \
        CHECK(v.asInt32() == val); /* #val " int value ok */ \
    }

    SECTION_testIntFromString(0);
    SECTION_testIntFromString(1);
    SECTION_testIntFromString(0x00);
    SECTION_testIntFromString(0xFF);
//    SECTION_testIntFromString(10l);


#define SECTION_testDoubleFromString(val) \
    SECTION("Test double from string [" #val "]") \
    { \
        Value v; \
        v.fromString(#val); \
        CHECK_FALSE(v.isBool()); /* #val is not a bool */ \
        CHECK_FALSE(v.isInt32()); /* #val is not an int */ \
        CHECK(v.isFloat64()); /* #val is a double */ \
        CHECK_FALSE(v.isString()); /* #val is not a string */ \
        CHECK(v.asFloat64() == val); /* #val " double value ok */ \
    }

    SECTION_testDoubleFromString(0.0);
    SECTION_testDoubleFromString(1.0);
    SECTION_testDoubleFromString(.0);
    SECTION_testDoubleFromString(.1);
    SECTION_testDoubleFromString(1e1);
    SECTION_testDoubleFromString(1e-1);
    SECTION_testDoubleFromString(-1e1);
    SECTION_testDoubleFromString(0.1);
    SECTION_testDoubleFromString(0.01);
    SECTION_testDoubleFromString(0.001);
    SECTION_testDoubleFromString(0.0001);
    SECTION_testDoubleFromString(0.00001);
    SECTION_testDoubleFromString(0.000001);
    SECTION_testDoubleFromString(0.0000001);
    SECTION_testDoubleFromString(0.00000001);
    SECTION_testDoubleFromString(0.000000001);
    SECTION_testDoubleFromString(0.0000000001);
    SECTION_testDoubleFromString(0.00000000001);
    SECTION_testDoubleFromString(0.000000000001);
    SECTION_testDoubleFromString(0.0000000000001);
//    SECTION_testDoubleFromString(0.1f);


#define SECTION_testStringFromString(val) \
    SECTION("Test string from string [" val "]") \
    { \
        Value v; \
        v.fromString(val); \
        CHECK_FALSE(v.isBool()); /* val is a bool */ \
        CHECK_FALSE(v.isInt32()); /* val " is not an int */ \
        CHECK_FALSE(v.isFloat64()); /* val is a double */ \
        CHECK(v.isString()); /* val is a string */ \
        CHECK(v.asString() == val); /* val string value ok */ \
        CHECK(v.toString() == val); /* val string value ok */ \
    }

    SECTION_testStringFromString("foo");
    SECTION_testStringFromString("e-e");
//    SECTION_testStringFromString(".1.1");
//    SECTION_testStringFromString(".1-1");
//    SECTION_testStringFromString("-1-1");
//    SECTION_testStringFromString("1-1");
    SECTION_testStringFromString("1foo");
}

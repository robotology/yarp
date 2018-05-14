/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * Copyright (C) 2006, 2008 Arjan Gijsberts
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/BottleImpl.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/StreamConnectionReader.h>
#include <yarp/os/impl/FakeTwoWayStream.h>
#include <yarp/os/impl/Logger.h>

#include <yarp/os/impl/UnitTest.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/Stamp.h>

//#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::os;


static double myfabs(double x) {
    if (x>=0) return x;
    return -x;
}

static bool similar(double x, double y, double eps) {
    return myfabs(x-y)<eps;
}

class BottleTest : public UnitTest {
public:
    void testSize() {
        report(0,"testing sizes...");
        BottleImpl bot;
        checkEqual(0,(int)bot.size(),"empty bottle");
        bot.addInt32(1);
        checkEqual(1,(int)bot.size(),"add int");
        bot.addString("hello");
        checkEqual(2,(int)bot.size(),"add string");
        bot.clear();
        checkEqual(0,(int)bot.size(),"clear");
    }


    void testString() {
        report(0,"testing string representation...");
        std::string target = "hello \"my\" \\friend";
        BottleImpl bot;
        bot.addInt32(5);
        bot.addString("hello \"my\" \\friend");
        std::string txt = bot.toString();
        const char *expect = "5 \"hello \\\"my\\\" \\\\friend\"";
        checkEqual(txt,expect,"string rep");
        BottleImpl bot2;
        bot2.fromString(txt);
        checkEqual(2,(int)bot2.size(),"return from string rep");
    }

    void testBinary() {
        report(0,"testing binary representation...");
        BottleImpl bot;
        bot.addInt32(5);
        bot.addString("hello");
        checkEqual(bot.isInt32(0),true,"type check");
        checkEqual(bot.isString(1),true,"type check");
        ManagedBytes store(bot.byteCount());
        bot.toBytes(store.bytes());
        BottleImpl bot2;
        bot2.fromBytes(store.bytes());
        checkEqual((int)bot2.size(),2,"recovery binary, length");
        checkEqual(bot2.isInt32(0),bot.isInt32(0),"recovery binary, integer");
        checkEqual(bot2.isString(1),bot.isString(1),"recovery binary, integer");
        BottleImpl bot3;
        bot3.fromString("[go] (10 20 30 40)");
        ManagedBytes store2(bot3.byteCount());
        bot3.toBytes(store2.bytes());
        bot.fromBytes(store2.bytes());
        checkEqual(bot.get(0).isVocab(),true,"type check");
        checkEqual(bot.get(1).isList(),true,"type check");

        Bottle bot4("0 1 2.2 3");
        size_t hsize;
        const char *hbuf = bot4.toBinary(&hsize);
        Bottle bot5;
        bot5.fromBinary(hbuf,hsize);
        checkEqual(bot5.size(),4,"player bug");

    }

    void testHex() {
        report(0,"testing hexadecimal...");
        Bottle b;
        b.fromString("0x0C");
        checkTrue(b.get(0).isInt32(),"0x0C is an integer");
        checkEqual(b.get(0).asInt32(),12,"0x0C");
        b.fromString("0x0E");
        checkTrue(b.get(0).isInt32(),"0x0E is an integer");
        checkEqual(b.get(0).asInt32(),14,"0x0E");
        b.fromString("0x0c");
        checkEqual(b.get(0).asInt32(),12,"0x0c");
        b.fromString("0x0e");
        checkEqual(b.get(0).asInt32(),14,"0x0e");
        b.fromString("0xff");
        checkEqual(b.get(0).asInt32(),255,"0xff");
    }

    void testStreaming() {
        report(0,"testing streaming (just text mode)...");

        BottleImpl bot;
        bot.addInt32(5);
        bot.addString("hello");

        BufferedConnectionWriter bbw(true);
        bot.write(bbw);

        std::string s;
        StringInputStream sis;
        StreamConnectionReader sbr;

        s = bbw.toString();
        sis.add(s);
        Route route;
        sbr.reset(sis,nullptr,route,s.length(),true);

        BottleImpl bot2;
        bot2.read(sbr);
        checkEqual(bot2.toString(),bot.toString(),"to/from stream");

    }

    void testTypes() {
        report(0,"testing types...");
        BottleImpl bot[4];

        bot[0].fromString("8 16 32 64 32.01 64.01 \"hello\" -0xF -15.0");

        ManagedBytes store0(bot[0].byteCount());
        bot[0].toBytes(store0.bytes());
        bot[1].fromBytes(store0.bytes());

        bot[2].addInt8(8);
        bot[2].addInt16(16);
        bot[2].addInt32(32);
        bot[2].addInt64(64);
        bot[2].addFloat32(32.01f);
        bot[2].addFloat64(64.01);
        bot[2].addString("hello");
        bot[2].addInt32(-0xF);
        bot[2].addFloat64(-15.0);


        ManagedBytes store1(bot[2].byteCount());
        bot[2].toBytes(store1.bytes());
        bot[3].fromBytes(store1.bytes());

        for (size_t i=0; i<4; i++) {
            BottleImpl& b = bot[i];
            report(0, std::string("check for bottle number ") + std::to_string(i));

            // arg 0 (int8)
            // note int8 are not created from string

            if (i < 2) {
                checkFalse(b.isInt8(0), "arg 0 is not int8");
            } else {
                checkTrue(b.isInt8(0), "arg 0 is int8");
            }
            checkEqual(b.get(0).asInt8(), 8, "arg 0 value is correct (as int8)");

            checkFalse(b.isInt16(0), "arg 0 is not int16");
            checkEqual(b.get(0).asInt16(), 8, "arg 0 value is correct (as int16)");

            if (i < 2) {
                checkTrue(b.isInt32(0), "arg 0 is int32");
            } else {
                checkFalse(b.isInt32(0), "arg 0 is not int32");
            }
            checkEqual(b.get(0).asInt32(), 8, "arg 0 value is correct (as int32)");

            checkFalse(b.isInt64(0), "arg 0 is not int64");
            checkEqual(b.get(0).asInt64(), 8, "arg 0 value is correct (as int64)");

            checkFalse(b.isFloat32(0), "arg 0 is not float32");
            checkTrue(myfabs(b.get(0).asFloat32() - 8) < 0.001, "arg 0 value is correct (as float32)");

            checkFalse(b.isFloat64(0), "arg 0 is not float64");
            checkTrue(myfabs(b.get(0).asFloat64() - 8) < 0.001, "arg 0 value is correct (as float64)");


            // arg 1 (int16)
            // note: int16 are not created from string

            checkFalse(b.isInt8(1), "arg 1 is not int8");
            checkEqual(b.get(1).asInt8(), 16, "arg 1 value is correct (as int8)");

            if (i < 2) {
                checkFalse(b.isInt16(1), "arg 1 is not int16");
            } else {
                checkTrue(b.isInt16(1), "arg 1 is int16");
            }
            checkEqual(b.get(1).asInt16(), 16, "arg 1 value is correct (as int16)");

            if (i < 2) {
                checkTrue(b.isInt32(1), "arg 1 is int32");
            } else {
                checkFalse(b.isInt32(1), "arg 1 is not int32");
            }
            checkEqual(b.get(1).asInt32(), 16, "arg 1 value is correct (as int32)");

            checkFalse(b.isInt64(1), "arg 1 is not int64");
            checkEqual(b.get(1).asInt64(), 16, "arg 1 value is correct (as int64)");

            checkFalse(b.isFloat32(1), "arg 1 is not float32");
            checkTrue(myfabs(b.get(1).asFloat32() - 16) < 0.001, "arg 1 value is correct (as float32)");

            checkFalse(b.isFloat64(1), "arg 1 is not float64");
            checkTrue(myfabs(b.get(1).asFloat64() - 16) < 0.001, "arg 1 value is correct (as float64)");


            // arg 2 (int32)
            checkFalse(b.isInt8(2), "arg 2 is not int8");
            checkEqual(b.get(2).asInt8(), 32, "arg 2 value is correct (as int8)");

            checkFalse(b.isInt16(2), "arg 2 is not int16");
            checkEqual(b.get(2).asInt16(), 32, "arg 2 value is correct (as int16)");

            checkTrue(b.isInt32(2), "arg 2 is int32");
            checkEqual(b.get(2).asInt32(), 32, "arg 2 value is correct (as int32)");

            checkFalse(b.isInt64(2), "arg 2 is not int64");
            checkEqual(b.get(2).asInt64(), 32, "arg 2 value is correct (as int64)");

            checkFalse(b.isFloat32(2), "arg 2 is not float32");
            checkTrue(myfabs(b.get(2).asFloat32() - 32) < 0.001, "arg 2 value is correct (as float32)");

            checkFalse(b.isFloat64(2), "arg 2 is not float64");
            checkTrue(myfabs(b.get(2).asFloat64() - 32) < 0.001, "arg 2 value is correct (as float64)");


            // arg 3 (int64)
            // note: int64 are not created from string

            checkFalse(b.isInt8(3), "arg 3 is not int8");
            checkEqual(b.get(3).asInt8(), 64, "arg 3 value is correct (as int8)");

            checkFalse(b.isInt16(3), "arg 3 is not int16");
            checkEqual(b.get(3).asInt16(), 64, "arg 3 value is correct (as int16)");

            if (i < 2) {
                checkTrue(b.isInt32(3), "arg 3 is int32");
            } else {
                checkFalse(b.isInt32(3), "arg 3 is not int32");
            }
            checkEqual(b.get(3).asInt32(), 64, "arg 3 value is correct (as int32)");

            if (i < 2) {
                checkFalse(b.isInt64(3), "arg 3 is not int64");
            } else {
                checkTrue(b.isInt64(3), "arg 3 is int64");
            }
            checkEqual(b.get(3).asInt64(), 64, "arg 3 value is correct (as int64)");

            checkFalse(b.isFloat32(3), "arg 3 is not float32");
            checkTrue(myfabs(b.get(3).asFloat32() - 64) < 0.001, "arg 3 value is correct (as float32)");

            checkFalse(b.isFloat64(3), "arg 3 is not float64");
            checkTrue(myfabs(b.get(3).asFloat64() - 64) < 0.001, "arg 3 value is correct (as float64)");

            // arg 4 (float32)
            // note: float32 are not created from string

            if (i < 2) {
                checkFalse(b.isFloat32(4), "arg 4 is not float32");
            } else {
                checkTrue(b.isFloat32(4), "arg 4 is float32");
            }
            checkTrue(myfabs(b.get(4).asFloat32() - 32.01f) < 0.001, "arg 4 value is correct (as float32)");

            if (i < 2) {
                checkTrue(b.isFloat64(4), "arg 4 is float64");
            } else {
                checkFalse(b.isFloat64(4), "arg 4 is not float64");
            }
            checkTrue(myfabs(b.get(4).asFloat64() - 32.01) < 0.001, "arg 4 value is correct (as float64)");


            checkFalse(b.isFloat32(5), "arg 5 is not float32");
            checkTrue(myfabs(b.get(5).asFloat32() - 64.01f) < 0.001, "arg 5 value is correct (as float32)");

            checkTrue(b.isFloat64(5), "arg 5 is float64");
            checkTrue(myfabs(b.get(5).asFloat64() - 64.01) < 0.001, "arg 5 value is correct (as float64)");


            checkTrue(b.isString(6), "arg 6 is string");
            checkEqual(b.get(6).asString().c_str(), "hello", "arg 6 value is correct");

            checkTrue(b.isInt32(7), "arg 7 is int32");
            checkEqual(b.get(7).asInt32(), -0xF, "arg 7 value is correct (hex works)");

            checkFalse(b.isFloat32(8), "arg 8 is not float32");
            checkTrue(b.isFloat64(8), "arg 8 is float64");
            checkTrue(myfabs(b.get(8).asFloat64() + 15.0) < 0.001,"arg 8 value is correct");
        }

    }


    void testClear() {
        report(0,"testing clear...");
        Bottle b;
        b.addString("hello");
        checkEqual(1,b.size(),"size ok");
        b.clear();
        b.addString("there");
        checkEqual(1,b.size(),"size ok");
    }

    void testLists() {
        report(0,"testing lists...");
        BottleImpl bot, bot2, bot3;
        bot.fromString("(1 (2 3 7) 3) (0.0 \"b\" 1)");
        checkEqual((int)bot.size(),2,"list test 1");
        bot2.fromString(bot.toString());
        checkEqual((int)bot2.size(),2,"list test 2");

        bot.fromString("(1 2) 4");
        ManagedBytes store(bot.byteCount());
        bot.toBytes(store.bytes());
        bot3.fromBytes(store.bytes());
        checkEqual((int)bot3.size(),2,"list test 3");
        report(0,std::string("bot3 is ") + bot3.toString());

        Bottle bot10;
        {
            Bottle& bb = bot10.addList();
            bb.addInt32(1);
            bb.addInt32(2);
            bb.addInt32(3);
        }
        {
            Bottle& bb = bot10.addList();
            bb.addInt32(4);
            bb.addInt32(5);
            bb.addInt32(6);
        }
        checkEqual(bot10.size(),2,"construction test 1");
        checkEqual(bot10.toString().c_str(),"(1 2 3) (4 5 6)",
                   "construction test 2");
        checkTrue(bot10.get(1).isList(),"construction test 3");
        checkEqual(bot10.get(1).asList()->toString().c_str(),"4 5 6",
                   "construction test 4");
    }

    void testBits() {
        report(0,"testing Value interface...");
        Bottle bot("1 \"hi\" (4 \"foo\") 6 7");
        checkTrue(bot.get(0).isInt32(),"type check");
        checkTrue(bot.get(1).isString(),"type check");
        checkTrue(bot.get(2).isList(),"type check");
        checkTrue(bot.get(2).asList()!=nullptr,"can get sublist");
        if (bot.get(2).asList()!=nullptr) {
            checkTrue(bot.get(2).asList()->get(0).isInt32(),"type check");
            checkTrue(bot.get(2).asList()->get(1).isString(),"type check");
        }
        checkTrue(bot.get(50).isNull(),"null type check");
    }

    void testEquality() {
        report(0,"testing equality...");
        Bottle bot1("1 2 3");
        Bottle bot2("1 2");
        Bottle bot3("1 2 3");
        checkTrue(bot1 != bot2, "A!=B");
        checkTrue(bot2 != bot3, "B!=C");
        checkTrue(bot1 == bot3, "A==C");
    }

    void testRange() {
        report(0,"testing range...");
        Bottle bot1("1 (2 3) (4 5 6) 7");
        Bottle bot2;
        bot2.copy(bot1,1,2);
        checkEqual(bot2.size(),2,"subrange");
        checkEqual(bot2.get(0).toString().c_str(),"2 3","subrange");
        bot2.copy(bot2,0,1);
        checkEqual(bot2.size(),1,"self copy");
    }

    void testFind() {
        report(0,"testing find...");
        Bottle bot("(hello friend) (say 12 13) green 255 blue 19");
        checkTrue(bot.check("hello"), "group check succeeds ok");
        checkFalse(bot.check("goodbye"), "group check fails ok");

        // shortcut find, some people use it
        checkEqual(bot.find("green").asInt32(), 255, "shortcut find succeeds ok");
        checkTrue(bot.find("red").isNull(), "shortcut find fails ok");

        // shortcut check?
        checkTrue(bot.check("green"), "shortcut check succeeds ok");
        checkFalse(bot.check("red"), "shortcut check fails ok");

        checkEqual(bot.findGroup("say").toString().c_str(),
                   "say 12 13","seek key");
        checkEqual(bot.find("hello").toString().c_str(),
                   "friend","seek key");
        checkTrue(bot.find("purple").isNull(),"seek absent key");
    }

    void testVocab() {
        report(0,"testing vocab...");
        Bottle bot("[send] 10 20");
        checkEqual(bot.size(),3,"plausible parse");
        checkTrue(bot.get(0).isVocab(),"vocab present");
        checkEqual(bot.get(0).asInt32(),VOCAB('s','e','n','d'),
                   "vocab match");
    }

    void testBlob() {
        report(0,"testing blob...");
        Bottle bot("{4 42 255} 10 20");
        checkEqual(bot.size(),3,"plausible parse");
        checkTrue(bot.get(0).isBlob(),"blob present");
        checkEqual((int)bot.get(0).asBlobLength(),3,"blob length");
        checkEqual(bot.get(0).asBlob()[1],42, "blob match");

        report(0,"testing blob with internal null...");
        char blob[12]="hello\0world";
        const Value v ((void*)blob, sizeof(blob));
        checkEqual(12,(int)v.asBlobLength(),"value length");
        checkFalse(v.isNull(),"value non-null");
        Bottle b;
        b.add(v);
        checkEqual(b.size(),1,"insertion happened");
        checkTrue(b.get(0).isBlob(),"insertion is right type");
        checkEqual(12,(int)b.get(0).asBlobLength(),"length within bottle");
    }

    void testStandard() {
        report(0,"testing standard compliance...");

        // in theory, bottles should comply with a standard binary format
        // we check that here

        Bottle bot("10 20 30 40");
        //bot.setNested(true);
        //bot.specialize(bot.get(0).getCode());
        BufferedConnectionWriter writer;
        bot.write(writer);
        std::string s = writer.toString();
        checkEqual((int)s.length(),sizeof(NetInt32)*(1+1+(int)bot.size()),
                   "exact number of integers, plus type/count");

        Bottle bot2("[go] (10 20 30 40)");
        writer.clear();
        bot2.write(writer);
        s = writer.toString();
        // 1 for (outer) list code
        // 1 for list length
        // 1 for vocab code
        // 1 for vocab code value
        // 1 for (inner) list code
        // 1 for (inner) list length
        // 4 for integers in list
        checkEqual((int)s.length(),sizeof(NetInt32)*(10),
                   "nested example");
    }


    void testReread() {
        report(0,"testing reread specialization is not broken...");

        Bottle bot("10 20 30");  // first a specialized list
        Bottle bot2;
        {
            BufferedConnectionWriter writer(false);
            bot.write(writer);
            StringInputStream sis;
            sis.add(writer.toString());
            StreamConnectionReader br;
            br.reset(sis,nullptr,Route(),sis.toString().length(),false);
            bot2.read(br);
            //printf("bot is %s\n", bot.toString().c_str());
            //printf("bot2 is %s\n", bot2.toString().c_str());
            checkEqual(bot.size(),bot2.size(),"length check");
            checkTrue(bot2.get(2).isInt32(),"type check");
            checkEqual(bot.toString().c_str(),bot2.toString().c_str(),
                       "content check");
        }
        bot.fromString("10 20 30.5"); // now an unspecialized list
        {
            BufferedConnectionWriter writer(false);
            bot.write(writer);
            StringInputStream sis;
            sis.add(writer.toString());
            StreamConnectionReader br;
            br.reset(sis,nullptr,Route(),sis.toString().length(),false);
            bot2.read(br);
            checkEqual(bot.size(),bot2.size(),"length check");
            checkTrue(bot2.get(2).isFloat64(),"type check");
            checkEqual(bot.toString().c_str(),bot2.toString().c_str(),
                       "content check");
        }

    }


    void testWhiteSpace() {
        report(0,"testing white space behavior...");
        Bottle bot;
        bot.fromString("\t\thello\t10\n");
        checkEqual(bot.size(),2,"ok with tab");
        checkEqual(bot.get(0).asString().c_str(),"hello","pre-tab ok");
        checkEqual(bot.get(1).asInt32(),10,"post-tab ok");

        report(0, "checking pasa problem with lists missing last element...");
        std::string s2 = "[set] [poss] (10.0 20.0 30.0 40.0 5.1)\n";
        Bottle p;
        p.fromString(s2.c_str());
        checkEqual(p.get(2).asList()->size(),5,"newline test checks out");
    }

    void testNestDetection() {
        report(0,"testing nesting detection...");
        checkTrue(!BottleImpl::isComplete("(1 2 3"), "incomplete");
        checkTrue(BottleImpl::isComplete("(1 2 3)"), "complete");
    }


    void testSpecialChars() {
        report(0,"testing special characters...");
        Bottle bot("file ../foo.txt");
        checkTrue(bot.get(1).isString(),"paths starting with a decimal");
        Bottle bot2;
        std::string test = "\"\n\r\"";
        bot2.addString(test);
        Bottle bot3;
        bot3.fromString(bot2.toString());
        checkEqual(bot3.get(0).asString().c_str(),test.c_str(),
                   "roundtripping quotes newline etc");
    }

    void testAppend() {
        report(0,"testing append...");
        Bottle bot1("1 2 3");
        Bottle bot2("4 5");
        bot1.append(bot2);
        checkEqual(bot1.size(),5,"add two bottles");
    }

    void testStack() {
        report(0,"testing stack functionality...");
        Bottle bot;
        bot.addInt32(10);
        bot.addInt32(11);
        bot.addString("Foo");
        Bottle& bot2 = bot.addList();
        bot2.addInt32(3);
        bot.addFloat64(2.71828);
        checkTrue(bot.pop().isFloat64(),"popping double");
        checkEqual(bot.size(),4,"bottle size decreased after pop");
        checkEqual(bot.pop().asList()->pop().asInt32(),3,"popping list and nested int");
        checkEqual(bot.pop().asString().c_str(),"Foo", "popping string");
        Value val;
        val = bot.pop();
        checkTrue(val.isInt32(), "popped value is of type int");
        checkEqual(val.asInt32(), 11, "popped value is integer 11");
        val = bot.pop();
        checkTrue(val.isInt32(), "popped value is of type int");
        checkEqual(val.asInt32(), 10, "popped value is integer 10");
        val = bot.pop();
        checkTrue(bot.pop().isNull(), "empty bottle pops null");
        checkEqual(bot.size(),0,"bottle is empty after popping");
    }

    void testTypeDetection() {
        report(0,"test type detection...");
        Bottle bot;
        bot.fromString("hello ip 10.0.0.10");
        checkEqual(bot.size(),3,"right length");
        checkTrue(bot.get(2).isString(),"right type");
        checkEqual(bot.get(2).asString().c_str(),"10.0.0.10","multiple period test");
    }

    void testModify() {
        report(0,"test bottle modification...");
        Bottle b;
        b.addInt32(3);
        b.get(0) = Value(5);
        b.hasChanged();
        checkEqual(b.get(0).asInt32(),5,"assignment works");
    }

    void testScientific() {
        report(0,"test scientific notation...");
        Bottle b;
        b.fromString("10.0e5");
        checkTrue(similar(b.get(0).asFloat64(),10e5, 1),
                  "positive positive lower case");
        b.fromString("10.0e-2");
        checkTrue(similar(b.get(0).asFloat64(),10e-2, 1e-2),
                  "positive negative lower case");
        b.fromString("1E-8");
        checkTrue(b.get(0).isFloat64(),"type check");
        checkTrue(similar(b.get(0).asFloat64(),1e-8, 1e-9),
                  "positive negative upper case");
    }

    void testContinuation() {
        report(0,"test continuation...");
        Bottle b("x (1 2\n3 4\n5 6)\ny (1 2)");
        checkEqual(b.find("x").asList()->size(),6,"x has right length");
        checkEqual(b.find("y").asList()->size(),2,"y has right length");
    }

    void testAssignment() {
        report(0,"test assignment...");
        Bottle b("x (1 2\n3 4\n5 6)\ny (1 2)");
        Bottle b2;
        b2 = b;
        checkEqual(b2.size(),4,"initial copy ok");
        b2 = b2;
        checkEqual(b2.size(),4,"re-copy ok");
        Bottle b3("zig zag");
        b2.clear();
        checkFalse(b2.isNull(),"have a non-null bottle");
        checkEqual(b2.size(),0,"have an empty bottle");
        b3 = b2;

        checkFalse(b3.isNull(),"copied a non-null bottle");

        checkEqual(b3.size(),0,"copied an empty bottle");
        Bottle& nullBot = b.findGroup("zig");
        checkTrue(nullBot.isNull(),"have a null bottle");
        b = nullBot;

        // The expected result has flipped, due to popular demand [Massera :-)]
        //checkFalse(b.isNull(),"failed, as expected");
        checkTrue(b.isNull(),"isNull() preserved correctly");
    }

    void testEmptyList() {
        // based on a case submitted by Stephane Lallee; doesn't appear to
        // fail here as he sees going from TCL to C++
        report(0,"test empty list");
        Bottle b;
        b.fromString("appp plan-clean (\"<Plan>\" \"<Names>\" cover \"</Names>\" \"<isAtomic>\" 1 \"</isAtomic>\" \"<motorCommand>\" () \"</motorCommand>\" \"<Primitive>\" (cover 28988.470168 \"<args>\" \"7106\" \"7103\" \"</args>\" \"<argsRole>\" object1 arg2 object2 arg1 subject \"7107\" \"</argsRole>\" \"<preReqRelations>\" \"</preReqRelations>\" \"<preForRelations>\" \"</preForRelations>\" \"<postAddRelations>\" \"</postAddRelations>\" \"<postRemRelations>\" \"</postRemRelations>\" \"<addRelations>\" \"</addRelations>\" \"<remRelations>\" visible null arg2 \"</remRelations>\") \"</Primitive>\" \"<SubPlans>\" \"</SubPlans>\" \"</Plan>\")");
        Bottle b2 = b;
        checkEqual(b2.size(),3,"copy ok level 1");
        Bottle *sub = b2.get(2).asList();
        checkTrue(sub!=nullptr,"list where list expected");
        if (sub!=nullptr) {
            checkEqual(sub->size(),16,"copy ok level 2");
        }

        DummyConnector con;
        con.setTextMode(false);
        b.write(con.getWriter());
        Bottle b3;
        b3.read(con.getReader());
        checkEqual(b3.size(),b.size(),"binary read/write ok");
        sub = b3.get(2).asList();
        checkTrue(sub!=nullptr,"list where list expected");
        if (sub!=nullptr) {
            checkEqual(sub->size(),16,"copy ok level 2");
        }
    }

    void testNull() {
        report(0,"test null bottle");
        Bottle bAll("(x 1) (y 2)");
        Bottle& b1 = bAll.findGroup("groupToFind");
        checkTrue(b1.isNull(), "bottle null referenced correctly");
        Bottle b2 = bAll.findGroup("groupToFind");
        checkTrue(b2.isNull(), "bottle null copied correctly");
    }

    void testSerialCopy() {
        report(0,"test serialization by copy");
        Value v(3.14);
        Bottle b;
        b.read(v);
        checkEqualish(b.get(0).asFloat64(),3.14,"copy to bottle succeeded");
        Bottle b2;
        b.write(b2);
        checkEqualish(b2.get(0).asFloat64(),3.14,"copy from bottle succeeded");
    }

    void testStringWithNull() {
        report(0,"test string with null");
        char buf1[] = "hello world";
        char buf2[] = "hello world";
        buf2[5] = '\0';
        size_t len = 11;
        std::string str1(buf1,len);
        std::string str2(buf2,len);
        checkEqual(str1.length(),len,"unmodified string length ok");
        checkEqual(str2.length(),len,"modified string length ok");
        std::string str3(str2);
        checkEqual(str3.length(),len,"copied string length ok");
        Bottle bot;
        bot.addString(str2);
        checkEqual(bot.get(0).asString().length(),len,"bottled string asString() length ok");
        checkEqual(bot.get(0).toString().length(),len,"bottled string toString() length ok");
        Bottle bot2 = bot;
        checkEqual(bot2.get(0).asString().length(),len,"bottled, copied string length ok");

        Bottle bot3;
        bot.write(bot3);
        checkEqual(bot3.get(0).asString().length(),len,"bottled, serialized string length ok");

        Bottle bot4;
        bot4.fromString(bot.toString());
        checkEqual(bot4.get(0).asString().length(),len,"bottled, text-serialized string length ok");
    }

    void testBool() {
        report(0,"test boolean values");
        Bottle bot("1 true \"true\" false \"false\" 0 [ok]");
        checkTrue(bot.get(1).isBool(),"true is boolean");
        checkFalse(bot.get(2).isBool(),"quoted true is not boolean");
        checkTrue(bot.get(3).isBool(),"false is boolean");
        checkFalse(bot.get(4).isBool(),"quoted false is not boolean");
        checkFalse(bot.get(6).isBool(),"not all vocabs are boolean");
        checkTrue(bot.get(1).asBool(),"true is true");
        checkFalse(bot.get(3).asBool(),"false is false");
        checkEqual(bot.get(1).asString().c_str(),"true","true can spell");
        checkEqual(bot.get(3).asString().c_str(),"false","false can spell");
    }

    void testDict() {
        report(0,"test dictionary values");
        Bottle bot("1");
        Property& p = bot.addDict();
        p.put("test","me");
        p.put("hi","there");
        Bottle bot2;
        bot2 = bot;
        checkTrue(bot2.get(1).isDict(),"dict copies ok");
        checkEqual(bot2.get(1).asDict()->find("test").asString().c_str(),
                   "me","dict content copies ok");

        Bottle bot3;
        bot3.fromString(bot.toString());
        checkEqual(bot2.get(1).asSearchable()->find("test").asString().c_str(),
                   "me","dict content serializes ok");
        // serialization currently will convert dicts to lists,
        // for backwards compatibility
    }

    void testLoopBug() {
        report(0,"test infinite loop tickled by yarpmanager + string type change");
        Bottle pos("Pos ((x 349.5) (y 122)) ((x 286) (y 122)) ((x 413) (y 122))");
        for(int i=1; i<pos.size(); i++) {
            pos.get(i).find("x").asFloat64();
        }
    }

    void testManyMinus() {
        report(0,"test a string with several minus characters");
        Bottle bot("---");
        checkEqual(bot.get(0).asString().c_str(),"---","interpreted ok");
    }

    void testCopyPortable() {
        report(0,"test copyPortable method");
        Bottle b1("1 2 3"), b2;
        Portable::copyPortable(b1,b2);
        checkEqual(b2.size(),b1.size(),"length ok");
        checkEqual(b2.get(2).asInt32(),3,"content ok");
        Stamp s1(42,10.0),s2,s3;
        Portable::copyPortable(s1,s2);
        checkEqual(s1.getCount(),42,"stamp-to-stamp ok");
        Portable::copyPortable(s1,b1);
        checkEqual(b1.get(0).asInt32(),42,"stamp-to-bottle ok");
        Portable::copyPortable(b1,s3);
        checkEqual(s3.getCount(),42,"bottle-to-stamp ok");
    }

    virtual void runTests() override {
        testClear();
        testSize();
        testString();
        testBinary();
        testStreaming();
        testTypes();
        testLists();
        testBits();
        testEquality();
        testRange();
        testFind();
        testVocab();
        testBlob();
        testWhiteSpace();
        testStandard();
        testNestDetection();
        testReread();
        testSpecialChars();
        testAppend();
        testStack();
        testTypeDetection();
        testModify();
        testScientific();
        testContinuation();
        testAssignment();
        testHex();
        testEmptyList();
        testNull();
        testSerialCopy();
        testStringWithNull();
        testBool();
        testDict();
        testLoopBug();
        testManyMinus();
        testCopyPortable();
    }

    virtual std::string getName() override {
        return "BottleTest";
    }
};


// single instance for testing
static BottleTest theBottleTest;

UnitTest& getBottleTest() {
    return theBottleTest;
}


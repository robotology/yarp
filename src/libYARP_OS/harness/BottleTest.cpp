// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/BottleImpl.h>
#include <yarp/BufferedConnectionWriter.h>
#include <yarp/StreamConnectionReader.h>
#include <yarp/FakeTwoWayStream.h>
#include <yarp/Logger.h>

#include <yarp/UnitTest.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Vocab.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::os;


static double myfabs(double x) {
    if (x>=0) return x;
    return -x;
}

class BottleTest : public UnitTest {
public:
    void testSize() {
        report(0,"testing sizes...");
        BottleImpl bot;
        checkEqual(0,bot.size(),"empty bottle");
        bot.addInt(1);
        checkEqual(1,bot.size(),"add int");
        bot.addString("hello");
        checkEqual(2,bot.size(),"add string");
        bot.clear();
        checkEqual(0,bot.size(),"clear");
    }


    void testString() {
        report(0,"testing string representation...");
        String target = "hello \"my\" \\friend";
        BottleImpl bot;
        bot.addInt(5);
        bot.addString("hello \"my\" \\friend");
        String txt = bot.toString();
        const char *expect = "5 \"hello \\\"my\\\" \\\\friend\"";
        checkEqual(txt,expect,"string rep");
        BottleImpl bot2;
        bot2.fromString(txt);
        checkEqual(2,bot2.size(),"return from string rep");
    }

    void testBinary() {
        report(0,"testing binary representation...");
        BottleImpl bot;
        bot.addInt(5);
        bot.addString("hello");
        checkEqual(bot.isInt(0),true,"type check");
        checkEqual(bot.isString(1),true,"type check");
        ManagedBytes store(bot.byteCount());
        bot.toBytes(store.bytes());
        BottleImpl bot2;
        bot2.fromBytes(store.bytes());
        checkEqual(bot2.size(),2,"recovery binary, length");
        checkEqual(bot2.isInt(0),bot.isInt(0),"recovery binary, integer");
        checkEqual(bot2.isString(1),bot.isString(1),"recovery binary, integer");
        BottleImpl bot3;
        bot3.fromString("[go] (10 20 30 40)");
        ManagedBytes store2(bot3.byteCount());
        bot3.toBytes(store2.bytes());
        bot.fromBytes(store2.bytes());
        checkEqual(bot.get(0).isVocab(),true,"type check");
        checkEqual(bot.get(1).isList(),true,"type check");
    }

    void testStreaming() {
        report(0,"testing streaming (just text mode)...");

        BottleImpl bot;
        bot.addInt(5);
        bot.addString("hello");

        try {
            BufferedConnectionWriter bbw(true);
            bot.write(bbw);
      
            String s;
            StringInputStream sis;
            StreamConnectionReader sbr;
      
            s = bbw.toString();
            sis.add(s);
            Route route;
            sbr.reset(sis,NULL,route,s.length(),true);
      
            BottleImpl bot2;
            bot2.read(sbr);
            checkEqual(bot2.toString(),bot.toString(),"to/from stream");
        } catch (IOException e) {
            report(1,e.toString() + " <<< exception thrown");
        }
    }

    void testTypes() {
        report(0,"testing types...");
        BottleImpl bot[3];
        bot[0].fromString("5 10.2 \"hello\" -0xF -15.0");
        checkEqual(bot[0].get(3).asInt(),-15,"hex works");
        bot[1].addInt(5);
        bot[1].addDouble(10.2);
        bot[1].addString("hello");
        bot[1].addInt(-15);
        bot[1].addDouble(-15.0);
        ManagedBytes store(bot[0].byteCount());
        bot[0].toBytes(store.bytes());
        bot[2].fromBytes(store.bytes());

        for (int i=0; i<3; i++) {
            BottleImpl& b = bot[i];
            report(0,String("check for bottle number ") +
                   NetType::toString(i));
            checkTrue(b.isInt(0)&&b.isInt(3),"ints");
            checkTrue(b.isDouble(1)&&b.isDouble(4),"doubles");
            checkTrue(b.isString(2),"strings");
            checkEqual(b.getInt(0),5,"arg 0");
            checkTrue(myfabs(b.getDouble(1)-10.2)<0.01,"arg 1");
            checkEqual(b.getString(2).c_str(),"hello","arg 2");
            checkEqual(b.getInt(3),-15,"arg 3");
            checkTrue(myfabs(b.getDouble(4)+15.0)<0.01,"arg 4");
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
        checkEqual(bot.size(),2,"list test 1");
        bot2.fromString(bot.toString());
        checkEqual(bot2.size(),2,"list test 2");

        bot.fromString("(1 2) 4");
        ManagedBytes store(bot.byteCount());
        bot.toBytes(store.bytes());
        bot3.fromBytes(store.bytes());
        checkEqual(bot3.size(),2,"list test 3");
        report(0,String("bot3 is ") + bot3.toString());

        Bottle bot10;
        {
            Bottle& bb = bot10.addList();
            bb.addInt(1);
            bb.addInt(2);
            bb.addInt(3);
        }
        {
            Bottle& bb = bot10.addList();
            bb.addInt(4);
            bb.addInt(5);
            bb.addInt(6);
        }
        checkEqual(bot10.size(),2,"construction test 1");
        checkEqual(bot10.toString().c_str(),"(1 2 3) (4 5 6)",
                   "construction test 2");
        checkTrue(bot10.isList(1),"construction test 3");
        checkEqual(bot10.getList(1)->toString().c_str(),"4 5 6",
                   "construction test 4");
    }

    void testBits() {
        report(0,"testing BottleBits interface...");
        Bottle bot("1 \"hi\" (4 \"foo\") 6 7");
        checkTrue(bot.isInt(0),"indirect type check");
        checkTrue(bot.get(0).isInt(),"type check");
        checkTrue(bot.get(1).isString(),"type check");
        checkTrue(bot.get(2).isList(),"type check");
        checkTrue(bot.get(2).asList()!=NULL,"can get sublist");
        if (bot.get(2).asList()!=NULL) {
            checkTrue(bot.get(2).asList()->get(0).isInt(),"type check");
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
        bot2.copyRange(bot1,1,2);
        checkEqual(bot2.size(),2,"subrange");
        checkEqual(bot2.get(0).toString().c_str(),"2 3","subrange");
    }

    void testFind() {
        report(0,"testing find...");
        Bottle bot("(hello friend) (say 12 13) green");
        checkEqual(bot.findGroup("say").toString().c_str(),
                   "say 12 13","seek key");
        checkEqual(bot.findValue("hello").toString().c_str(),
                   "friend","seek key");
        checkTrue(bot.findValue("blue").isNull(),"seek absent key");
    }

    void testVocab() {
        report(0,"testing vocab...");
        Bottle bot("[send] 10 20");
        checkEqual(bot.size(),3,"plausible parse");
        checkTrue(bot.get(0).isVocab(),"vocab present");
        checkEqual(bot.get(0).asInt(),VOCAB('s','e','n','d'),
                   "vocab match");
    }

    void testBlob() {
        report(0,"testing blob...");
        Bottle bot("{4 42 255} 10 20");
        checkEqual(bot.size(),3,"plausible parse");
        checkTrue(bot.get(0).isBlob(),"blob present");
        checkEqual(bot.get(0).asBlobLength(),3,"blob length");
        checkEqual(bot.get(0).asBlob()[1],42, "blob match");
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
        String s = writer.toString();
        checkEqual(s.length(),sizeof(NetInt32)*(1+1+1+bot.size()),
                   "exact number of integers, plus bytes/type/count");

        Bottle bot2("[go] (10 20 30 40)");
        writer.clear();
        bot2.write(writer);
        s = writer.toString();
        // 1 for message size
        // 1 for (outer) list code
        // 1 for list length
        // 1 for vocab code
        // 1 for vocab code value
        // 1 for (inner) list code
        // 1 for (inner) list length
        // 4 for integers in list
        checkEqual(s.length(),sizeof(NetInt32)*(11),
                   "nested example");
    }

    virtual void runTests() {
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
        testStandard();
    }

    virtual String getName() {
        return "BottleTest";
    }
};


// single instance for testing
static BottleTest theBottleTest;

UnitTest& getBottleTest() {
    return theBottleTest;
}


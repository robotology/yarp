/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2006, 2008 Arjan Gijsberts
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Bottle.h>

#include <yarp/os/DummyConnector.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Vocab.h>

#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/StreamConnectionReader.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os::impl;
using namespace yarp::os;

TEST_CASE("os::BottleTest", "[yarp::os]")
{
    SECTION("testing string representation")
    {
        std::string target = "hello \"my\" \\friend";
        Bottle bot;
        bot.addInt32(5);
        bot.addString("hello \"my\" \\friend");
        std::string txt = bot.toString();
        const char *expect = "5 \"hello \\\"my\\\" \\\\friend\"";
        CHECK(txt == expect); // "string rep"
        Bottle bot2;
        bot2.fromString(txt);
        CHECK(bot2.size() == (size_t) 2); //"return from string rep"
    }

    SECTION("testing hexadecimal")
    {
        Bottle b;
        b.fromString("0x0C");
        CHECK(b.get(0).isInt32()); // "0x0C is an integer"
        CHECK(b.get(0).asInt32() == 12); // "0x0C" == 12
        b.fromString("0x0E");
        CHECK(b.get(0).isInt32()); // "0x0E is an integer"
        CHECK(b.get(0).asInt32() == 14); // "0x0E"
        b.fromString("0x0c");
        CHECK(b.get(0).asInt32() == 12); // "0x0c"
        b.fromString("0x0e");
        CHECK(b.get(0).asInt32() == 14); // "0x0e"
        b.fromString("0xff");
        CHECK(b.get(0).asInt32() == 255); // "0xff"
    }

    SECTION("testing streaming (just text mode)")
    {
        Bottle bot;
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
        sbr.reset(sis, nullptr, route, s.length(), true);

        Bottle bot2;
        bot2.read(sbr);
        CHECK(bot2.toString() == bot.toString()); // "to/from stream"
    }

    SECTION("testing clear")
    {
        Bottle b;
        b.addString("hello");
        CHECK(b.size() == (size_t) 1); // "size ok"
        b.clear();
        CHECK(b.size() == (size_t) 0); // "size ok"
        b.addString("there");
        CHECK(b.size() == (size_t) 1); // "size ok"
    }

    SECTION("testing lists")
    {
        Bottle bot, bot2;
        bot.fromString("(1 (2 3 7) 3) (0.0 \"b\" 1)");
        CHECK(bot.size() == (size_t) 2); // "list test 1"
        bot2.fromString(bot.toString());
        CHECK(bot2.size() == (size_t) 2); // "list test 2"

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
        CHECK(bot10.size() == (size_t) 2); // "construction test 1"
        CHECK(bot10.toString() == "(1 2 3) (4 5 6)"); // "construction test 2"
        CHECK(bot10.get(1).isList()); // "construction test 3"
        CHECK(bot10.get(1).asList()->toString() == "4 5 6"); // "construction test 4"
    }

    SECTION("testing Value interface")
    {
        Bottle bot("1 \"hi\" (4 \"foo\") 6 7");
        CHECK(bot.get(0).isInt32());
        CHECK(bot.get(1).isString());
        CHECK(bot.get(2).isList());
        REQUIRE(bot.get(2).asList() != nullptr); // "can get sublist"
        CHECK(bot.get(2).asList()->get(0).isInt32());
        CHECK(bot.get(2).asList()->get(1).isString());
        CHECK(bot.get(50).isNull()); // "null type check"
    }

    SECTION("testing equality")
    {
        Bottle bot1("1 2 3");
        Bottle bot2("1 2");
        Bottle bot3("1 2 3");
        CHECK(bot1 != bot2);
        CHECK(bot2 != bot3);
        CHECK(bot1 == bot3);
    }

    SECTION("testing range")
    {
        Bottle bot1("1 (2 3) (4 5 6) 7");
        Bottle bot2;
        bot2.copy(bot1, 1, 2);
        CHECK(bot2.size() == (size_t) 2); // "subrange"
        CHECK(bot2.get(0).toString() == "2 3"); // "subrange"
        bot2.copy(bot2, 0, 1);
        CHECK(bot2.size() == (size_t) 1); // "self copy"
    }

    SECTION("testing find")
    {
        Bottle bot("(hello friend) (say 12 13) green 255 blue 19");
        CHECK(bot.check("hello")); // "group check succeeds ok"
        CHECK_FALSE(bot.check("goodbye")); // "group check fails ok"

        // shortcut find, some people use it
        CHECK(bot.find("green").asInt32() == 255); // "shortcut find succeeds ok"
        CHECK(bot.find("red").isNull()); // "shortcut find fails ok"

        // shortcut check?
        CHECK(bot.check("green")); // "shortcut check succeeds ok"
        CHECK_FALSE(bot.check("red")); // "shortcut check fails ok"

        CHECK(bot.findGroup("say").toString() == "say 12 13"); // "seek key"
        CHECK(bot.find("hello").toString() == "friend"); // "seek key"
        CHECK(bot.find("purple").isNull()); //"seek absent key"
    }

    SECTION("testing vocab")
    {
        Bottle bot("[send] 10 20");
        CHECK(bot.size() == (size_t) 3); // "plausible parse"
        CHECK(bot.get(0).isVocab32()); // "vocab present"
        CHECK(bot.get(0).asVocab32() == yarp::os::createVocab32('s', 'e', 'n', 'd')); // "vocab match"
    }


    SECTION("testing blob")
    {
        Bottle bot("{4 42 255} 10 20");
        CHECK(bot.size() == (size_t) 3); // "plausible parse"
        CHECK(bot.get(0).isBlob()); // "blob present"
        CHECK(bot.get(0).asBlobLength() == 3); // "blob length"
        CHECK(bot.get(0).asBlob()[1] == 42); // "blob match"

        INFO("testing blob with internal null...");
        char blob[12]="hello\0world";
        const Value v ((void*)blob, sizeof(blob));
        CHECK(v.asBlobLength() == 12); // "value length"
        CHECK_FALSE(v.isNull()); // "value non-null"
        Bottle b;
        b.add(v);
        CHECK(b.size() == (size_t) 1); // "insertion happened"
        CHECK(b.get(0).isBlob()); // "insertion is right type"
        CHECK(b.get(0).asBlobLength() == (size_t) 12); // "length within bottle"
    }


    SECTION("testing standard compliance")
    {
        // in theory, bottles should comply with a standard binary format
        // we check that here

        Bottle bot("10 20 30 40");
        //bot.setNested(true);
        //bot.specialize(bot.get(0).getCode());
        BufferedConnectionWriter writer;
        bot.write(writer);
        std::string s = writer.toString();
        CHECK(s.length() == sizeof(NetInt32)*(1+1+bot.size())); // "exact number of integers, plus type/count"

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
        CHECK(s.length() == sizeof(NetInt32)*(10)); // "nested example"
    }


    SECTION("testing reread specialization")
    {
        Bottle bot("10 20 30");  // first a specialized list
        Bottle bot2;
        {
            BufferedConnectionWriter writer(false);
            bot.write(writer);
            StringInputStream sis;
            sis.add(writer.toString());
            StreamConnectionReader br;
            br.reset(sis, nullptr, Route(), sis.toString().length(), false);
            bot2.read(br);
            CHECK(bot.size() == bot2.size()); // length check
            CHECK(bot2.get(2).isInt32()); // type check
            CHECK(bot.toString() == bot2.toString()); // content check
        }
        bot.fromString("10 20 30.5"); // now an unspecialized list
        {
            BufferedConnectionWriter writer(false);
            bot.write(writer);
            StringInputStream sis;
            sis.add(writer.toString());
            StreamConnectionReader br;
            br.reset(sis, nullptr, Route(), sis.toString().length(), false);
            bot2.read(br);
            CHECK(bot.size() == bot2.size()); // length check
            CHECK(bot2.get(2).isFloat64()); // type check
            CHECK(bot.toString() == bot2.toString()); // content check
        }

    }


    SECTION("testing white space behavior")
    {
        Bottle bot;
        bot.fromString("\t\thello\t10\n");
        CHECK(bot.size() == (size_t) 2); // ok with tab
        CHECK(bot.get(0).asString() == "hello"); // pre-tab ok
        CHECK(bot.get(1).asInt32() == 10); // post-tab ok

        std::string s2 = "[set] [poss] (10.0 20.0 30.0 40.0 5.1)\n";
        Bottle p;
        p.fromString(s2.c_str());
        CHECK(p.get(2).asList()->size() == (size_t) 5); // newline test checks out
    }

    SECTION("testing special characters")
    {
        Bottle bot("file ../foo.txt");
        CHECK(bot.get(1).isString()); // paths starting with a decimal
        Bottle bot2;
        std::string test = "\"\n\r\"";
        bot2.addString(test);
        Bottle bot3;
        bot3.fromString(bot2.toString());
        CHECK(bot3.get(0).asString() == test); // roundtripping quotes newline etc
    }

    SECTION("testing append")
    {
        Bottle bot1("1 2 3");
        Bottle bot2("4 5");
        bot1.append(bot2);
        CHECK(bot1.size() == 5); // add two bottles
    }

    SECTION("testing stack functionality")
    {
        Bottle bot;
        bot.addInt32(10);
        bot.addInt32(11);
        bot.addString("Foo");
        Bottle& bot2 = bot.addList();
        bot2.addInt32(3);
        bot.addFloat64(2.71828);
        CHECK(bot.pop().isFloat64()); // popping double
        CHECK(bot.size() == (size_t) 4); // bottle size decreased after pop
        CHECK(bot.pop().asList()->pop().asInt32() == 3); // popping list and nested int
        CHECK(bot.pop().asString() == "Foo"); // popping string
        Value val;
        val = bot.pop();
        CHECK(val.isInt32()); // popped value is of type int
        CHECK(val.asInt32() == 11); // popped value is integer 11
        val = bot.pop();
        CHECK(val.isInt32()); // popped value is of type int
        CHECK(val.asInt32() == 10); // popped value is integer 10
        val = bot.pop();
        CHECK(bot.pop().isNull()); // empty bottle pops null
        CHECK(bot.size() == 0); // bottle is empty after pop
    }

    SECTION("test type detection")
    {
        Bottle bot;
        bot.fromString("hello ip 10.0.0.10");
        CHECK(bot.size() == (size_t) 3); // right length
        CHECK(bot.get(2).isString()); // right type
        CHECK(bot.get(2).asString() == "10.0.0.10"); // multiple period test
    }

    SECTION("test bottle modification...")
    {
        Bottle b;
        b.addInt32(3);
        b.get(0) = Value(5);
        b.hasChanged();
        CHECK(b.get(0).asInt32() == 5); // assignment works
    }

    SECTION("test scientific notation...")
    {
        Bottle b;
        b.fromString("10.0e5");
        CHECK(b.get(0).asFloat64() == Approx(10e5).epsilon(1)); // positive positive lower case
        b.fromString("10.0e-2");
        CHECK(b.get(0).asFloat64() == Approx(10e-2).epsilon(1e-2)); // positive negative lower case
        b.fromString("1E-8");
        CHECK(b.get(0).isFloat64());
        CHECK(b.get(0).asFloat64() == Approx(1e-8).epsilon(1e-9)); // positive negative upper case
    }

    SECTION("test continuation")
    {
        Bottle b("x (1 2\n3 4\n5 6)\ny (1 2)");
        CHECK(b.find("x").asList()->size() == (size_t) 6); // x has right length
        CHECK(b.find("y").asList()->size() == (size_t) 2); // y has right length
    }

    SECTION("test assignment")
    {
        Bottle b("x (1 2\n3 4\n5 6)\ny (1 2)");
        Bottle b2;
        b2 = b;
        CHECK(b2.size() == (size_t) 4); // initial copy ok
        b2 = b2;
        CHECK(b2.size() == (size_t) 4); // re-copy ok
        Bottle b3("zig zag");
        b2.clear();
        CHECK(!b2.isNull()); // have a non-null bottle
        CHECK(b2.size() == (size_t) 0); // have an empty bottle
        b3 = b2;

        CHECK(!b3.isNull()); // copied a non-null bottle

        CHECK(b3.size() == (size_t) 0); // copied an empty bottle
        Bottle& nullBot = b.findGroup("zig");
        CHECK(nullBot.isNull()); // have a null bottle
        b = nullBot;

        // The expected result has flipped, due to popular demand [Massera :-)]
        //checkFalse(b.isNull(), "failed, as expected");
        CHECK(b.isNull()); // isNull() preserved correctly
    }

    SECTION("test empty list")
    {
        // based on a case submitted by Stephane Lallee; doesn't appear to
        // fail here as he sees going from TCL to C++
        Bottle b;
        b.fromString("appp plan-clean (\"<Plan>\" \"<Names>\" cover \"</Names>\" \"<isAtomic>\" 1 \"</isAtomic>\" \"<motorCommand>\" () \"</motorCommand>\" \"<Primitive>\" (cover 28988.470168 \"<args>\" \"7106\" \"7103\" \"</args>\" \"<argsRole>\" object1 arg2 object2 arg1 subject \"7107\" \"</argsRole>\" \"<preReqRelations>\" \"</preReqRelations>\" \"<preForRelations>\" \"</preForRelations>\" \"<postAddRelations>\" \"</postAddRelations>\" \"<postRemRelations>\" \"</postRemRelations>\" \"<addRelations>\" \"</addRelations>\" \"<remRelations>\" visible null arg2 \"</remRelations>\") \"</Primitive>\" \"<SubPlans>\" \"</SubPlans>\" \"</Plan>\")");
        Bottle b2 = b;
        CHECK(b2.size() == (size_t) 3); // copy ok level 1
        Bottle *sub = b2.get(2).asList();
        CHECK(sub!=nullptr); // list where list expected
        if (sub!=nullptr) {
            CHECK(sub->size() == (size_t) 16); // copy ok level 2
        }

        DummyConnector con;
        con.setTextMode(false);
        b.write(con.getWriter());
        Bottle b3;
        b3.read(con.getReader());
        CHECK(b3.size() == b.size()); // binary read/write ok
        sub = b3.get(2).asList();
        CHECK(sub!=nullptr); // list where list expected
        if (sub!=nullptr) {
            CHECK(sub->size() == (size_t) 16); // copy ok level 2
        }
    }

    SECTION("test null bottle")
    {
        Bottle bAll("(x 1) (y 2)");
        Bottle& b1 = bAll.findGroup("groupToFind");
        CHECK(b1.isNull()); // bottle null referenced correctly
        Bottle b2 = bAll.findGroup("groupToFind");
        CHECK(b2.isNull()); // bottle null copied correctly
    }

    SECTION("test serialization by copy")
    {
        Value v(3.14);
        Bottle b;
        b.read(v);
        CHECK(b.get(0).asFloat64() == Approx(3.14)); // copy to bottle succeeded
        Bottle b2;
        b.write(b2);
        CHECK(b2.get(0).asFloat64() == Approx(3.14)); // copy from bottle succeeded
    }

    SECTION("test string with null")
    {
        char buf1[] = "hello world";
        char buf2[] = "hello world";
        buf2[5] = '\0';
        size_t len = 11;
        std::string str1(buf1, len);
        std::string str2(buf2, len);
        CHECK(str1.length() == len); // unmodified string length ok
        CHECK(str2.length() == len); // modified string length ok
        std::string str3(str2);
        CHECK(str3.length() == len); // copied string length ok
        Bottle bot;
        bot.addString(str2);
        CHECK(bot.get(0).asString().length() == len); // bottled string asString() length ok
        CHECK(bot.get(0).toString().length() == len); // bottled string toString() length ok
        Bottle bot2 = bot;
        CHECK(bot2.get(0).asString().length() == len); // bottled, copied string length ok

        Bottle bot3;
        bot.write(bot3);
        CHECK(bot3.get(0).asString().length() == len); // bottled, serialized string length ok

        Bottle bot4;
        bot4.fromString(bot.toString());
        CHECK(bot4.get(0).asString().length() == len); // bottled, text-serialized string length ok
    }

    SECTION("test boolean values")
    {
        Bottle bot("1 true \"true\" false \"false\" 0 [ok]");
        CHECK(bot.get(1).isBool()); // true is boolean
        CHECK(!bot.get(2).isBool()); // quoted true is not boolean
        CHECK(bot.get(3).isBool()); // false is boolean
        CHECK(!bot.get(4).isBool()); // quoted false is not boolean
        CHECK(!bot.get(6).isBool()); // not all vocabs are boolean
        CHECK(bot.get(1).asBool()); // true is true
        CHECK(!bot.get(3).asBool()); // false is false
        CHECK(bot.get(1).asString() == "true");  // true can spell
        CHECK(bot.get(3).asString() == "false"); // false can spell
    }

    SECTION("test dictionary values")
    {
        Bottle bot("1");
        Property& p = bot.addDict();
        p.put("test", "me");
        p.put("hi", "there");
        Bottle bot2;
        bot2 = bot;
        CHECK(bot2.get(1).isDict()); // "dict copies ok"
        CHECK(bot2.get(1).asDict()->find("test").asString() == "me"); // "dict content copies ok"

        Bottle bot3;
        bot3.fromString(bot.toString());
        CHECK(bot2.get(1).asSearchable()->find("test").asString() == "me"); // "dict content serializes ok"
        // serialization currently will convert dicts to lists,
        // for backwards compatibility
    }

    SECTION("test infinite loop tickled by yarpmanager + string type change")
    {
        Bottle pos("Pos ((x 349.5) (y 122)) ((x 286) (y 122)) ((x 413) (y 122))");
        CHECK(pos.get(1).find("x").asFloat64() == Approx(349.5));
        CHECK(pos.get(2).find("x").asFloat64() == Approx(286.0));
        CHECK(pos.get(3).find("x").asFloat64() == Approx(413.0));
    }

    SECTION("test a string with several minus characters")
    {
        Bottle bot("---");
        CHECK(bot.get(0).asString() == "---"); // "interpreted ok"
    }

    SECTION("test copyPortable method")
    {
        Bottle b1("1 2 3"), b2;
        Portable::copyPortable(b1, b2);
        CHECK(b2.size() == b1.size()); // "length ok"
        CHECK(b2.get(2).asInt32() == 3); // "content ok"
        Stamp s1(42, 10.0), s2, s3;
        Portable::copyPortable(s1, s2);
        CHECK(s1.getCount() == 42); // "stamp-to-stamp ok"
        Portable::copyPortable(s1, b1);
        CHECK(b1.get(0).asInt32() == 42); // "stamp-to-bottle ok"
        Portable::copyPortable(b1, s3);
        CHECK(s3.getCount() == 42); // "bottle-to-stamp ok"
    }

    SECTION("test initializer_list constructor")
    {
        Bottle b { Value(1),
                   Value(2.0),
                   Value("foo")
                 };

        REQUIRE(b.size() == 3);
        CHECK(b.isNull() == false);

        CHECK(b.get(0).isInt32());
        CHECK(b.get(1).isFloat64());
        CHECK(b.get(2).isString());

        CHECK(b.get(0).asInt32() == 1);
        CHECK(b.get(1).asFloat64() == 2.0);
        CHECK(b.get(2).asString() == "foo");


    }
}

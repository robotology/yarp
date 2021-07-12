/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Property.h>
#include <yarp/os/Os.h>
#include <yarp/os/Value.h>
#include <yarp/os/Log.h>

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cfloat>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;

static void printStringToFile(const char * filename, const char * filecontent)
{
    FILE *fout = fopen(filename, "w");
    yAssert(fout!=nullptr);
    fprintf(fout, "%s", filecontent);
    fclose(fout);
    fout = nullptr;
}

TEST_CASE("os::PropertyTest", "[yarp::os]")
{
    SECTION("checking puts and gets")
    {
        Property p;
        p.put("hello", "there");
        p.put("hello", "friend");
        p.put("x", "y");
        CHECK(p.check("hello")); // key 1 exists
        CHECK(p.check("x")); // key 2 exists
        CHECK(!(p.check("y"))); // other key should not exist
        CHECK(p.find("hello").toString() =="friend"); // key 1 has good value
        CHECK(p.find("x").toString() == "y"); // key 2 has good value
        p.fromString("(hello)");
        CHECK(p.check("hello")); // key exists
        Value *v;
        CHECK_FALSE(p.check("hello", v)); // has no value
    }

    SECTION("checking puts and gets of various types")
    {
        Property p;
        p.put("ten", 10);
        p.put("pi", (double)3.14);
        CHECK(p.find("ten").asInt32() == 10); // ten
        CHECK(p.find("pi").asFloat64()>3); // pi>3
        CHECK(p.find("pi").asFloat64()<4); // pi<4
        p.unput("ten");
        CHECK(p.find("ten").isNull()); // unput
    }

    SECTION("checking that issue https://github.com/robotology/yarp/issues/1057 is properly solved")
    {
        Property p;
        double val = 1e-5;
        p.fromString("(dbl 0.00001)");
        CHECK(std::fabs(p.find("dbl").asFloat64() - val) < DBL_EPSILON); // checking 1e-5
        p.unput("dbl");
        val = 1e-6;
        p.fromString("(dbl 0.000001)");
        CHECK(std::fabs(p.find("dbl").asFloat64() - val) < DBL_EPSILON); // checking 1e-6
        p.unput("dbl");
        val = 1e-7;
        p.fromString("(dbl 0.0000001)");
        CHECK(std::fabs(p.find("dbl").asFloat64() - val) < DBL_EPSILON); // checking 1e-7
        p.unput("dbl");
        val = 1e-8;
        p.fromString("(dbl 0.00000001)");
        CHECK(std::fabs(p.find("dbl").asFloat64() - val) < DBL_EPSILON); // checking 1e-8
        p.unput("dbl");
        val = 1e-9;
        p.fromString("(dbl 0.000000001)");
        CHECK(std::fabs(p.find("dbl").asFloat64() - val) < DBL_EPSILON); // checking 1e-9
    }

    SECTION("checking that issue https://github.com/robotology/yarp/issues/1057 is properly solved")
    {
        Property p;
        p.put("ten", 10);
        CHECK(p.check("ten")); // found
        CHECK_FALSE(p.check("eleven")); // not found
        Bottle& bot = p.findGroup("twelve");
        CHECK(bot.isNull()); // group not found
    }

    SECTION("checking external forms")
    {
        Property p;
        p.fromString("(foo 12) (testing left right)");
        CHECK(p.find("foo").asInt32() == 12); // good key 1
        CHECK(p.find("testing").asString() == "left"); // good key 2
        CHECK(p.findGroup("testing").toString() == "testing left right"); // good key 2 (more)

        Property p2;
        p2.fromString(p.toString());
        CHECK(p.find("testing").asString() == "left"); // good key after copy

        Property p3;
        const char *args[] = {"CMD", "--size", "10", "20", "--mono", "on"};
        p3.fromCommand(5, args);
        Bottle bot(p3.toString());
        CHECK(bot.size() == (size_t) 2); // right number of terms
        CHECK(p3.findGroup("size").get(1).toString() == "10"); // width
        CHECK(p3.findGroup("size").get(2).toString() == "20"); // height
        CHECK(p3.findGroup("size").get(1).isInt32()); // width type
        CHECK(p3.findGroup("size").get(1).asInt32() == 10); // width type val

        INFO("reading from config-style string");
        Property p4;
        p4.fromConfig("size 10 20\nmono on\n");
        Bottle bot2(p4.toString());
        CHECK(bot2.size() == (size_t) 2); // right number of terms
        CHECK(p4.findGroup("size").get(1).toString() == "10"); // width
        CHECK(p4.findGroup("size").get(2).toString() == "20"); // height
        CHECK(p4.findGroup("size").get(1).isInt32()); // width type
        CHECK(p4.findGroup("size").get(1).asInt32() == 10); // width type val

        INFO("more realistic config-style string");
        Property p5;
        p5.fromConfig("[cat1]\nsize 10 20\nmono on\n[cat2]\nfoo\t100\n");
        Bottle bot3(p5.toString());
        CHECK(bot3.size() == (size_t) 2); // right number of terms
        CHECK(p5.findGroup("cat1").findGroup("size").get(1).asInt32() == 10); // category 1, size, width
        CHECK(p5.findGroup("cat2").findGroup("foo").get(1).asInt32() == 100); // category 2, foo

        INFO("command line style string");
        Property p6;
        const char *strs[] = { "program", "--name", "/foo" };
        p6.fromCommand(3, strs);
        CHECK(p6.find("name").asString() == "/foo"); // command line name
        Value *v = nullptr;
        p6.check("name", v);
        CHECK(v!=nullptr); // check method

        Searchable *network = &p6.findGroup("NETWORK");
        if (network->isNull()) { network = &p6; }
        v = nullptr;
        network->check("name", v);
        CHECK(v!=nullptr); // check method 2

        Property p7;
    }

    SECTION("checking command line parsing")
    {
        const char *argv[] = {
            "program",
            "--on",
            "/server",
            "--cmd",
            "\"ls foo\"",
            "--x::y::z",
            "10"
        };
        int argc = 7;
        Property p;
        p.fromCommand(argc, argv);
        CHECK(p.findGroup("x").findGroup("y").find("z").asInt32() == 10); // x::y::z ok
        Property p2("(x (y (z 45) (r 92))) (winding roads)");
        p2.fromCommand(argc, argv, true, false);
        CHECK(p2.findGroup("x").findGroup("y").find("z").asInt32() == 10); // x::y::z #2 ok
        CHECK(p2.findGroup("x").findGroup("y").find("r").asInt32() == 92); // x::y::r ok
    }

    SECTION("checking line break")
    {
        Property p;
        p.fromConfig("x to\\\ny 20\n");
        CHECK_FALSE(p.check("y")); // ran on ok
        CHECK(p.findGroup("x").get(1).asString() == "toy"); // splice ok
    }

    SECTION("checking hex")
    {
        Property p;
        p.fromString("(CanAddress 0x0C)");
        CHECK(p.find("CanAddress").asInt32() == 12); // 0x0C
        p.fromString("(CanAddress 0x0E)");
        CHECK(p.find("CanAddress").asInt32() == 14); // 0x0E
        p.fromString("(CanAddress 0x0c)");
        CHECK(p.find("CanAddress").asInt32() == 12); // 0x0c
        p.fromString("(CanAddress 0x0e)");
        CHECK(p.find("CanAddress").asInt32() == 14); // 0x0e
        p.fromString("(CanAddress 0xff)");
        CHECK(p.find("CanAddress").asInt32() == 255); // 0xff
        p.fromConfig("\
CanAddress1 0x0C\n\
CanAddress2 0x0E\n\
");
        CHECK(p.find("CanAddress1").asInt32() == 12); // config text 0x0C
        CHECK(p.find("CanAddress2").asInt32() == 14); // config text 0x0E

        const char *fname1 = "_yarp_regression_test1.txt";

        FILE *fout = fopen(fname1, "w");
        REQUIRE(fout!=nullptr);
        fprintf(fout, "CanAddress1 0x0E\n");
        fprintf(fout, "CanAddress2 0x0C\n");
        fclose(fout);
        fout = nullptr;

        p.fromConfigFile(fname1);
        CHECK(p.find("CanAddress1").asInt32() == 14); // config text 0x0E
        CHECK(p.find("CanAddress2").asInt32() == 12); // config text 0x0C
    }

    SECTION("checking copy")
    {
        Property p0;
        p0.fromString("(foo 12) (testing left right)");
        {
            Property p(p0);
            CHECK(p.find("foo").asInt32() == 12); // good key 1
            CHECK(p.find("testing").asString() == "left"); // good key 2
            CHECK(p.findGroup("testing").toString() == "testing left right"); // good key 2 (more)");
        }
        {
            Property p;
            p.fromString("bozo");
            p = p0;
            CHECK(p.find("foo").asInt32() == 12); // good key 1
            CHECK(p.find("testing").asString() == "left"); // good key 2
            CHECK(p.findGroup("testing").toString() == "testing left right"); // good key 2 (more)
        }

    }

    SECTION("checking expansion")
    {
        Property p;
        p.fromConfig("\
color red\n\
yarp1 $__YARP__\n\
yarp2 ${__YARP__}\n\
yarp3 pre_${__YARP__}_post\n\
");
        CHECK(p.find("color").asString() == "red"); // normal key
        CHECK(p.find("yarp1").asInt32() == 1); // basic expansion
        CHECK(p.find("yarp2").asInt32() == 1); // expansion with parenthesis
        CHECK(p.find("yarp3").asString() == "pre_1_post"); // expansion with neighbor

        Property env;
        env.put("TARGET", "Earth");
        env.put("WIN_PATH", "c:\\foo");
        p.fromConfig("\
targ $TARGET\n\
path $WIN_PATH\n\
", env);
        CHECK(p.find("targ").asString() == "Earth"); // environment addition
        CHECK(p.find("path").asString() == "c:\\foo"); // path interpretation

        p.fromConfig("\
x 10\n\
y 20\n\
check $x $y\n\
");
        CHECK(p.findGroup("check").get(1).asInt32() == 10); // local x is ok
        CHECK(p.findGroup("check").get(2).asInt32() == 20); // local y is ok
    }

    SECTION("checking url parsing")
    {
        Property p;
        p.fromQuery("prop1=val1&prop2=val2");
        CHECK(p.find("prop1").asString() == "val1"); // basic prop 1
        CHECK(p.find("prop2").asString() == "val2"); // basic prop 2
        p.fromQuery("http://foo.bar.org/link?prop3=val3&prop4=val4", true);
        CHECK(p.find("prop3").asString() == "val3"); // full prop 3
        CHECK(p.find("prop4").asString() == "val4"); // full prop 4
        p.fromQuery("prop1=val+one&prop2=val%2Ftwo%2C");
        CHECK(p.find("prop1").asString() == "val one"); // mix prop 1
        CHECK(p.find("prop2").asString() == "val/two,"); // mix prop 2
    }

    SECTION("checking nested forms")
    {
        Property p;
        p.fromConfig("[sect a]\nhello there\n[sect b]\nx 10\n");
        std::string sects = p.findGroup("sect").tail().toString();
        CHECK(sects == "a b"); // section list present
        std::string hello = p.findGroup("a").find("hello").asString();
        CHECK(hello == "there"); // individual sections present
    }

    SECTION("checking comments")
    {
        Property p;
        p.fromConfig("x 10\n// x 11\n");
        CHECK(p.find("x").asInt32() == 10); // comment ignored ok
        p.fromConfig("url \"http://www.robotcub.org\"\n");
        CHECK(p.find("url").asString() == "http://www.robotcub.org"); // url with // passed ok
        p.fromConfig("x 10 # 15");
        CHECK(p.findGroup("x").size() == (size_t) 2); // group size with # ok
        p.fromConfig("x 10 // 15");
        CHECK(p.findGroup("x").size() == (size_t) 2); // group size with // ok
        p.fromConfig("x \"# 1 // 2\" 4 5");
        CHECK(p.findGroup("x").size() == (size_t) 4); // group size with quoting ok
        p.fromConfig("x 10#15 4 5");
        CHECK(p.findGroup("x").size() == (size_t) 4); // group size with x#y ok
        INFO("checking comment in configuration file");
        p.fromConfig("robotName icub \n urdf_file model.urdf \n # this is trash \n");
        CHECK(p.check("#") == false); // presence of comment line properly ignored in fromConfig
        const char *fname1 = "_yarp_regression_test_ini_comments.txt";
        FILE *fout = fopen(fname1, "w");
        REQUIRE(fout!=nullptr);
        fprintf(fout, "robotName icub\n");
        fprintf(fout, "urdf_file model.urdf\n");
        fprintf(fout, "# this is trash\n");
        fclose(fout);
        fout = nullptr;
        CHECK(p.fromConfigFile(fname1) == true); // test file correctly loaded
        CHECK(p.check("#") == false); // presence of comment line properly ignored in fromConfigFile
    }

    SECTION("checking loading of list with spaces")
    {
        Property p_spaces, p_no_spaces;
        p_spaces.fromConfig("torso_yaw = ((0.275,\"0B3M0\") ,(0.275,\"0B3M1\") ,(0.55,\"0B4M0\"))\n");
        p_no_spaces.fromConfig("torso_yaw = ((0.275,\"0B3M0\"),(0.275,\"0B3M1\"),(0.55,\"0B4M0\"))\n");
        CHECK(p_no_spaces.find("torso_yaw").isList() == true); // list without spaces correctly loaded
        CHECK(p_spaces.find("torso_yaw").isList() == true); // list with spaces correctly loaded
        CHECK(p_no_spaces.find("torso_yaw").asList()->size() == (size_t) 3); // list without spaces loaded with correct size
        CHECK(p_spaces.find("torso_yaw").asList()->size() == (size_t) 3); // list with spaces loaded with correct size
    }

    SECTION("checking wipe suppression")
    {
        Property p;
        p.put("x", 12);
        p.fromConfig("y 20", false);
        CHECK(p.find("x").asInt32() == 12); // x is ok
        CHECK(p.find("y").asInt32() == 20); // y is ok
    }

    SECTION("checking backslash path behavior")
    {
        // on windows, backslashes are used in paths
        // if passed on command-line, don't be shocked
        Property p;
        std::string target = "conf\\brains-brains.ini";
        const char *argv[] = {
            "PROGRAM NAME",
            "--file",
            (char*)target.c_str()
        };
        int argc = 3;
        p.fromCommand(argc, argv);
        CHECK(p.find("file").asString() == target); // string with slash
    }

    SECTION("checking include behavior")
    {

        const char *fname1 = "_yarp_regression_test1.txt";
        const char *fname2 = "_yarp_regression_test2.txt";

        // create some test files

        {
            FILE *fout = fopen(fname1, "w");
            REQUIRE(fout!=nullptr);
            fprintf(fout, "x 1\n");
            fclose(fout);
            fout = nullptr;

            fout = fopen(fname2, "w");
            REQUIRE(fout!=nullptr);
            fprintf(fout, "[include %s]\n", fname1);
            fprintf(fout, "y 2\n");
            fclose(fout);
            fout = nullptr;

            Property p;
            p.fromConfigFile(fname2);
            CHECK(p.find("x").asInt32() == 1); // x is ok
            CHECK(p.find("y").asInt32() == 2); // y is ok
        }


        {
            FILE *fout = fopen(fname1, "w");
            REQUIRE(fout!=nullptr);
            fprintf(fout, "x 1\n");
            fclose(fout);
            fout = nullptr;

            fout = fopen(fname2, "w");
            REQUIRE(fout!=nullptr);
            fprintf(fout, "[include base %s]\n", fname1);
            fprintf(fout, "y 2\n");
            fclose(fout);
            fout = nullptr;

            Property p;
            p.fromConfigFile(fname2);
            CHECK(p.findGroup("base").find("x").asInt32() == 1); // x is ok
            CHECK(p.find("y").asInt32() == 2); // y is ok
            CHECK(p.findGroup("base").toString() == "base (x 1)"); // expected external structure
        }

        {
            FILE *fout = fopen(fname1, "w");
            REQUIRE(fout!=nullptr);
            fprintf(fout, "x 1\n");
            fclose(fout);
            fout = nullptr;

            fout = fopen(fname2, "w");
            REQUIRE(fout!=nullptr);
            fprintf(fout, "[base]\n");
            fprintf(fout, "w 4\n");
            fprintf(fout, "[base]\n");
            fprintf(fout, "z 3\n");
            fprintf(fout, "[include base %s]\n", fname1);
            fprintf(fout, "y 2\n");
            fclose(fout);
            fout = nullptr;

            Property p;
            p.fromConfigFile(fname2);
            CHECK(p.findGroup("base").find("x").asInt32() == 1); // x is ok
            CHECK(p.find("y").asInt32() == 2); // y is ok
            CHECK(p.findGroup("base").find("z").asInt32() == 3); // z is ok
            CHECK(p.findGroup("base").find("w").asInt32() == 4); // w is ok
        }

        {
            FILE *fout = fopen(fname1, "w");
            REQUIRE(fout!=nullptr);
            fprintf(fout, "x 1\n");
            fclose(fout);
            fout = nullptr;

            fout = fopen(fname2, "w");
            REQUIRE(fout!=nullptr);
            fprintf(fout, "[b1]\n");
            fprintf(fout, "z 3\n");
            fprintf(fout, "[include base b1 %s]\n", fname1);
            fprintf(fout, "[include base b2 %s]\n", fname1);
            fprintf(fout, "y 2\n");
            fclose(fout);
            fout = nullptr;

            Property p;
            p.fromConfigFile(fname2);
            CHECK(p.findGroup("b1").find("x").asInt32() == 1); // x is ok
            CHECK(p.findGroup("base").get(1).asString() == "b1"); // list element 1 is ok
            CHECK(p.findGroup("b2").find("x").asInt32() == 1); // x is ok
            CHECK(p.findGroup("base").get(2).asString() == "b2"); // list element 2 is ok
        }
    }

    SECTION("checking import behavior")
    {
        {
            const char* fname1 = "_yarp_regression_import.txt";
            FILE* fout = fopen(fname1, "w");
            REQUIRE(fout != nullptr);
            fprintf(fout, "[import context1 c1f1.ini]\n");
            fclose(fout);
            fout = nullptr;

            Property p;
            p.fromConfigFile(fname1);
            yDebug("Expanded property: %s", p.toString().c_str());
            CHECK(p.find                    ("c1f1p1").asInt32()   == 10);
            CHECK(p.find                    ("c1f1p2").asInt32()   == 20);
            CHECK(p.find                    ("c1f1p3").asInt32()   == 30);
            CHECK(p.findGroup("c1f1s1").find("c1f1s1p1").asInt32() == 40);
            CHECK(p.findGroup("c1f1s1").find("c1f1s1p2").asInt32() == 50);
            CHECK(p.find                    ("c2f1p1").asInt32()   == 11);
            CHECK(p.find                    ("c2f1p2").asInt32()   == 12);
            CHECK(p.findGroup("c2f1s1").find("c2f1s1p1").asInt32() == 13);
            CHECK(p.findGroup("c2f1s1").find("c2f1s1p2").asInt32() == 14);
            CHECK(p.find                    ("c2f2p1").asInt32()   == 15);
            CHECK(p.find                    ("c2f2p2").asInt32()   == 16);
            CHECK(p.findGroup("c2f2s1").find("c2f2s1p1").asInt32() == 17);
            CHECK(p.findGroup("c2f2s1").find("c2f2s1p2").asInt32() == 18);
        }
    }

    SECTION("checking that issue https://github.com/robotology/yarp/issues/459 is properly solved")
    {

        // create test files
        const char *include_one_name = "_yarp_regression_include_one.ini";
        const char *include_one_content = "[include_one]\n"
                                          "incl_param1   red\n"
                                          "incl_param2   green\n"
                                          "incl_param3   blue\n";

        const char *include_two_name = "_yarp_regression_include_two.ini";
        const char *include_two_content = "[include_two]\n"
                                          "incl2_paramA   car\n"
                                          "incl2_paramB   table\n";

        const char *root_file_name   = "_yarp_regression_root_file.ini";
        const char *root_file_content   = "[include \"_yarp_regression_include_one.ini\"]\n"
                                           "                             \n"
                                           "[include \"_yarp_regression_include_two.ini\"]  \n"
                                           "[root_group]                 \n"
                                           "ciao hello                   \n"
                                           "bau 10                     \n"
                                           "                             \n"
                                           "                             \n"
                                           "                             \n"
                                           "[end_group]                  \n"
                                           "lili  lolo                   \n"
                                           "cici  bubu                   \n";

        const char *root_file_check_name   = "_yarp_regression_root_file_check.ini";
        const char *root_file_check_content   = "[include \"_yarp_regression_include_one.ini\"]\n"
                                           "                             \n"
                                           "[root_group]                 \n"
                                           "ciao hello                   \n"
                                           "bau 10                     \n"
                                           "                             \n"
                                           "                             \n"
                                           "[include \"_yarp_regression_include_two.ini\"]  \n"
                                           "                             \n"
                                           "[end_group]                  \n"
                                           "lili  lolo                   \n"
                                           "cici  bubu                   \n";

        printStringToFile(include_one_name, include_one_content);
        printStringToFile(include_two_name, include_two_content);
        printStringToFile(root_file_name, root_file_content);
        printStringToFile(root_file_check_name, root_file_check_content);

        // load the property and check that root_group is actually present
        // in all groups

        Property propRoot, propRootCheck;
        INFO("Parsing root_file ");
        propRoot.fromConfigFile(root_file_name);
        INFO("Parsing root_file_check ");
        propRootCheck.fromConfigFile(root_file_check_name);
        CHECK(propRoot.findGroup("root_group").find("bau").asInt32() == 10); // root_group is found in root_file
        CHECK(propRootCheck.findGroup("root_group").find("bau").asInt32() == 10); // root_group is found in root_file_check

    }

    SECTION("checking command line parsing")
    {
        const char *argv[] = {
            "program",
            "--on",
            "/server",
            "--cmd",
            "\"ls foo\""
        };
        int argc = 5;
        Property p;
        p.fromCommand(argc, argv);
        std::string target1 = "(cmd \"ls foo\") (on \"/server\")";
        std::string target2 = "(on \"/server\") (cmd \"ls foo\")";
        std::string actual = p.toString();
        if (actual==target1) {
            CHECK(actual == target1); // command ok
        } else {
            CHECK(actual == target2); // command ok
        }
    }

    SECTION("checking directory scanning")
    {
        // change directory name if test files removed
        std::string dirname = "__test_dir_1";
        if (yarp::os::stat(dirname.c_str())<0) {
            yarp::os::mkdir(dirname.c_str());
        }
        CHECK(yarp::os::stat(dirname.c_str())>=0); // test directory present
        {
            FILE *fout = fopen((dirname + "/t1.ini").c_str(), "w");
            REQUIRE(fout!=nullptr);
            fprintf(fout, "x 3\n");
            fprintf(fout, "[nesttest]\n");
            fprintf(fout, "z 14\n");
            fclose(fout);
            fout = nullptr;
        }
        {
            FILE *fout = fopen((dirname + "/t2.ini").c_str(), "w");
            REQUIRE(fout!=nullptr);
            fprintf(fout, "y 4\n");
            fclose(fout);
            fout = nullptr;
        }
        Property p;
        p.fromConfigFile(dirname.c_str());
        CHECK(p.find("x").asInt32() == 3); // t1 read
        CHECK(p.find("y").asInt32() == 4); // t2 read
    }

    SECTION("checking long long hex")
    {
        const char* parms[]={"foo", "--longlonghex", "0xFEDCBA9876543210"};
        yarp::os::Property config;
        config.fromCommand(3, parms);
        CHECK(config.find("longlonghex").asString() == "0xFEDCBA9876543210"); // hex that is too big remains a string
    }

    SECTION("check add group")
    {
        Property p;
        p.put("x", 1);
        Property& psub = p.addGroup("psub");
        psub.put("y", 2);
        CHECK(p.find("x").asInt32() == 1); // basic int
        CHECK(p.findGroup("psub").find("y").asInt32() == 2); // nested int
        Property pCopy = p;
        CHECK(pCopy.toString() == p.toString()); // test if addGroup works fine with Property copy assigment
        Property pCopy2;
        pCopy2 = p;
        CHECK(pCopy.toString() == p.toString()); // test if addGroup works fine with Property copy operator
    }

    SECTION("checking initializer_list constructor")
    {
        Property p {{"one", Value(1)},
                    {"two", Value(2.0)},
                    {"string", Value("foo")}
                    };

        CHECK(p.find("one").isInt32());
        CHECK(p.find("two").isFloat64());
        CHECK(p.find("string").isString());

        CHECK(p.find("one").asInt32() == 1);
        CHECK(p.find("two").asFloat64() == 2.0);
        CHECK(p.find("string").asString() == "foo");
    }
}

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Property.h>
#include <yarp/os/Os.h>

#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/impl/Logger.h>

#include <yarp/os/impl/PlatformStdlib.h>

#include <stdio.h>

using namespace yarp::os::impl;
using namespace yarp::os;

class PropertyTest : public UnitTest {
public:
    virtual String getName() { return "PropertyTest"; }

    void checkPutGet() {
        report(0,"checking puts and gets");
        Property p;
        p.put("hello","there");
        p.put("hello","friend");
        p.put("x","y");
        checkTrue(p.check("hello"), "key 1 exists");
        checkTrue(p.check("x"), "key 2 exists");
        checkTrue(!(p.check("y")), "other key should not exist");
        checkEqual(p.find("hello").toString().c_str(),"friend",
                   "key 1 has good value");
        checkEqual(p.find("x").toString().c_str(),"y",
                   "key 2 has good value");
        p.fromString("(hello)");
        checkTrue(p.check("hello"), "key exists");
        Value *v;
        checkFalse(p.check("hello",v), "has no value");
    }


    void checkTypes() {
        report(0,"checking puts and gets of various types");
        Property p;
        p.put("ten",10);
        p.put("pi",(double)3.14);
        checkEqual(p.find("ten").asInt(),10,"ten");
        checkTrue(p.find("pi").asDouble()>3,"pi>3");
        checkTrue(p.find("pi").asDouble()<4,"pi<4");
        p.unput("ten");
        checkTrue(p.find("ten").isNull(),"unput");
    }


    void checkNegative() {
        report(0,"checking things are NOT found in correct manner");
        Property p;
        p.put("ten",10);
        checkTrue(p.check("ten"),"found");
        checkFalse(p.check("eleven"),"not found");
        Bottle& bot = p.findGroup("twelve");
        checkTrue(bot.isNull(),"group not found");
    }

    void checkExternal() {
        report(0,"checking external forms");
        Property p;
        p.fromString("(foo 12) (testing left right)");
        checkEqual(p.find("foo").asInt(),12,"good key 1");
        checkEqual(p.find("testing").asString().c_str(),"left","good key 2");
        checkEqual(p.findGroup("testing").toString().c_str(),
                   "testing left right","good key 2 (more)");

        Property p2;
        p2.fromString(p.toString().c_str());
        checkEqual(p.find("testing").asString().c_str(),"left","good key after copy");

        Property p3;
        const char *args[] = {"CMD","--size","10","20","--mono","on"};
        p3.fromCommand(5,args);
        Bottle bot(p3.toString().c_str());
        checkEqual(bot.size(),2,"right number of terms");
        checkEqual(p3.findGroup("size").get(1).toString().c_str(),"10","width");
        checkEqual(p3.findGroup("size").get(2).toString().c_str(),"20","height");
        checkTrue(p3.findGroup("size").get(1).isInt(),"width type");
        checkEqual(p3.findGroup("size").get(1).asInt(),10,"width type val");

        report(0,"reading from config-style string");
        Property p4;
        p4.fromConfig("size 10 20\nmono on\n");
        Bottle bot2(p4.toString().c_str());
        checkEqual(bot2.size(),2,"right number of terms");
        checkEqual(p4.findGroup("size").get(1).toString().c_str(),"10","width");
        checkEqual(p4.findGroup("size").get(2).toString().c_str(),"20","height");
        checkTrue(p4.findGroup("size").get(1).isInt(),"width type");
        checkEqual(p4.findGroup("size").get(1).asInt(),10,"width type val");

        report(0,"more realistic config-style string");
        Property p5;
        p5.fromConfig("[cat1]\nsize 10 20\nmono on\n[cat2]\nfoo\t100\n");
        Bottle bot3(p5.toString().c_str());
        checkEqual(bot3.size(),2,"right number of terms");
        checkEqual(p5.findGroup("cat1").findGroup("size").get(1).asInt(),
                   10,"category 1, size, width");
        checkEqual(p5.findGroup("cat2").findGroup("foo").get(1).asInt(),
                   100,"category 2, foo");

        report(0,"command line style string");
        Property p6;
        const char *strs[] = { "program", "--name", "/foo" };
        p6.fromCommand(3,strs);
        checkEqual(p6.find("name").asString().c_str(),"/foo",
                   "command line name");
        Value *v = NULL;
        p6.check("name",v);
        checkTrue(v!=NULL,"check method");

        Searchable *network = &p6.findGroup("NETWORK");
        if (network->isNull()) { network = &p6; }
        v = NULL;
        network->check("name",v);
        checkTrue(v!=NULL,"check method 2");

        Property p7;
    }

    void checkNestedCommandLine() {
        report(0,"checking command line parsing");
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
        p.fromCommand(argc,argv);
        checkEqual(p.findGroup("x").findGroup("y").find("z").asInt(),10,"x::y::z ok");
        Property p2("(x (y (z 45) (r 92))) (winding roads)");
        p2.fromCommand(argc,argv,true,false);
        checkEqual(p2.findGroup("x").findGroup("y").find("z").asInt(),10,"x::y::z #2 ok");
        checkEqual(p2.findGroup("x").findGroup("y").find("r").asInt(),92,"x::y::r ok");
    }

    void checkLineBreak() {
        report(0,"checking line break");
        Property p;
        p.fromConfig("x to\\\ny 20\n");
        checkFalse(p.check("y"),"ran on ok");
        checkEqual(p.findGroup("x").get(1).asString().c_str(),"toy","splice ok");
    }

    void checkHex() {
        report(0,"checking hex");
        Property p;
        p.fromString("(CanAddress 0x0C)");
        checkEqual(p.find("CanAddress").asInt(),12,"0x0C");
        p.fromString("(CanAddress 0x0E)");
        checkEqual(p.find("CanAddress").asInt(),14,"0x0E");
        p.fromString("(CanAddress 0x0c)");
        checkEqual(p.find("CanAddress").asInt(),12,"0x0c");
        p.fromString("(CanAddress 0x0e)");
        checkEqual(p.find("CanAddress").asInt(),14,"0x0e");
        p.fromString("(CanAddress 0xff)");
        checkEqual(p.find("CanAddress").asInt(),255,"0xff");
        p.fromConfig("\
CanAddress1 0x0C\n\
CanAddress2 0x0E\n\
");
        checkEqual(p.find("CanAddress1").asInt(),12,"config text 0x0C");
        checkEqual(p.find("CanAddress2").asInt(),14,"config text 0x0E");

        const char *fname1 = "_yarp_regression_test1.txt";

        FILE *fout = fopen(fname1,"w");
        yAssert(fout!=NULL);
        fprintf(fout,"CanAddress1 0x0E\n");
        fprintf(fout,"CanAddress2 0x0C\n");
        fclose(fout);
        fout = NULL;

        p.fromConfigFile(fname1);
        checkEqual(p.find("CanAddress1").asInt(),14,"config text 0x0E");
        checkEqual(p.find("CanAddress2").asInt(),12,"config text 0x0C");
    }

    virtual void checkCopy() {
        report(0,"checking copy");
        Property p0;
        p0.fromString("(foo 12) (testing left right)");
        {
            Property p(p0);
            checkEqual(p.find("foo").asInt(),12,"good key 1");
            checkEqual(p.find("testing").asString().c_str(),"left",
                       "good key 2");
            checkEqual(p.findGroup("testing").toString().c_str(),
                       "testing left right","good key 2 (more)");
        }
        {
            Property p;
            p.fromString("bozo");
            p = p0;
            checkEqual(p.find("foo").asInt(),12,"good key 1");
            checkEqual(p.find("testing").asString().c_str(),"left",
                       "good key 2");
            checkEqual(p.findGroup("testing").toString().c_str(),
                       "testing left right","good key 2 (more)");
        }

    }


    virtual void checkExpansion() {
        report(0,"checking expansion");
        Property p;
        p.fromConfig("\
color red\n\
yarp1 $__YARP__\n\
yarp2 ${__YARP__}\n\
yarp3 pre_${__YARP__}_post\n\
");
        checkEqual(p.find("color").asString().c_str(),"red","normal key");
        checkEqual(p.find("yarp1").asInt(),1,"basic expansion");
        checkEqual(p.find("yarp2").asInt(),1,"expansion with parenthesis");
        checkEqual(p.find("yarp3").asString().c_str(),"pre_1_post",
                   "expansion with neighbor");

        Property env;
        env.put("TARGET","Earth");
        env.put("WIN_PATH","c:\\foo");
        p.fromConfig("\
targ $TARGET\n\
path $WIN_PATH\n\
",env);
        checkEqual(p.find("targ").asString().c_str(),"Earth",
                   "environment addition");
        checkEqual(p.find("path").asString().c_str(),"c:\\foo",
                   "path interpretation");

        p.fromConfig("\
x 10\n\
y 20\n\
check $x $y\n\
");
        checkEqual(p.findGroup("check").get(1).asInt(),10,"local x is ok");
        checkEqual(p.findGroup("check").get(2).asInt(),20,"local y is ok");
    }


    virtual void checkUrl() {
        report(0,"checking url parsing");
        Property p;
        p.fromQuery("prop1=val1&prop2=val2");
        checkEqual(p.find("prop1").asString().c_str(),"val1","basic prop 1");
        checkEqual(p.find("prop2").asString().c_str(),"val2","basic prop 2");
        p.fromQuery("http://foo.bar.org/link?prop3=val3&prop4=val4",true);
        checkEqual(p.find("prop3").asString().c_str(),"val3","full prop 3");
        checkEqual(p.find("prop4").asString().c_str(),"val4","full prop 4");
        p.fromQuery("prop1=val+one&prop2=val%2Ftwo%2C");
        checkEqual(p.find("prop1").asString().c_str(),"val one","mix prop 1");
        checkEqual(p.find("prop2").asString().c_str(),"val/two,","mix prop 2");
    }


    void checkNesting() {
        report(0,"checking nested forms");
        Property p;
        p.fromConfig("[sect a]\nhello there\n[sect b]\nx 10\n");
        ConstString sects = p.findGroup("sect").tail().toString();
        checkEqual(sects.c_str(),"a b","section list present");
        ConstString hello = p.findGroup("a").find("hello").asString();
        checkEqual(hello.c_str(),"there","individual sections present");
    }

    void checkComment() {
        report(0,"checking comments");
        Property p;
        p.fromConfig("x 10\n// x 11\n");
        checkEqual(p.find("x").asInt(),10,"comment ignored ok");
        p.fromConfig("url \"http://www.robotcub.org\"\n");
        checkEqual(p.find("url").asString().c_str(),"http://www.robotcub.org","url with // passed ok");
        p.fromConfig("x 10 # 15");
        checkEqual(p.findGroup("x").size(),2,"group size with # ok");
        p.fromConfig("x 10 // 15");
        checkEqual(p.findGroup("x").size(),2,"group size with // ok");
        p.fromConfig("x \"# 1 // 2\" 4 5");
        checkEqual(p.findGroup("x").size(),4,"group size with quoting ok");
        p.fromConfig("x 10#15 4 5");
        checkEqual(p.findGroup("x").size(),4,"group size with x#y ok");
        report(0,"checking comment in configuration file");
        p.fromConfig("robotName icub \n urdf_file model.urdf \n # this is trash \n");
        checkEqual(p.check("#"),false,"presence of comment line properly ignored in fromConfig");
        const char *fname1 = "_yarp_regression_test_ini_comments.txt";
        FILE *fout = fopen(fname1,"w");
        yAssert(fout!=NULL);
        fprintf(fout,"robotName icub\n");
        fprintf(fout,"urdf_file model.urdf\n");
        fprintf(fout,"# this is trash\n");
        fclose(fout);
        fout = NULL;
        checkEqual(p.fromConfigFile(fname1),true,"test file correctly loaded");
        checkEqual(p.check("#"),false,"presence of comment line properly ignored in fromConfigFile");
    }

    void checkListWithSpaces()
    {
        report(0,"checking loading of list with spaces");
        Property p_spaces, p_no_spaces;
        p_spaces.fromConfig("torso_yaw = ((0.275,\"0B3M0\") ,(0.275,\"0B3M1\") ,(0.55,\"0B4M0\"))\n");
        p_no_spaces.fromConfig("torso_yaw = ((0.275,\"0B3M0\"),(0.275,\"0B3M1\"),(0.55,\"0B4M0\"))\n");
        checkEqual(p_no_spaces.find("torso_yaw").isList(),true,"list without spaces correctly loaded");
        checkEqual(p_spaces.find("torso_yaw").isList(),true,"list with spaces correctly loaded");
        checkEqual(p_no_spaces.find("torso_yaw").asList()->size(),3,"list without spaces loaded with correct size");
        checkEqual(p_spaces.find("torso_yaw").asList()->size(),3,"list with spaces loaded with correct size");
    }

    virtual void checkWipe() {
        report(0,"checking wipe suppression");
        Property p;
        p.put("x",12);
        p.fromConfig("y 20",false);
        checkEqual(p.find("x").asInt(),12,"x is ok");
        checkEqual(p.find("y").asInt(),20,"y is ok");
    }

    virtual void checkBackslashPath() {
        // on windows, backslashes are used in paths
        // if passed on command-line, don't be shocked
        report(0,"checking backslash path behavior");
        Property p;
        ConstString target = "conf\\brains-brains.ini";
        const char *argv[] = {
            "PROGRAM NAME",
            "--file",
            (char*)target.c_str()
        };
        int argc = 3;
        p.fromCommand(argc,argv);
        checkEqual(p.find("file").asString().c_str(),target.c_str(),
                   "string with slash");
    }


    virtual void checkIncludes() {
        report(0,"checking include behavior");

        const char *fname1 = "_yarp_regression_test1.txt";
        const char *fname2 = "_yarp_regression_test2.txt";

        // create some test files

        {
            FILE *fout = fopen(fname1,"w");
            yAssert(fout!=NULL);
            fprintf(fout,"x 1\n");
            fclose(fout);
            fout = NULL;

            fout = fopen(fname2,"w");
            yAssert(fout!=NULL);
            fprintf(fout,"[include %s]\n",fname1);
            fprintf(fout,"y 2\n");
            fclose(fout);
            fout = NULL;

            Property p;
            p.fromConfigFile(fname2);
            checkEqual(p.find("x").asInt(),1,"x is ok");
            checkEqual(p.find("y").asInt(),2,"y is ok");
        }


        {
            FILE *fout = fopen(fname1,"w");
            yAssert(fout!=NULL);
            fprintf(fout,"x 1\n");
            fclose(fout);
            fout = NULL;

            fout = fopen(fname2,"w");
            yAssert(fout!=NULL);
            fprintf(fout,"[include base %s]\n",fname1);
            fprintf(fout,"y 2\n");
            fclose(fout);
            fout = NULL;

            /*
            ofstream fout1(fname1);
            fout1 << "x 1" << endl;
            fout1.close();
            ofstream fout2(fname2);
            fout2 << "[include base " << fname1 << "]" << endl;
            fout2 << "y 2" << endl;
            fout2.close();
            */
            Property p;
            p.fromConfigFile(fname2);
            checkEqual(p.findGroup("base").find("x").asInt(),1,"x is ok");
            checkEqual(p.find("y").asInt(),2,"y is ok");
            checkEqual(p.findGroup("base").toString().c_str(),
                       "base (x 1)","expected external structure");
        }

        {
            FILE *fout = fopen(fname1,"w");
            yAssert(fout!=NULL);
            fprintf(fout,"x 1\n");
            fclose(fout);
            fout = NULL;

            fout = fopen(fname2,"w");
            yAssert(fout!=NULL);
            fprintf(fout,"[base]\n");
            fprintf(fout,"w 4\n");
            fprintf(fout,"[base]\n");
            fprintf(fout,"z 3\n");
            fprintf(fout,"[include base %s]\n",fname1);
            fprintf(fout,"y 2\n");
            fclose(fout);
            fout = NULL;

            Property p;
            p.fromConfigFile(fname2);
            checkEqual(p.findGroup("base").find("x").asInt(),1,"x is ok");
            checkEqual(p.find("y").asInt(),2,"y is ok");
            checkEqual(p.findGroup("base").find("z").asInt(),3,"z is ok");
            checkEqual(p.findGroup("base").find("w").asInt(),4,"w is ok");
        }

        {
            FILE *fout = fopen(fname1,"w");
            yAssert(fout!=NULL);
            fprintf(fout,"x 1\n");
            fclose(fout);
            fout = NULL;

            fout = fopen(fname2,"w");
            yAssert(fout!=NULL);
            fprintf(fout,"[b1]\n");
            fprintf(fout,"z 3\n");
            fprintf(fout,"[include base b1 %s]\n",fname1);
            fprintf(fout,"[include base b2 %s]\n",fname1);
            fprintf(fout,"y 2\n");
            fclose(fout);
            fout = NULL;

            Property p;
            p.fromConfigFile(fname2);
            checkEqual(p.findGroup("b1").find("x").asInt(),1,"x is ok");
            checkEqual(p.findGroup("base").get(1).asString().c_str(),
                       "b1","list element 1 is ok");
            checkEqual(p.findGroup("b2").find("x").asInt(),1,"x is ok");
            checkEqual(p.findGroup("base").get(2).asString().c_str(),
                       "b2","list element 2 is ok");
        }
    }

    virtual void printStringToFile(const char * filename, const char * filecontent)
    {
        FILE *fout = fopen(filename,"w");
        yAssert(fout!=NULL);
        fprintf(fout,filecontent);
        fclose(fout);
        fout = NULL;
    }

    virtual void checkIncludesIssue459() {
        report(0,"checking that issue https://github.com/robotology/yarp/issues/459 is properly solved");

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

        printStringToFile(include_one_name,include_one_content);
        printStringToFile(include_two_name,include_two_content);
        printStringToFile(root_file_name,root_file_content);
        printStringToFile(root_file_check_name,root_file_check_content);

        // load the property and check that root_group is actually present
        // in all groups

        Property propRoot,propRootCheck;
        propRoot.fromConfigFile(root_file_name);
        propRootCheck.fromConfigFile(root_file_check_name);
        checkEqual(propRoot.findGroup("root_group").find("bau").asInt(),10,"root_group is found in root_file");
        checkEqual(propRootCheck.findGroup("root_group").find("bau").asInt(),10,"root_group is found in root_file_check");

    }


    virtual void checkCommand() {
        report(0,"checking command line parsing");
        const char *argv[] = {
            "program",
            "--on",
            "/server",
            "--cmd",
            "\"ls foo\""
        };
        int argc = 5;
        Property p;
        p.fromCommand(argc,argv);
        ConstString target1 = "(cmd \"ls foo\") (on \"/server\")";
        ConstString target2 = "(on \"/server\") (cmd \"ls foo\")";
        ConstString actual = p.toString();
        if (actual==target1) {
            checkEqual(actual.c_str(),target1.c_str(),"command ok");
        } else {
            checkEqual(actual.c_str(),target2.c_str(),"command ok");
        }
    }

    virtual void checkDirectory() {
        report(0,"checking directory scanning");
        // change directory name if test files removed
        ConstString dirname = "__test_dir_1";
        ACE_stat sb;
        if (ACE_OS::stat(dirname.c_str(),&sb)<0) {
            yarp::os::mkdir(dirname.c_str());
        }
        checkTrue(ACE_OS::stat(dirname.c_str(),&sb)>=0,"test directory present");
        {
            FILE *fout = fopen((dirname + "/t1.ini").c_str(),"w");
            yAssert(fout!=NULL);
            fprintf(fout,"x 3\n");
            fprintf(fout,"[nesttest]\n");
            fprintf(fout,"z 14\n");
            fclose(fout);
            fout = NULL;
        }
        {
            FILE *fout = fopen((dirname + "/t2.ini").c_str(),"w");
            yAssert(fout!=NULL);
            fprintf(fout,"y 4\n");
            fclose(fout);
            fout = NULL;
        }
        Property p;
        p.fromConfigFile(dirname.c_str());
        checkEqual(p.find("x").asInt(),3,"t1 read");
        checkEqual(p.find("y").asInt(),4,"t2 read");
    }

    virtual void checkMonitor() {
        report(0,"checking monitoring");
    }

    virtual void checkLongLongHex() {
        report(0,"checking long long hex");
        const char* parms[]={"foo","--longlonghex","0xFEDCBA9876543210"};
        yarp::os::Property config;
        config.fromCommand(3,parms);
        checkEqual(config.find("longlonghex").asString(),
                   "0xFEDCBA9876543210","hex that is too big remains a string");
    }

    virtual void checkAddGroup() {
        report(0,"check add group");
        Property p;
        p.put("x",1);
        Property& psub = p.addGroup("psub");
        psub.put("y",2);
        checkEqual(p.find("x").asInt(),1,"basic int");
        checkEqual(p.findGroup("psub").find("y").asInt(),2,"nested int");
    }

    virtual void runTests() {
        checkPutGet();
        checkExternal();
        checkTypes();
        checkNegative();
        checkCopy();
        checkExpansion();
        checkUrl();
        checkNesting();
        checkWipe();
        checkBackslashPath();
        checkIncludes();
        checkIncludesIssue459();
        checkCommand();
        checkComment();
        checkListWithSpaces();
        checkLineBreak();
        checkMonitor();
        checkHex();
        checkNestedCommandLine();
        checkDirectory();
        checkLongLongHex();
        checkAddGroup();
    }
};

static PropertyTest thePropertyTest;

UnitTest& getPropertyTest() {
    return thePropertyTest;
}

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Network.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os;
using namespace yarp::os::impl;

#include <stdio.h>

class ResourceFinderTest : public UnitTest {
public:
    virtual String getName() { return "ResourceFinderTest"; }

    void testBasics() {
        report(0,"testing the basics of RF...");
        ResourceFinder rf;

        const char *fname0 = "_yarp_regression_test.ini";
        const char *fname1 = "_yarp_regression_test_rf1.txt";
        const char *fname2 = "_yarp_regression_test_rf2.txt";

        // create some test files

        {
            FILE *fout = fopen(fname0,"w");
            YARP_ASSERT(fout!=NULL);
            fprintf(fout,"style capability\n");
            fprintf(fout,"capability_directory \".\"\n");
            fprintf(fout,"default_capability \".\"\n");
            fclose(fout);
            fout = NULL;

            fout = fopen(fname1,"w");
            YARP_ASSERT(fout!=NULL);
            fprintf(fout,"alt %s\n", fname2);
            fclose(fout);
            fout = NULL;

            fout = fopen(fname2,"w");
            fprintf(fout,"x 14\n");
            fclose(fout);
            fout = NULL;

            const char *argv[] = { "ignore", 
                                   "--policy", "_yarp_regression_test",
                                   "--_yarp_regression_test", ".",
                                   "--from", fname1, 
                                   "--verbose", "0",
                                   NULL };
            int argc = 9;

            rf.configure("none",argc,(char **)argv);
            ConstString alt = rf.findFile("alt");
            checkTrue(alt!="","found ini file");

            rf.setDefault("alt2",fname2);
            alt = rf.findFile("alt2");
            checkTrue(alt!="","default setting worked");

            rf.setDefault("alt3","_yarp_nonexistent.txt");
            alt = rf.findFile("alt3");
            checkTrue(alt=="","cannot find nonexistent files");

            rf.setDefault("alt","_yarp_nonexistent.txt");
            alt = rf.findFile("alt");
            checkTrue(alt!="","default setting is safe");

            checkTrue(rf.findPath()!="","existing path found");
        }
        
    }

    void testCommandLineArgs() {
        report(0,"make sure command line args take priority...");

        const char *fname0 = "_yarp_regression_test.ini";
        const char *fname1 = "_yarp_regression_test_rf1.txt";

        // create some test files

        {
            FILE *fout = fopen(fname0,"w");
            YARP_ASSERT(fout!=NULL);
            fprintf(fout,"style capability\n");
            fprintf(fout,"capability_directory \".\"\n");
            fprintf(fout,"default_capability \".\"\n");
            fclose(fout);
            fout = NULL;

            fout = fopen(fname1,"w");
            fprintf(fout,"x 14\n");
            fclose(fout);
            fout = NULL;

            const char *argv[] = { "ignore", 
                                   "--policy", "_yarp_regression_test",
                                   "--_yarp_regression_test", ".",
                                   "--from", fname1, 
                                   "--verbose", "0",
                                   NULL };
            int argc = 9;

            ResourceFinder rf1;
            rf1.configure("none",argc,(char **)argv);
            checkEqual(rf1.find("x").asInt(),14,"found x");

            const char *argv2[] = { "ignore", 
                                    "--policy", "_yarp_regression_test",
                                    "--_yarp_regression_test", ".",
                                    "--from", fname1, 
                                    "--verbose", "0",
                                    "--x", "20",
                                    "--y", "30",
                                    NULL };
            int argc2 = 13;

            ResourceFinder rf2;
            rf2.configure("none",argc2,(char **)argv2);
            checkEqual(rf2.find("y").asInt(),30,"found y");
            checkEqual(rf2.find("x").asInt(),20,"override x");
        }

    }


    void testContext() {
        report(0,"test context setting");
        ResourceFinder rf;
        const char *argv[] = { "ignore", 
                               "--policy", "_yarp_regression_test",
                               "--_yarp_regression_test", ".",
                               "--context", "zig", 
                               "--verbose", "0",
                               NULL };
        int argc = 9;
        rf.configure("none",argc,(char **)argv);
        checkEqual(rf.getContext().c_str(),"zig","recovered context");
    }

    void testSubGroup() {
        report(0,"test subgroup");
        const char *fname0 = "_yarp_regression_subgroup_test.ini";
        const char *fname1 = "_yarp_regression_subgroup_test_rf1.txt";
        const char *fname2 = "_yarp_regression_subgroup_test_rf2.txt";
        {
            FILE *fout = fopen(fname0,"w");
            YARP_ASSERT(fout!=NULL);
            fprintf(fout,"[section1]\n");
            fprintf(fout,"fname \"_yarp_regression_subgroup_test_rf1.txt\"\n");
            fprintf(fout,"[section2]\n");
            fprintf(fout,"fname \"_yarp_regression_subgroup_test_rf2.txt\"\n");
            fclose(fout);
            fout = NULL;
        }
        {
            FILE *fout = fopen(fname1,"w");
            YARP_ASSERT(fout!=NULL);
            fprintf(fout,"x 1\n");
            fclose(fout);
            fout = NULL;
        }
        {
            FILE *fout = fopen(fname2,"w");
            YARP_ASSERT(fout!=NULL);
            fprintf(fout,"x 2\n");
            fclose(fout);
            fout = NULL;
        }
        ResourceFinder rf;
        const char *argv[] = { "ignore", 
                               "--policy", "_yarp_regression_test",
                               "--_yarp_regression_test", ".",
                               "--from", fname0, 
                               "--verbose", "0",
                               NULL };
        int argc = 9;
        rf.configure("none",argc,(char **)argv);
        ResourceFinder rf1 = rf.findNestedResourceFinder("section1");
        checkEqual(rf1.findFile("fname").c_str(),fname1,"section1 ok");
        checkFalse(rf1.isNull(),"section1 not null ok");
        ResourceFinder rf2 = rf.findNestedResourceFinder("section2");
        checkEqual(rf2.findFile("fname").c_str(),fname2,"section2 ok");
        checkFalse(rf2.isNull(),"section2 not null ok");
        ResourceFinder rf3 = rf.findNestedResourceFinder("section3");
        checkTrue(rf3.isNull(),"section3 null ok");
    }

    void testGetDataHome() {
        report(0,"test getDataHome");
        saveEnvironment("YARP_DATA_HOME");
        saveEnvironment("XDG_DATA_HOME");
        saveEnvironment("HOME");
        Network::setEnvironment("YARP_DATA_HOME","/foo");
        checkEqual(ResourceFinder::getDataHome().c_str(),"/foo","YARP_DATA_HOME noticed");
        Network::unsetEnvironment("YARP_DATA_HOME");
        Network::setEnvironment("XDG_DATA_HOME","/foo");
        ConstString slash = Network::getDirectorySeparator();
        checkEqual(ResourceFinder::getDataHome().c_str(),
                   (ConstString("/foo") + slash + "yarp").c_str(),
                   "XDG_DATA_HOME noticed");
        Network::unsetEnvironment("XDG_DATA_HOME");
#ifdef __linux__
        Network::setEnvironment("HOME","/foo");
        checkEqual(ResourceFinder::getDataHome().c_str(),
                   "/foo/.local/share/yarp",
                   "HOME noticed");
#endif
        restoreEnvironment();
    }

    void testGetConfigHome() {
        report(0,"test getConfigHome");
        saveEnvironment("YARP_CONFIG_HOME");
        saveEnvironment("XDG_CONFIG_HOME");
        saveEnvironment("HOME");
        Network::setEnvironment("YARP_CONFIG_HOME","/foo");
        checkEqual(ResourceFinder::getConfigHome().c_str(),"/foo","YARP_CONFIG_HOME noticed");
        Network::unsetEnvironment("YARP_CONFIG_HOME");
        Network::setEnvironment("XDG_CONFIG_HOME","/foo");
        ConstString slash = Network::getDirectorySeparator();
        checkEqual(ResourceFinder::getConfigHome().c_str(),
                   (ConstString("/foo") + slash + "yarp").c_str(),
                   "XDG_CONFIG_HOME noticed");
        Network::unsetEnvironment("XDG_CONFIG_HOME");
#ifdef __linux__
        Network::setEnvironment("HOME","/foo");
        checkEqual(ResourceFinder::getConfigHome().c_str(),
                   "/foo/.config/yarp",
                   "HOME noticed");
#endif
        restoreEnvironment();
    }

    void testGetDataDirs() {
        report(0,"test getDataDirs");
        saveEnvironment("YARP_DATA_DIRS");
        saveEnvironment("XDG_DATA_DIRS");
        ConstString slash = Network::getDirectorySeparator();
        ConstString colon = Network::getPathSeparator();
        ConstString foobar = ConstString("/foo") + colon + "/bar";
        ConstString yfoo = ConstString("/foo") + slash + "yarp";
        ConstString ybar = ConstString("/bar") + slash + "yarp";
        Network::setEnvironment("YARP_DATA_DIRS",foobar);
        Bottle dirs;
        dirs = ResourceFinder::getDataDirs();
        checkEqual(dirs.size(),2,"YARP_DATA_DIRS parsed as two directories");
        checkEqual(dirs.get(0).asString().c_str(),"/foo","YARP_DATA_DIRS first dir ok");
        checkEqual(dirs.get(1).asString().c_str(),"/bar","YARP_DATA_DIRS second dir ok");

        Network::setEnvironment("YARP_DATA_DIRS","/foo");
        dirs = ResourceFinder::getDataDirs();
        checkEqual(dirs.size(),1,"YARP_DATA_DIRS parsed as one directory");

        Network::unsetEnvironment("YARP_DATA_DIRS");
        Network::setEnvironment("XDG_DATA_DIRS",foobar);
        dirs = ResourceFinder::getDataDirs();
        checkEqual(dirs.size(),2,"XDG_DATA_DIRS gives two directories");
        checkEqual(dirs.get(0).asString().c_str(),yfoo.c_str(),"XDG_DATA_DIRS first dir ok");
        checkEqual(dirs.get(1).asString().c_str(),ybar.c_str(),"XDG_DATA_DIRS second dir ok");

        Network::unsetEnvironment("XDG_DATA_DIRS");
#ifdef __linux__
        dirs = ResourceFinder::getDataDirs();
        checkEqual(dirs.size(),2,"DATA_DIRS default length 2");
        checkEqual(dirs.get(0).asString().c_str(),"/usr/local/share/yarp","DATA_DIRS default element 0 is ok");
        checkEqual(dirs.get(1).asString().c_str(),"/usr/share/yarp","DATA_DIRS default element 1 is ok");
#endif

        restoreEnvironment();
    }

    void testGetConfigDirs() {
        report(0,"test getConfigDirs");
        saveEnvironment("YARP_CONFIG_DIRS");
        saveEnvironment("XDG_CONFIG_DIRS");
        ConstString slash = Network::getDirectorySeparator();
        ConstString colon = Network::getPathSeparator();
        ConstString foobar = ConstString("/foo") + colon + "/bar";
        ConstString yfoo = ConstString("/foo") + slash + "yarp";
        ConstString ybar = ConstString("/bar") + slash + "yarp";
        Network::setEnvironment("YARP_CONFIG_DIRS",foobar);
        Bottle dirs;
        dirs = ResourceFinder::getConfigDirs();
        checkEqual(dirs.size(),2,"YARP_CONFIG_DIRS parsed as two directories");
        checkEqual(dirs.get(0).asString().c_str(),"/foo","YARP_CONFIG_DIRS first dir ok");
        checkEqual(dirs.get(1).asString().c_str(),"/bar","YARP_CONFIG_DIRS second dir ok");

        Network::unsetEnvironment("YARP_CONFIG_DIRS");
        Network::setEnvironment("XDG_CONFIG_DIRS",foobar);
        dirs = ResourceFinder::getConfigDirs();
        checkEqual(dirs.size(),2,"XDG_CONFIG_DIRS gives two directories");
        checkEqual(dirs.get(0).asString().c_str(),yfoo.c_str(),"XDG_CONFIG_DIRS first dir ok");
        checkEqual(dirs.get(1).asString().c_str(),ybar.c_str(),"XDG_CONFIG_DIRS second dir ok");

        Network::unsetEnvironment("XDG_CONFIG_DIRS");
#ifdef __linux__
        dirs = ResourceFinder::getConfigDirs();
        checkEqual(dirs.size(),1,"CONFIG_DIRS default length 1");
        checkEqual(dirs.get(0).asString().c_str(),"/etc/yarp","CONFIG_DIRS default is ok");
#endif

        restoreEnvironment();
    }

    virtual void runTests() {
        testBasics();
        testCommandLineArgs();
        testContext();
        testSubGroup();
        testGetDataHome();
        testGetConfigHome();
        testGetDataDirs();
        testGetConfigDirs();
    }
};

static ResourceFinderTest theResourceFinderTest;

UnitTest& getResourceFinderTest() {
    return theResourceFinderTest;
}


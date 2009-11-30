// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/ResourceFinder.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/impl/UnitTest.h>

using namespace yarp::os;
using namespace yarp::os::impl;

// does ACE require new c++ header files or not?
#if ACE_HAS_STANDARD_CPP_LIBRARY
#include <fstream>
using namespace std;
#else
#include <fstream.h>
#endif

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
            ofstream fout0(fname0);
            fout0 << "style capability" << endl;
            fout0 << "capability_directory \".\"" << endl;
            fout0 << "default_capability \".\"" << endl;
            fout0.close();

            ofstream fout1(fname1);
            fout1 << "alt " << fname2 << endl;
            fout1.close();

            ofstream fout2(fname2);
            fout2 << "x 14" << endl;
            fout2.close();

            const char *argv[] = { "ignore", 
                                   "--policy", "_yarp_regression_test",
                                   "--_yarp_regression_test", ".",
                                   "--from", fname1, 
                                   "--verbose", "0",
                                   NULL };
            int argc = 9;

            rf.configure("",argc,(char **)argv);
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
            ofstream fout0(fname0);
            fout0 << "style capability" << endl;
            fout0 << "capability_directory \".\"" << endl;
            fout0 << "default_capability \".\"" << endl;
            fout0.close();

            ofstream fout1(fname1);
            fout1 << "x 14" << endl;
            fout1.close();

            const char *argv[] = { "ignore", 
                                   "--policy", "_yarp_regression_test",
                                   "--_yarp_regression_test", ".",
                                   "--from", fname1, 
                                   "--verbose", "0",
                                   NULL };
            int argc = 9;

            ResourceFinder rf1;
            rf1.configure("",argc,(char **)argv);
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
            rf2.configure("",argc2,(char **)argv2);
            checkEqual(rf2.find("y").asInt(),30,"found y");
            checkEqual(rf2.find("x").asInt(),20,"override x");
        }

    }

    virtual void runTests() {
        testBasics();
        testCommandLineArgs();
    }
};

static ResourceFinderTest theResourceFinderTest;

UnitTest& getResourceFinderTest() {
    return theResourceFinderTest;
}


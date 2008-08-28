// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/ResourceFinder.h>
#include <yarp/String.h>
#include <yarp/UnitTest.h>

using namespace yarp::os;

// does ACE require new c++ header files or not?
#if ACE_HAS_STANDARD_CPP_LIBRARY
#include <fstream>
using namespace std;
#else
#include <fstream.h>
#endif

class ResourceFinderTest : public yarp::UnitTest {
public:
    virtual yarp::String getName() { return "ResourceFinderTest"; }

    void testBasics() {
        report(0,"testing the basics of RF...");
        ResourceFinder rf;

        const char *fname0 = "_yarp_regression_test_rf0.txt";
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
                                   "--policy", fname0,
                                   "--from", fname1, 
                                   "--verbose", "0",
                                   NULL };
            int argc = 7;

            rf.configure("",argc,(char **)argv);
            ConstString alt = rf.findFile("alt");
            checkTrue(alt!="","found ini file");
        }
        
    }

    virtual void runTests() {
        testBasics();
    }
};

static ResourceFinderTest theResourceFinderTest;

yarp::UnitTest& getResourceFinderTest() {
    return theResourceFinderTest;
}


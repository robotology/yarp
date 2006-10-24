// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/sig/Vector.h>
#include <yarp/BufferedConnectionWriter.h>
#include <yarp/os/Bottle.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;

class VectorTest : public UnitTest {
public:
    virtual String getName() { return "VectorTest"; }

    void checkFormat() {
        report(0,"check vector format conforms to network standard...");
        Vector v(4,0.0);
        {
            for (unsigned int i=0; i<v.size(); i++ ){
                v[i] = i;
            }
        }
        BufferedConnectionWriter writer;
        v.write(writer);
        String s = writer.toString();
        Bottle bot;
        bot.fromBinary(s.c_str(),s.length());
        checkEqual(bot.size(),v.size(),"size matches");
        {
            for (int i=0; i<bot.size(); i++) {
                checkTrue(bot.get(i).asDouble()>i-0.25,"bounded below");
                checkTrue(bot.get(i).asDouble()<i+0.25,"bounded above");
            }
        }
    }

    void checkCopy() {
        report(0,"check vectors copy works...");
        Vector v(4,7.0);
        Vector v2;
        v2 = v;
        checkEqual(v.size(),v2.size(),"size matches");
    }

    virtual void runTests() {
        checkFormat();
        checkCopy();
    }
};

static VectorTest theVectorTest;

UnitTest& getVectorTest() {
    return theVectorTest;
}

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/impl/PortCommand.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/impl/StreamConnectionReader.h>

#include <yarp/os/impl/UnitTest.h>

using namespace yarp::os;
using namespace yarp::os::impl;

class PortCommandTest : public UnitTest {
public:
    virtual String getName() { return "PortCommandTest"; }

    void testTextWriting() {
        report(0,"testing text-mode writing...");

        PortCommand cmd1('d',"");;
        BufferedConnectionWriter bw(true);
        cmd1.write(bw);
        checkEqual(humanize(bw.toString()),"d\\r\\n","basic data command");
    
        PortCommand cmd2('\0',"/bozo");;
        BufferedConnectionWriter bw2(true);
        cmd2.write(bw2);
        checkEqual(humanize(bw2.toString()),"/bozo\\r\\n","connect command");
    }

    void testTextReading() {
        report(0,"testing text-mode reading...");

        PortCommand cmd;
        StringInputStream sis;
        StreamConnectionReader br;
        sis.add("d\r\n");
        Route route;
        br.reset(sis,NULL,route,sis.toString().length(),true);
        cmd.read(br);
        checkEqual('d',cmd.getKey(),"basic data command");
    }

    virtual void runTests() {
        testTextWriting();
        testTextReading();
    }
};

static PortCommandTest thePortCommandTest;

UnitTest& getPortCommandTest() {
    return thePortCommandTest;
}


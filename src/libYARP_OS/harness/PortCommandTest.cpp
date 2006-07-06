// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/PortCommand.h>
#include <yarp/BufferedConnectionWriter.h>
#include <yarp/StringInputStream.h>
#include <yarp/StreamConnectionReader.h>

#include "TestList.h"

using namespace yarp;

class PortCommandTest : public UnitTest {
public:
    virtual String getName() { return "PortCommandTest"; }

    void testTextWriting() {
        report(0,"testing text-mode writing...");

        PortCommand cmd1('d',"");;
        BufferedConnectionWriter bw(true);
        cmd1.writeBlock(bw);
        checkEqual(humanize(bw.toString()),"d\\r\\n","basic data command");
    
        PortCommand cmd2('\0',"/bozo");;
        BufferedConnectionWriter bw2(true);
        cmd2.writeBlock(bw2);
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
        cmd.readBlock(br);
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


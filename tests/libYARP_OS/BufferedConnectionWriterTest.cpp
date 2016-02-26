/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/DummyConnector.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/StringOutputStream.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/UnitTest.h>
#include <yarp/sig/Image.h>

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::sig;

typedef PortablePair<PortablePair<PortablePair<Bottle, ImageOf<PixelRgb> >, 
                                  PortablePair<ImageOf<PixelRgb>, Stamp> >, 
                     Bottle> Monster;

class BufferedConnectionWriterTest : public UnitTest {
public:
    virtual String getName() { return "BufferedConnectionWriterTest"; }

    void testWrite() {
        report(0,"testing writing...");
        StringOutputStream sos;    
        BufferedConnectionWriter bbr;
        bbr.reset(true);
        bbr.appendLine("Hello");
        bbr.appendLine("Greetings");
        bbr.write(sos);
        checkEqual(sos.toString(),"Hello\r\nGreetings\r\n","two line writes");
    }

    void testRestart() {
        report(0,"test restarting writer without reallocating memory...");

        size_t pool_sizes[] = {BUFFERED_CONNECTION_INITIAL_POOL_SIZE, 13, 7, 3, 1};
        for (size_t i=0; i<sizeof(pool_sizes)/sizeof(size_t); i++) {
            StringOutputStream sos;
            // first we test a message with a few short strings
            BufferedConnectionWriter bbr;
            report(0,ConstString("pool size of ") + Bottle::toString(pool_sizes[i]) + " begins");
            bbr.setInitialPoolSize(pool_sizes[i]);
            bbr.reset(false);
            ConstString msg1("Hello");
            ConstString msg2("Greetings");
            heapMonitorBegin();
            bbr.appendLine(msg1);
            bbr.appendLine(msg2);
            int ops = heapMonitorEnd();
            bbr.write(sos);
            checkEqual(sos.toString(),"Hello\r\nGreetings\r\n","two line writes");
            if (heapMonitorSupported()) {
                checkTrue(ops>0,"memory allocation happened");
            }
            sos.reset();
            heapMonitorBegin(false);
            bbr.restart();
            bbr.appendLine(msg1);
            bbr.appendLine(msg2);
            heapMonitorEnd();
            bbr.write(sos);
            checkEqual(sos.toString(),"Hello\r\nGreetings\r\n","two line writes dup");

            // Make sure we survive a small change in message
            bbr.restart();
            sos.reset();
            bbr.appendLine("Space Monkeys");
            bbr.appendLine("Attack");
            bbr.write(sos);
            checkEqual(sos.toString(),"Space Monkeys\r\nAttack\r\n","alternate text");

            // And again, a bigger change this time
            String test(2048,'x');
            bbr.restart();
            sos.reset();
            bbr.appendLine(test);
            bbr.appendLine(test);
            bbr.appendLine(test);
            bbr.write(sos);
            ConstString result = sos.toString();
            ConstString expect = test + "\r\n" + test + "\r\n" + test + "\r\n";
            checkTrue(result==expect,"long text");
            sos.reset();
            heapMonitorBegin(false);
            bbr.restart();
            bbr.appendLine(test);
            bbr.appendLine(test);
            bbr.appendLine(test);
            heapMonitorEnd();
            bbr.write(sos);
            result = sos.toString();
            checkTrue(result==expect,"long text, take 2");

            // Try the image class
            ImageOf<PixelRgb> img1, img2;
            img1.resize(320,240);
            img1.zero();
            img1.pixel(10,5).r = 41;
            bbr.restart();
            img1.write(bbr);
            img1.pixel(10,5).r = 42; // sneak change to external buffer to make sure it was not copied
            bbr.write(img2);
            img1.pixel(10,5).r = 43; // now modify original
            checkTrue(img2.width()==img1.width() && img2.height()==img1.height(), "image size matches");
            checkEqual(img2.pixel(10,5).r, 42, "pixel behavior is correct");
            img2.resize(1,1);
            // Now resend, checking that no memory is allocated
            bbr.restart();
            heapMonitorBegin(false);
            img1.write(bbr);
            heapMonitorEnd();
            bbr.write(img2);
            checkTrue(img2.width()==img1.width() && img2.height()==img1.height(), "image size still matches");

            // Now send something completely different
            Monster m1, m2;
            m1.body.fromString("hello (1 (2 (3))) {1 2 3} [done]");
            m1.head.head.body.resize(41,12);
            m1.head.body.head.resize(17,63);
            bbr.restart();
            m1.write(bbr);
            bbr.write(m2);
            checkEqual(m2.body.get(0).asString(),"hello","tail matches");
            // Now resend, checking that no memory is allocated
            m2 = Monster();
            bbr.restart();
            heapMonitorBegin(false);
            m1.write(bbr);
            heapMonitorEnd();
            bbr.write(m2);
            checkEqual(m2.body.get(0).asString(),"hello","tail still matches");

            // Now send something completely different
            Stamp stamp1(42, 1.23), stamp2;
            bbr.restart();
            stamp1.write(bbr);
            bbr.write(stamp2);
            checkEqual(stamp1.getCount(),stamp2.getCount(),"stamp matches");
            // Now resend, checking that no memory is allocated
            stamp2 = Stamp();
            bbr.restart();
            heapMonitorBegin(false);
            stamp1.write(bbr);
            heapMonitorEnd();
            bbr.write(stamp2);
            checkEqual(stamp1.getCount(),stamp2.getCount(),"stamp still matches");

            report(0,ConstString("pool size of ") + Bottle::toString(pool_sizes[i]) + " had " + Bottle::toString(bbr.bufferCount()) + " buffers");
        }
    }

    virtual void runTests() {
        testWrite();
        testRestart();
    }
};

static BufferedConnectionWriterTest theBufferedConnectionWriterTest;

UnitTest& getBufferedConnectionWriterTest() {
    return theBufferedConnectionWriterTest;
}


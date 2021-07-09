/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/BufferedConnectionWriter.h>

#include <yarp/os/DummyConnector.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/StringOutputStream.h>
#include <yarp/sig/Image.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::sig;

typedef PortablePair<PortablePair<PortablePair<Bottle, ImageOf<PixelRgb> >,
                                  PortablePair<ImageOf<PixelRgb>, Stamp> >,
                     Bottle> Monster;

TEST_CASE("os::impl::BufferedConnectionWriterTest", "[yarp::os][yarp::os::impl]")
{

    SECTION("testing writing")
    {
        StringOutputStream sos;
        BufferedConnectionWriter bbr;
        bbr.reset(true);
        bbr.appendLine("Hello");
        bbr.appendLine("Greetings");
        bbr.write(sos);
        CHECK(sos.toString() == "Hello\r\nGreetings\r\n"); // "two line writes"
    }

    SECTION("test restarting writer without reallocating memory...")
    {

        size_t pool_sizes[] = {BUFFERED_CONNECTION_INITIAL_POOL_SIZE, 13, 7, 3, 1};
        for (size_t i=0; i<sizeof(pool_sizes)/sizeof(size_t); i++) {
            StringOutputStream sos;
            // first we test a message with a few short strings
            BufferedConnectionWriter bbr;
            INFO("pool size of " << Bottle::toString(pool_sizes[i]) << " begins");
            bbr.setInitialPoolSize(pool_sizes[i]);
            bbr.reset(false);
            std::string msg1("Hello");
            std::string msg2("Greetings");
            bbr.appendLine(msg1);
            bbr.appendLine(msg2);
            bbr.write(sos);
            CHECK(sos.toString() == "Hello\r\nGreetings\r\n"); // two line writes;
            sos.reset();
            bbr.restart();
            bbr.appendLine(msg1);
            bbr.appendLine(msg2);
            bbr.write(sos);
            CHECK(sos.toString() == "Hello\r\nGreetings\r\n"); // two line writes dup;

            // Make sure we survive a small change in message
            bbr.restart();
            sos.reset();
            bbr.appendLine("Space Monkeys");
            bbr.appendLine("Attack");
            bbr.write(sos);
            CHECK(sos.toString() == "Space Monkeys\r\nAttack\r\n"); // alternate text;

            // And again, a bigger change this time
            std::string test(2048, 'x');
            bbr.restart();
            sos.reset();
            bbr.appendLine(test);
            bbr.appendLine(test);
            bbr.appendLine(test);
            bbr.write(sos);
            std::string result = sos.toString();
            std::string expect = test + "\r\n" + test + "\r\n" + test + "\r\n";
            CHECK(result == expect); // long text
            sos.reset();
            bbr.restart();
            bbr.appendLine(test);
            bbr.appendLine(test);
            bbr.appendLine(test);
            bbr.write(sos);
            result = sos.toString();
            CHECK(result == expect); // long text, take 2

            // Try the image class
            ImageOf<PixelRgb> img1, img2;
            img1.resize(320, 240);
            img1.zero();
            img1.pixel(10, 5).r = 41;
            bbr.restart();
            img1.write(bbr);
            img1.pixel(10, 5).r = 42; // sneak change to external buffer to make sure it was not copied
            bbr.write(img2);
            img1.pixel(10, 5).r = 43; // now modify original
            CHECK(img2.width() == img1.width()); // image width matches
            CHECK(img2.height() == img1.height()); // image width matches
            CHECK(img2.pixel(10, 5).r == 42); // pixel behavior is correct
            img2.resize(1, 1);
            // Now resend, checking that no memory is allocated
            bbr.restart();
            img1.write(bbr);
            bbr.write(img2);
            CHECK(img2.width() == img1.width()); // image width still matches
            CHECK(img2.height() == img1.height()); // image height still matches

            // Now send something completely different
            Monster m1, m2;
            m1.body.fromString("hello (1 (2 (3))) {1 2 3} [done]");
            m1.head.head.body.resize(41, 12);
            m1.head.body.head.resize(17, 63);
            bbr.restart();
            m1.write(bbr);
            bbr.write(m2);
            CHECK(m2.body.get(0).asString() == "hello"); // tail matches
            // Now resend, checking that no memory is allocated
            m2 = Monster();
            bbr.restart();
            m1.write(bbr);
            bbr.write(m2);
            CHECK(m2.body.get(0).asString() == "hello"); // tail still matches

            // Now send something completely different
            Stamp stamp1(42, 1.23), stamp2;
            bbr.restart();
            stamp1.write(bbr);
            bbr.write(stamp2);
            CHECK(stamp1.getCount() == stamp2.getCount()); // stamp matches
            // Now resend, checking that no memory is allocated
            stamp2 = Stamp();
            bbr.restart();
            stamp1.write(bbr);
            bbr.write(stamp2);
            CHECK(stamp1.getCount() == stamp2.getCount()); // stamp still matches

            INFO("pool size of " << Bottle::toString(pool_sizes[i]) << " had " << Bottle::toString(bbr.bufferCount()) << " buffers");
        }
    }
}

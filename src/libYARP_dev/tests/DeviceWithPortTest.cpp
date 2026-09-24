/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Image.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;


/**
 * @ingroup dev_impl_media
 *
 * A fake device for testing closure after a prepare of a closed port.
 */
class BrokenDevice :
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread
{
    static constexpr double period = 0.03;

public:
    /**
     * Constructor.
     */
    BrokenDevice() :
            PeriodicThread(period)
    {
    }

    bool close() override
    {
        pImg.close();
        PeriodicThread::stop();
        return true;

    }

    bool open(yarp::os::Searchable& /*config*/) override
    {
        return PeriodicThread::start();
    }

    //RateThread
    bool threadInit() override
    {
        return true;
   }

   void threadRelease() override
   {
   }

   void run() override
    {
        img = &pImg.prepare();
        img->resize(10, 10);
        pImg.write();
    }


private:
    yarp::sig::ImageOf<yarp::sig::PixelRgb>* img {nullptr};
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb>> pImg;

};

TEST_CASE("dev::DeviceWithPortTest", "[yarp::dev]")
{
    Network::setLocalMode(true);

    yarp::dev::Drivers::factory().add(new yarp::dev::DriverCreatorOf<BrokenDevice>("brokenDevice",
                                                                                  "brokenDevice",
                                                                                  "BrokenDevice"));

   SECTION("testing the deadlock when you close a device(PeriodicThread) after the prepare of a closed port")
    {
        yarp::dev::PolyDriver p;
        Property prop;
        prop.put("device", "brokenDevice");
        CHECK(p.open(prop)); // Opening the broken_device
        CHECK(p.close()); // Closing the broken_device
    }

    Network::setLocalMode(false);
}

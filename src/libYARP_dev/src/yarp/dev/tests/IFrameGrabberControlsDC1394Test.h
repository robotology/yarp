/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IFRAMEGRABBERCONTROLSDC1394TEST_H
#define IFRAMEGRABBERCONTROLSDC1394TEST_H

#include <array>
#include <yarp/dev/IFrameGrabberControlsDC1394.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_IFrameGrabberControlsDC1394_test_1(IFrameGrabberControlsDC1394* ictl)
    {
        REQUIRE(ictl);

        // Check getBytesPerPacketDC1394()
        {
            unsigned int bpp = 0;
            CHECK(ictl->getBytesPerPacketDC1394(bpp));
        }

        // Check getColorCodingDC1394()
        {
            unsigned int val = 0;
            CHECK(ictl->getColorCodingDC1394(val));
        }

        // Check getColorCodingMaskDC1394()
        {
            unsigned int val = 0;
            CHECK(ictl->getColorCodingMaskDC1394(0,val));
        }
    }
}

#endif

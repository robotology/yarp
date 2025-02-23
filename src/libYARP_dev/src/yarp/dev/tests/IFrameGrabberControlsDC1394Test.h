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

        // Check getFormat7MaxWindowDC1394()
        {
            unsigned int xdim = 0;
            unsigned int ydim = 0;
            unsigned int xstep = 0;
            unsigned int ystep = 0;
            unsigned int xoffstep = 0;
            unsigned int yoffstep = 0;
            CHECK(ictl->getFormat7MaxWindowDC1394(xdim,ydim,xstep,ystep,xoffstep,yoffstep));
        }

        // Check getFormat7MaxWindowDC1394()
        {
            unsigned int xdim = 0;
            unsigned int ydim = 0;
            int x0 = 0;
            int y0 = 0;
            CHECK(ictl->getFormat7WindowDC1394(xdim,ydim,x0,y0));
        }

        // Check getFPSDC1394()
        {
            unsigned int val = 0;
            CHECK(ictl->getFPSDC1394(val));
        }

        // Check getFPSMaskDC1394()
        {
            unsigned int val = 0;
            CHECK(ictl->getFPSMaskDC1394(val));
        }

        // Check getISOSpeedDC1394()
        {
            unsigned int val = 0;
            CHECK(ictl->getISOSpeedDC1394(val));
        }

        // Check getOperationModeDC1394()
        {
            bool val = false;
            CHECK(ictl->getOperationModeDC1394(val));
        }

        // Check getTransmissionDC1394()
        {
            bool val = false;
            CHECK(ictl->getTransmissionDC1394(val));
        }

        // Check getVideoModeDC1394()
        {
            unsigned int val = 0;
            CHECK(ictl->getVideoModeDC1394(val));
        }

        // Check getVideoModeDC1394()
        {
            unsigned int val = 0;
            CHECK(ictl->getVideoModeMaskDC1394(val));
        }

        // Check setBroadcastDC1394()
        {
            bool val = true;
            CHECK(ictl->setBroadcastDC1394(val));
        }

        // Check setBytesPerPacketDC1394()
        {
            unsigned int bpp = 0;
            CHECK(ictl->setBytesPerPacketDC1394(bpp));
        }

        // Check setCaptureDC1394()
        {
            bool bon = true;
            CHECK(ictl->setCaptureDC1394(bon));
        }

        // Check setColorCodingDC1394()
        {
            int coding = 0;
            CHECK(ictl->setColorCodingDC1394(coding));
        }

        // Check setDefaultsDC1394()
        {
            CHECK(ictl->setDefaultsDC1394());
        }

        // Check setFormat7MaxWindowDC1394()
        {
            unsigned int xdim = 0;
            unsigned int ydim = 0;
            int x0 = 0;
            int y0 = 0;
            CHECK(ictl->setFormat7WindowDC1394(xdim,ydim,x0,y0));
        }

        // Check setFPSDC1394()
        {
            unsigned int val = 0;
            CHECK(ictl->setFPSDC1394(val));
        }

        // Check setISOSpeedDC1394()
        {
            unsigned int val = 0;
            CHECK(ictl->setISOSpeedDC1394(val));
        }

        // Check setOperationModeDC1394()
        {
            bool val = true;
            CHECK(ictl->setOperationModeDC1394(val));
        }

        // Check setPowerDC1394()
        {
            bool onoff = true;
            CHECK(ictl->setPowerDC1394(onoff));
        }

        // Check setResetDC1394()
        {
            CHECK(ictl->setResetDC1394());
        }

        // Check setTransmissionDC1394()
        {
            bool onoff = true;
            CHECK(ictl->setTransmissionDC1394(onoff));
        }

        // Check setVideoModeDC1394()
        {
            int mode = 0;
            CHECK(ictl->setVideoModeDC1394(mode));
        }
    }
}

#endif

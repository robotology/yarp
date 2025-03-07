/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IRGBDSENSORTEST_H
#define IRGBDSENSORTEST_H

#include <yarp/dev/IRGBDSensor.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iRGBDSensor_test_1(IRGBDSensor* irgbd)
    {
        REQUIRE(irgbd != nullptr);

        {
            double accuracy = 0;
            ReturnValue r;
            r = irgbd->getDepthAccuracy(accuracy);
            CHECK(r);
            CHECK(accuracy != 5.0);
            r = irgbd->setDepthAccuracy(5.0);
            CHECK(r);
            r = irgbd->getDepthAccuracy(accuracy);
            CHECK(r);
            CHECK(accuracy == 5.0);
        }
        {
            double near = 0;
            double far = 0;
            ReturnValue r1 = irgbd->getDepthClipPlanes(near,far);
            CHECK(r1);
            CHECK(near != 1.0);
            CHECK(far != 2.0 );
            ReturnValue r2 = irgbd->setDepthClipPlanes(1.0,2.0);
            CHECK(r2);
            ReturnValue r3 = irgbd->getDepthClipPlanes(near,far);
            CHECK(r3);
            CHECK(near == 1.0);
            CHECK(far == 2.0 );
        }
        {
            double hfov = 0;
            double vfov = 0;
            ReturnValue r1 = irgbd->getDepthFOV(hfov,vfov);
            CHECK(r1);
            CHECK(hfov != 3.0);
            CHECK(vfov != 4.0);
            ReturnValue r2 = irgbd->setDepthFOV(3.0,4.0);
            CHECK(r2);
            ReturnValue r3 = irgbd->getDepthFOV(hfov,vfov);
            CHECK(r3);
            CHECK(hfov == 3.0);
            CHECK(vfov == 4.0 );
        }
        {
            int h = 0;
            h = irgbd->getDepthHeight();
            CHECK(h > 0);
        }
        {
            yarp::os::Stamp stamp;
            yarp::sig::ImageOf<yarp::sig::PixelFloat> img;
            ReturnValue r1;
            ReturnValue r2;
            r1 = irgbd->getDepthImage(img, nullptr);
            r2 = irgbd->getDepthImage(img, &stamp);
            CHECK(r1);
            CHECK(r2);
        }
        {
            yarp::os::Property intrinsic;
            ReturnValue r;
            r = irgbd->getDepthIntrinsicParam(intrinsic);
            CHECK(r);
        }
        {
            bool mir=false;
            ReturnValue r;
            r = irgbd->getDepthMirroring(mir);
            CHECK(r);
            CHECK(mir==false);
            r = irgbd->setDepthMirroring(true);
            CHECK(r);
            r = irgbd->getDepthMirroring(mir);
            CHECK(r);
            CHECK(mir==true);
        }
        {
            int w;
            w = irgbd->getDepthWidth();
            CHECK(w>0);
        }
        {
            int w = 0;
            int h = 0;
            ReturnValue r;
            r = irgbd->getDepthResolution(w, h);
            CHECK(r);
            CHECK(w != 10);
            CHECK(h != 11);
            r = irgbd->setDepthResolution(10, 11);
            CHECK(r);
            r = irgbd->getDepthResolution(w, h);
            CHECK(r);
            CHECK(w == 10);
            CHECK(h == 11);
        }
        {
            ReturnValue r;
            yarp::sig::Matrix matr;
            r = irgbd->getExtrinsicParam(matr);
            CHECK(r);
        }
        {
            ReturnValue r1;
            ReturnValue r2;
            ReturnValue r3;
            yarp::sig::Matrix matr;
            yarp::os::Stamp rgbstamp;
            yarp::os::Stamp depthstamp;
            yarp::sig::ImageOf<yarp::sig::PixelFloat> depth;
            yarp::sig::FlexImage rgb;
            r1 = irgbd->getImages(rgb, depth);
            r2 = irgbd->getImages(rgb,depth,&rgbstamp);
            r3 = irgbd->getImages(rgb,depth,&rgbstamp,&depthstamp);
            CHECK(r1);
            CHECK(r2);
            CHECK(r3);
        }
        {
            ReturnValue r1;
            ReturnValue r2;
            std::string mesg;
            yarp::os::Stamp stamp;
            r1 = irgbd->getLastErrorMsg(mesg);
            r2 = irgbd->getLastErrorMsg(mesg,&stamp);
            CHECK(r1);
            CHECK(r2);
        }
        {
            double hfov = 0;
            double vfov = 0;
            ReturnValue r1 = irgbd->getRgbFOV(hfov,vfov);
            CHECK(r1);
            CHECK(hfov != 31.0);
            CHECK(vfov != 41.0);
            ReturnValue r2 = irgbd->setRgbFOV(31.0,41.0);
            CHECK(r2);
            ReturnValue r3 = irgbd->getRgbFOV(hfov,vfov);
            CHECK(r3);
            CHECK(hfov == 31.0);
            CHECK(vfov == 41.0);
        }
        {
            double h = 0;
            h = irgbd->getRgbHeight();
            CHECK(h>0);
        }
        {
            ReturnValue r1;
            ReturnValue r2;
            yarp::os::Stamp stamp;
            yarp::sig::FlexImage rgb;
            r1 = irgbd->getRgbImage(rgb);
            r2 = irgbd->getRgbImage(rgb,&stamp);
            CHECK(r1);
            CHECK(r2);
        }
        {
            ReturnValue r;
            yarp::os::Property intrinsic;
            r = irgbd->getRgbIntrinsicParam(intrinsic);
            CHECK(r);
        }
        {
            bool mir=false;
            ReturnValue r;
            r = irgbd->getRgbMirroring(mir);
            CHECK(r);
            CHECK(mir==false);
            r = irgbd->setRgbMirroring(true);
            CHECK(r);
            r = irgbd->getRgbMirroring(mir);
            CHECK(r);
            CHECK(mir==true);
        }
        {
            int w = 0;
            int h = 0;
            ReturnValue r;
            r = irgbd->getRgbResolution(w, h);
            CHECK(r);
            CHECK(w != 30);
            CHECK(h != 31);
            r = irgbd->setRgbResolution(30, 31);
            CHECK(r);
            r = irgbd->getRgbResolution(w, h);
            CHECK(r);
            CHECK(w == 30);
            CHECK(h == 31);
        }
        {
            ReturnValue r;
            std::vector<yarp::dev::CameraConfig> cfg;
            r = irgbd->getRgbSupportedConfigurations(cfg);
            CHECK(r);
            CHECK(cfg.size()>0);
        }
        {
            ReturnValue r;
            int w = irgbd->getRgbWidth();
            CHECK(w>0);
        }
        {
            ReturnValue r;
            yarp::dev::IRGBDSensor::RGBDSensor_status status;
            r = irgbd->getSensorStatus(status);
            CHECK(r);
        }
    }
}

#endif

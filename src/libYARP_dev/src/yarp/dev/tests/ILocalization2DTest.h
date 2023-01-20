/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ILOCALIZATION2DTEST_H
#define ILOCALIZATION2DTEST_H

#include <yarp/dev/ILocalization2D.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iLocalization2D_test_1(yarp::dev::Nav2D::ILocalization2D* iloc)
    {
        REQUIRE(iloc != nullptr);

        bool b;

        yarp::dev::Nav2D::Map2DLocation ll;
        yarp::dev::OdometryData odom;
        yarp::dev::Nav2D::LocalizationStatusEnum status;
        std::vector<yarp::dev::Nav2D::Map2DLocation> poses;
        b = iloc->getCurrentPosition(ll);         CHECK(b);
        b = iloc->startLocalizationService();     CHECK(b);
        b = iloc->stopLocalizationService();      CHECK(b);
        b = iloc->setInitialPose(ll);             CHECK(b);
        b = iloc->getEstimatedOdometry(odom);     CHECK(b);
        b = iloc->getEstimatedPoses(poses);       CHECK(b);
        b = iloc->getLocalizationStatus(status);  CHECK(b);
    }
}

#endif

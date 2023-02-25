/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IFRAMETRANSFORMSTORAGETEST_H
#define IFRAMETRANSFORMSTORAGETEST_H

#define _USE_MATH_DEFINES
#include <math.h>

#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>
#include <yarp/math/Quaternion.h>
#include <yarp/dev/IFrameTransformStorage.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/os/LogStream.h>

#include <yarp/math/Math.h>
#include <yarp/math/FrameTransform.h>

#include <cmath>
#include <vector>

#include <catch2/catch_amalgamated.hpp>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::math;

namespace yarp::dev::tests
{
    inline bool isEqual(const yarp::sig::Vector& v1, const yarp::sig::Vector& v2, double precision)
    {
        if (v1.size() != v2.size())
        {
            return false;
        }

        for (size_t i = 0; i < v1.size(); i++)
        {
            double check = fabs(v1[i] - v2[i]);
            if (check > precision)
            {
                return false;
            }
        }
        return true;
    }

    inline bool isEqual(const yarp::math::Quaternion& q1, const yarp::math::Quaternion& q2, double precision)
    {
        yarp::sig::Vector v1 = q1.toVector();
        yarp::sig::Vector v2 = q2.toVector();

        for (size_t i = 0; i < v1.size(); i++)
        {
            double check = fabs(v1[i] - v2[i]);
            if (check > precision)
            {
                return false;
            }
        }
        return true;
    }

    inline bool isEqual(const yarp::sig::Matrix& m1, const yarp::sig::Matrix& m2, double precision)
    {
        if (m1.cols() != m2.cols() || m1.rows() != m2.rows())
        {
            return false;
        }

        for (size_t i = 0; i < m1.rows(); i++)
        {
            if (!isEqual(m1.getRow(i), m2.getRow(i), precision))
            {
                return false;
            }
        }
        return true;
    }

    inline void exec_frameTransformStorage_test_1(IFrameTransformStorageSet* itfSet,
                                                  IFrameTransformStorageGet* itfGet,
                                                  IFrameTransformStorageUtils* itfUtils)
    {
        if (itfSet)
        {
            CHECK(itfSet->clearAll());
        }
        if (itfGet)
        {
            std::vector<yarp::math::FrameTransform> transforms;
            CHECK(itfGet->getTransforms(transforms));
        }
        if (itfUtils)
        {
            size_t siz;
            CHECK(itfUtils->size(siz));
        }
    }

}

#endif

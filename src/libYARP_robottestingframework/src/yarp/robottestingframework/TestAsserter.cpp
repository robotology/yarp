/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/robottestingframework/TestAsserter.h>
#include <yarp/sig/Vector.h>
#include <robottestingframework/TestAssert.h>
#include <cmath>


class yarp::robottestingframework::TestAsserter::Private
{
};

yarp::robottestingframework::TestAsserter::TestAsserter() :
        mPriv(new Private)
{
}

yarp::robottestingframework::TestAsserter::~TestAsserter()
{
    delete mPriv;
}

bool yarp::robottestingframework::TestAsserter::isApproxEqual(const double *left,
                                            const double *right,
                                            const double *thresholds,
                                            int length)
{
    return isApproxEqual(left, right, thresholds, thresholds, length);
}

bool yarp::robottestingframework::TestAsserter::isApproxEqual(const double *left,
                                            const double *right,
                                            const double *l_thresholds,
                                            const double *h_thresholds,
                                            int length)
{
    bool reached = true;
    for(int j = 0; j < length; j++)
    {
        if (left[j]<(right[j]-fabs(l_thresholds[j])) || left[j]>(right[j]+fabs(h_thresholds[j]))) {
            reached=false;
        }
    }
    return reached;
}


bool yarp::robottestingframework::TestAsserter::isApproxEqual(const yarp::sig::Vector &left,
                                            const yarp::sig::Vector &right,
                                            const yarp::sig::Vector &thresholds)
{
    if (left.size() != right.size() && right.size() != thresholds.size()) {
        ROBOTTESTINGFRAMEWORK_ASSERT_ERROR("yarp::robottestingframework::TestAsserter::isApproxEqual : vectors must have same size!");
        return false;
    }
    return isApproxEqual(left.data(), right.data(), thresholds.data(), left.size());
}

bool yarp::robottestingframework::TestAsserter::isApproxEqual(double left,
                                            double right,
                                            double l_th,
                                            double h_th)
{

    if (left >= right - fabs(l_th) && left <= right + fabs(h_th)) {
        return true;
    } else {
        return false;
    }
}

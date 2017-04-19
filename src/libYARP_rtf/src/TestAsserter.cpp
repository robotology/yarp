/*
 * Copyright (C) 2015 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Ali Paikan <ali.paikan@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/rtf/TestAsserter.h>
#include <yarp/sig/Vector.h>
#include <rtf/TestAssert.h>
#include <cmath>


class yarp::rtf::TestAsserter::Private
{
};

yarp::rtf::TestAsserter::TestAsserter() :
        mPriv(new Private)
{
}

yarp::rtf::TestAsserter::~TestAsserter()
{
    delete mPriv;
}

bool yarp::rtf::TestAsserter::isApproxEqual(const double *left,
                                            const double *right,
                                            const double *thresholds,
                                            int lenght)
{
    return isApproxEqual(left, right, thresholds, thresholds, lenght);
}

bool yarp::rtf::TestAsserter::isApproxEqual(const double *left,
                                            const double *right,
                                            const double *l_thresholds,
                                            const double *h_thresholds,
                                            int lenght)
{
    bool reached = true;
    for(int j = 0; j < lenght; j++)
    {
        if (left[j]<(right[j]-fabs(l_thresholds[j])) || left[j]>(right[j]+fabs(h_thresholds[j]))) {
            reached=false;
        }
    }
    return reached;
}


bool yarp::rtf::TestAsserter::isApproxEqual(const yarp::sig::Vector &left,
                                            const yarp::sig::Vector &right,
                                            const yarp::sig::Vector &thresholds)
{
    if (left.size() != right.size() && right.size() != thresholds.size()) {
        RTF_ASSERT_ERROR("yarp::rtf::TestAsserter::isApproxEqual : vectors must have same size!");
        return false;
    }
    return isApproxEqual(left.data(), right.data(), thresholds.data(), left.size());
}

bool yarp::rtf::TestAsserter::isApproxEqual(double left,
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


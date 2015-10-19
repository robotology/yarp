// -*- mode:C++ { } tab-width:4 { } c-basic-offset:4 { } indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <math.h>
#include <rtf/TestAssert.h>
#include <rtf/yarp/YarpTestAsserter.h>

using namespace yarp::sig;
using namespace RTF;
using namespace RTF::YARP;


bool YarpTestAsserter::isApproxEqual(const double *left, const double *right,
                              const double *thresholds, int lenght)
{
    return isApproxEqual(left, right, thresholds, thresholds, lenght);
}

bool YarpTestAsserter::isApproxEqual(const double *left, const double *right,
                              const double *l_thresholds, const double *h_thresholds,
                              int lenght)
{
    bool reached=true;
    for(int j=0; j<lenght; j++)
    {
        if (left[j]<(right[j]-fabs(l_thresholds[j])) || left[j]>(right[j]+fabs(h_thresholds[j])))
            reached=false;
    }
    return reached;
}


bool YarpTestAsserter::isApproxEqual(const Vector &left,
                              const Vector &right,
                              const Vector &thresholds)
{
    if (left.size()!=right.size() && right.size()!=thresholds.size()) {
            RTF_ASSERT_ERROR("YarpTestAsserter::isApproxEqual : vectors must have same size!");
            return false;
    }
    return isApproxEqual(left.data(), right.data(), thresholds.data(), left.size());
}

bool YarpTestAsserter::isApproxEqual(double left, double right, double l_th, double h_th)
{

    if (left>=right-fabs(l_th) && left<=right+fabs(h_th))
        return true;
    else
        return false;
}


// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/math/math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_matrix_double.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_vector_double.h>
#include <gsl/gsl_cblas.h>

gsl_vector_view getView(const yarp::sig::Vector &v)
{
    gsl_vector_view ret=gsl_vector_view_array(const_cast<double *>(v.data()), v.size());
    return ret;
}

gsl_vector_view getView(yarp::sig::Vector &v)
{
    gsl_vector_view ret=gsl_vector_view_array(v.data(),
                                             v.size());
    return ret;
}

using namespace yarp::sig;

Vector yarp::math::operator+(const Vector &a, const Vector &b)
{
    int s=a.size();
    yarp::sig::Vector ret(s);
    for (int k=0; k<s;k++)
        ret[k]=a[k]+b[k];
    return ret;
}
 
Vector yarp::math::operator-(const Vector &a, const Vector &b)
{
    int s=a.size();
    yarp::sig::Vector ret(s);
    for (int k=0; k<s;k++)
        ret[k]=a[k]-b[k];
    return ret;
}

double yarp::math::operator*(const yarp::sig::Vector &a, const yarp::sig::Vector &b)
{
    double ret;
    ret=cblas_ddot(a.size(), a.data(),1, b.data(),1);
    return ret;
}

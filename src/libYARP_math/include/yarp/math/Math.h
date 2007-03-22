// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __YARP_MATH__
#define __YARP_MATH__

#include <yarp/sig/Vector.h>

namespace yarp
{
    namespace math 
    {
        yarp::sig::Vector operator+(const yarp::sig::Vector &a, const yarp::sig::Vector &b);
        yarp::sig::Vector operator-(const yarp::sig::Vector &a, const yarp::sig::Vector &b);
        double operator*(const yarp::sig::Vector &a, const yarp::sig::Vector &b);
    }
}

#if 0
// norm 2.
double norm2(void) const;
double norm2square(void) const;

Vector operator/(double dbl) const;

YVector& operator+=(const Vector &b);
YVector& operator-=(const Vector &b);

Vector& operator+=(double dbl);
Vector& operator-=(double dbl);
Vector operator+(double dbl) const;
Vector operator-(double dbl) const;

Vector& operator*=(double dbl);
Vector& operator/=(double dbl);
#endif
#endif

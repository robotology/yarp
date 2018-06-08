/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/sig/Vec2D.h>
#include <yarp/math/Math.h>
#include <sstream>
#include <cmath>
#include <cstdio>

// network stuff
#include <yarp/os/NetInt32.h>

using namespace yarp::sig;
using namespace yarp::math;

/*
template <typename T>
 yarp::sig::Vec2D<T>  operator * (const yarp::sig::Matrix& lhs, yarp::sig::Vec2D<T> rhs)
{
    yAssert(lhs.rows() == 2 && lhs.cols() == 2);
    T x = rhs.x; T y = rhs.y;
    rhs.x = T(lhs[0][0] * x + lhs[0][1] * y);
    rhs.y = T(lhs[1][0] * x + lhs[1][1] * y);
    return rhs;
}

template <typename T>
yarp::sig::Vec2D<T> operator + (yarp::sig::Vec2D<T> lhs, const yarp::sig::Vec2D<T>& rhs)
{
    lhs += rhs;
    return lhs;
}

template <typename T>
yarp::sig::Vec2D<T> operator - (yarp::sig::Vec2D<T> lhs, const yarp::sig::Vec2D<T>& rhs)
{
    lhs -= rhs;
    return lhs;
}

template <typename T>
yarp::sig::Vec2D<T>& yarp::sig::Vec2D<T>::operator+=(const yarp::sig::Vec2D<T>& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
}

template <typename T>
yarp::sig::Vec2D<T>& yarp::sig::Vec2D<T>::operator-=(const yarp::sig::Vec2D<T>& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    return *this;
}

template <typename T>
yarp::sig::Vec2D<T>& yarp::sig::Vec2D<T>::operator =(const yarp::sig::Vec2D<T>& rhs)
{
    if (this != &rhs)
    {
        x = rhs.x;
        y = rhs.y;
    }
    return *this;
}

template <typename T>
bool yarp::sig::Vec2D<T>::operator ==(const yarp::sig::Vec2D<T>& rhs)
{
    if (this->x == rhs.x &&
        this->y == rhs.y) return true;
    return false;
}

template <typename T>
bool yarp::sig::Vec2D<T>::operator !=(const yarp::sig::Vec2D<T>& rhs)
{
    if (this->x == rhs.x &&
        this->y == rhs.y) return false;
    return true;
}

template yarp::sig::Vec2D<double> YARP_sig_API operator + (yarp::sig::Vec2D<double> lhs, const yarp::sig::Vec2D<double>& rhs);
template yarp::sig::Vec2D<int>    YARP_sig_API operator + (yarp::sig::Vec2D<int> lhs, const yarp::sig::Vec2D<int>& rhs);
template yarp::sig::Vec2D<double> YARP_sig_API operator - (yarp::sig::Vec2D<double> lhs, const yarp::sig::Vec2D<double>& rhs);
template yarp::sig::Vec2D<int>    YARP_sig_API operator - (yarp::sig::Vec2D<int> lhs, const yarp::sig::Vec2D<int>& rhs);
template yarp::sig::Vec2D<double> YARP_sig_API operator * (const yarp::sig::Matrix& lhs, yarp::sig::Vec2D<double> rhs);
template yarp::sig::Vec2D<int>    YARP_sig_API operator * (const yarp::sig::Matrix& lhs, yarp::sig::Vec2D<int> rhs);

template class YARP_sig_API yarp::sig::Vec2D<double>;
template class YARP_sig_API yarp::sig::Vec2D<int>;
*/
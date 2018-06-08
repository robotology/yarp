/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/sig/Vec2D.h>
#include <sstream>
#include <cmath>
#include <cstdio>

// network stuff
#include <yarp/os/NetInt32.h>

using namespace yarp::sig;

YARP_BEGIN_PACK
class Vec2DPortContentHeader
{
public:
    yarp::os::NetInt32 listTag;
    yarp::os::NetInt32 listLen;
    Vec2DPortContentHeader() : listTag(0), listLen(0) {}
};
YARP_END_PACK

namespace yarp {
namespace sig {
template<>
bool Vec2D<double>::read(yarp::os::ConnectionReader& connection)
{
    // auto-convert text mode interaction
    connection.convertTextMode();
    Vec2DPortContentHeader header;
    bool ok = connection.expectBlock((char*)&header, sizeof(header));
    if (!ok) return false;

    if (header.listLen == 2 && header.listTag == (BOTTLE_TAG_LIST | BOTTLE_TAG_FLOAT64))
    {
        this->x = connection.expectFloat64();
        this->y = connection.expectFloat64();
    }
    else
    {
        return false;
    }

    return !connection.isError();
}

template<>
bool Vec2D<int>::read(yarp::os::ConnectionReader& connection)
{
    // auto-convert text mode interaction
    connection.convertTextMode();
    Vec2DPortContentHeader header;
    bool ok = connection.expectBlock((char*)&header, sizeof(header));
    if (!ok) return false;

    if (header.listLen == 2 && header.listTag == (BOTTLE_TAG_LIST | BOTTLE_TAG_INT32))
    {
        this->x = connection.expectInt32();
        this->y = connection.expectInt32();
    }
    else
    {
        return false;
    }

    return !connection.isError();
}

template<>
bool Vec2D<double>::write(yarp::os::ConnectionWriter& connection)
{
    Vec2DPortContentHeader header;

    header.listTag = (BOTTLE_TAG_LIST | BOTTLE_TAG_FLOAT64);
    header.listLen = 2;

    connection.appendBlock((char*)&header, sizeof(header));

    connection.appendFloat64(this->x);
    connection.appendFloat64(this->y);

    connection.convertTextMode();

    return !connection.isError();
}

template<>
bool Vec2D<int>::write(yarp::os::ConnectionWriter& connection)
{
    Vec2DPortContentHeader header;

    header.listTag = (BOTTLE_TAG_LIST | BOTTLE_TAG_INT32);
    header.listLen = 2;

    connection.appendBlock((char*)&header, sizeof(header));

    connection.appendInt32(this->x);
    connection.appendInt32(this->y);

    connection.convertTextMode();

    return !connection.isError();
}

} // namespace sig
} // namespace yarp


template <typename T>
std::string yarp::sig::Vec2D<T>::toString(int precision, int width) const
{
    std::ostringstream stringStream;
    stringStream.precision(precision);
    stringStream.width(width);
    stringStream << std::string("x:") << x << std::string(" y:") << y;
    return stringStream.str();
}

template <typename T>
T yarp::sig::Vec2D<T>::norm() const
{
    return T(sqrt(x*x + y*y));
}

//constructors
template <typename T>
yarp::sig::Vec2D<T>::Vec2D() : x(0), y(0)
{
}

template <typename T>
yarp::sig::Vec2D<T>::Vec2D(const yarp::sig::Vector& v)
{
    yAssert(v.size() == 2);
    x = T(v[0]);
    y = T(v[1]);
}

template <typename T>
yarp::sig::Vec2D<T>::Vec2D(const T& x_value, const T& y_value)
{
    x = x_value;
    y = y_value;
}

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

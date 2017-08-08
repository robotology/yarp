/*
* Author: Marco Randazzo
* Copyright (C) 2017 Istituto Italiano di Tecnologia (IIT)
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/math/Vec2D.h>
#include <yarp/math/Math.h>
#include <sstream>
#include <cmath>
#include <cstdio>

/// network stuff
#include <yarp/os/NetInt32.h>

using namespace yarp::math;

YARP_BEGIN_PACK
class Vec2DPortContentHeader
{
public:
    yarp::os::NetInt32 listTag;
    yarp::os::NetInt32 listLen;
};
YARP_END_PACK

namespace yarp
{
    namespace math
    {
        template<>
        bool Vec2D<double>::read(yarp::os::ConnectionReader& connection)
        {
            // auto-convert text mode interaction
            connection.convertTextMode();
            Vec2DPortContentHeader header;
            bool ok = connection.expectBlock((char*)&header, sizeof(header));
            if (!ok) return false;

            if (header.listLen == 2 && header.listTag == (BOTTLE_TAG_LIST | BOTTLE_TAG_DOUBLE))
            {
                this->x = connection.expectDouble();
                this->y = connection.expectDouble();
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

            if (header.listLen == 2 && header.listTag == (BOTTLE_TAG_LIST | BOTTLE_TAG_INT))
            {
                this->x = connection.expectInt();
                this->y = connection.expectInt();
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

            header.listTag = (BOTTLE_TAG_LIST | BOTTLE_TAG_DOUBLE);
            header.listLen = 2;

            connection.appendBlock((char*)&header, sizeof(header));

            connection.appendDouble(this->x);
            connection.appendDouble(this->y);

            connection.convertTextMode();

            return !connection.isError();
        }

        template<>
        bool Vec2D<int>::write(yarp::os::ConnectionWriter& connection)
        {
            Vec2DPortContentHeader header;

            header.listTag = (BOTTLE_TAG_LIST | BOTTLE_TAG_INT);
            header.listLen = 2;

            connection.appendBlock((char*)&header, sizeof(header));

            connection.appendInt(this->x);
            connection.appendInt(this->y);

            connection.convertTextMode();

            return !connection.isError();
        }
    }
}

template <typename T>
std::string yarp::math::Vec2D<T>::toString(int precision, int width) const
{
    std::ostringstream stringStream;
    stringStream.precision(precision);
    stringStream.width(width);
    stringStream << std::string("x:") << x << std::string(" y:") << y;
    return stringStream.str();
}

template <typename T>
T yarp::math::Vec2D<T>::norm() const
{
    return T(sqrt(x*x + y*y));
}

//constructors
template <typename T>
yarp::math::Vec2D<T>::Vec2D() : x(0), y(0)
{
}

template <typename T>
yarp::math::Vec2D<T>::Vec2D(const yarp::sig::Vector& v)
{
    yAssert(v.size() == 2);
    x = T(v[0]);
    y = T(v[1]);
}

template <typename T>
yarp::math::Vec2D<T>::Vec2D(const T& x_value, const T& y_value)
{
    x = x_value;
    y = y_value;
}

template <typename T>
 yarp::math::Vec2D<T>  operator * (const yarp::sig::Matrix& lhs, yarp::math::Vec2D<T> rhs)
{
    yAssert(lhs.rows() == 2 && lhs.cols() == 2);
    T x = rhs.x; T y = rhs.y;
    rhs.x = T(lhs[0][0] * x + lhs[0][1] * y);
    rhs.y = T(lhs[1][0] * x + lhs[1][1] * y);
    return rhs;
}

template <typename T>
yarp::math::Vec2D<T> operator + (yarp::math::Vec2D<T> lhs, const yarp::math::Vec2D<T>& rhs)
{
    lhs += rhs;
    return lhs;
}

template <typename T>
yarp::math::Vec2D<T> operator - (yarp::math::Vec2D<T> lhs, const yarp::math::Vec2D<T>& rhs)
{
    lhs -= rhs;
    return lhs;
}

template <typename T>
yarp::math::Vec2D<T>& yarp::math::Vec2D<T>::operator+=(const yarp::math::Vec2D<T>& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
}

template <typename T>
yarp::math::Vec2D<T>& yarp::math::Vec2D<T>::operator-=(const yarp::math::Vec2D<T>& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    return *this;
}

template <typename T>
yarp::math::Vec2D<T>& yarp::math::Vec2D<T>::operator =(const yarp::math::Vec2D<T>& rhs)
{
    if (this != &rhs)
    {
        x = rhs.x;
        y = rhs.y;
    }
    return *this;
}

template <typename T>
bool yarp::math::Vec2D<T>::operator ==(const yarp::math::Vec2D<T>& rhs)
{
    if (this->x == rhs.x &&
        this->y == rhs.y) return true;
    return false;
}

template <typename T>
bool yarp::math::Vec2D<T>::operator !=(const yarp::math::Vec2D<T>& rhs)
{
    if (this->x == rhs.x &&
        this->y == rhs.y) return false;
    return true;
}

template yarp::math::Vec2D<double> YARP_math_API operator + (yarp::math::Vec2D<double> lhs, const yarp::math::Vec2D<double>& rhs);
template yarp::math::Vec2D<int>    YARP_math_API operator + (yarp::math::Vec2D<int> lhs, const yarp::math::Vec2D<int>& rhs);
template yarp::math::Vec2D<double> YARP_math_API operator - (yarp::math::Vec2D<double> lhs, const yarp::math::Vec2D<double>& rhs);
template yarp::math::Vec2D<int>    YARP_math_API operator - (yarp::math::Vec2D<int> lhs, const yarp::math::Vec2D<int>& rhs);
template yarp::math::Vec2D<double> YARP_math_API operator * (const yarp::sig::Matrix& lhs, yarp::math::Vec2D<double> rhs);
template yarp::math::Vec2D<int>    YARP_math_API operator * (const yarp::sig::Matrix& lhs, yarp::math::Vec2D<int> rhs);

template class YARP_math_API yarp::math::Vec2D<double>;
template class YARP_math_API yarp::math::Vec2D<int>;


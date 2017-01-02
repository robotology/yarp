/*
* Author: Marco Randazzo
* Copyright (C) 2017 iCubFacility - Istituto Italiano di Tecnologia
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#ifndef YARP_VEC2D
#define YARP_VEC2D

#include <yarp/math/api.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/os/Portable.h>
#include <type_traits>
#include <sstream>
#include <math.h>

namespace yarp
{
    namespace math
    { 
        template <typename T>
        class Vec2D;
    }
}

template <typename T>
class yarp::math::Vec2D : public yarp::os::Portable
{
    static_assert (std::is_same<int, T>::value ||
                   std::is_same<double, T>::value, "Vec2D can be specialized only as int, double");
public:
    T x;
    T y;

public:
    Vec2D<T>();
    Vec2D<T>(const T& x_value, const T& y_value);
    Vec2D<T>(const yarp::sig::Vector& v);

    explicit operator yarp::sig::Vector () const
    {
        yarp::sig::Vector v(2);
        v[0] = double(x);
        v[1] = double(y);
        return v;
    }

    /**
    * Returns the Euclidean norm of the Vec2D, i.e. sqrt(x^2+y^2)
    * @return the computed Euclidean norm.
    */
    T norm() const;

    /**
    * Creates a string object containing a text representation of the object. Useful for printing.
    * @return the generated string
    */
    std::string toString(int precision=-1, int width=-1) const
    {
        std::ostringstream stringStream;
        stringStream.precision(precision);
        stringStream.width(width);
        stringStream << std::string("x:") << x << std::string(" y:") << y;
        return stringStream.str();
    }

    ///////// Serialization methods
    /*
    * Read vector from a connection.
    * return true if a Vec2D was read correctly
    */
    YARP_math_API virtual bool read(yarp::os::ConnectionReader& connection);

    /**
    * Write vector to a connection.
    * return true if a Vec2D was written correctly
    */
    YARP_math_API virtual bool write(yarp::os::ConnectionWriter& connection);

    virtual yarp::os::Type getType()
    {
        return yarp::os::Type::byName("yarp/vec2D");
    }

    //operators
    yarp::math::Vec2D<T>& operator+=(const yarp::math::Vec2D<T>& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }

    yarp::math::Vec2D<T>& operator-=(const yarp::math::Vec2D<T>& rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
        return *this;
    }

    yarp::math::Vec2D<T>& operator =(const yarp::math::Vec2D<T>& rhs)
    {
        if (this != &rhs)
        {
            x = rhs.x;
            y = rhs.y;
        }
        return *this;
    }

    friend yarp::math::Vec2D<T> operator + (yarp::math::Vec2D<T> lhs, const yarp::math::Vec2D<T>& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    friend yarp::math::Vec2D<T> operator - (yarp::math::Vec2D<T> lhs, const yarp::math::Vec2D<T>& rhs)
    {
        lhs -= rhs;
        return lhs;
    }
};

template <typename T>
yarp::math::Vec2D<T>::Vec2D(const yarp::sig::Vector& v)
{
    yAssert(v.size() == 2);
    x = T(v[0]);
    y = T(v[1]);
}

template <typename T>
T yarp::math::Vec2D<T>::norm() const
{
    return T(sqrt(x*x + y*y));
}

//constructors
template <typename T>
yarp::math::Vec2D<T> :: Vec2D() : x(0), y(0)
{
}

template <typename T>
yarp::math::Vec2D<T>::Vec2D(const T& x_value, const T& y_value)
{
    x = x_value;
    y = y_value;
}

//operators
template <typename T>
yarp::math::Vec2D<T> operator * (const yarp::sig::Matrix& lhs, yarp::math::Vec2D<T> rhs)
{
    yAssert(lhs.rows() == 2 && lhs.cols() == 2);
    T x = rhs.x; T y = rhs.y;
    rhs.x = T(lhs[0][0] * x + lhs[0][1] * y);
    rhs.y = T(lhs[1][0] * x + lhs[1][1] * y);
    return rhs;
}

#endif //#ifndef YARP_VEC2D

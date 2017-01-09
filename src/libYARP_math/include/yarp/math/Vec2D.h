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

namespace yarp
{
    namespace math
    { 
        template <typename T>
        class Vec2D;
    }
}

template <typename T>
class YARP_math_API yarp::math::Vec2D : public yarp::os::Portable
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
    explicit operator yarp::sig::Vector() const
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
    std::string toString(int precision = -1, int width = -1) const;

    ///////// Serialization methods
    /*
    * Read vector from a connection.
    * return true if a Vec2D was read correctly
    */
    virtual bool read(yarp::os::ConnectionReader& connection);

    /**
    * Write vector to a connection.
    * return true if a Vec2D was written correctly
    */
    virtual bool write(yarp::os::ConnectionWriter& connection);

    virtual yarp::os::Type getType()
    {
        return yarp::os::Type::byName("yarp/vec2D");
    }

    //operators
    yarp::math::Vec2D<T>& operator+=(const yarp::math::Vec2D<T>& rhs);
    yarp::math::Vec2D<T>& operator-=(const yarp::math::Vec2D<T>& rhs);
    yarp::math::Vec2D<T>& operator =(const yarp::math::Vec2D<T>& rhs);
};

//operators
template <typename T>
yarp::math::Vec2D<T> YARP_math_API operator + (yarp::math::Vec2D<T> lhs, const yarp::math::Vec2D<T>& rhs);

template <typename T>
yarp::math::Vec2D<T> YARP_math_API operator - (yarp::math::Vec2D<T> lhs, const yarp::math::Vec2D<T>& rhs);

template <typename T>
yarp::math::Vec2D<T> YARP_math_API operator * (const yarp::sig::Matrix& lhs, yarp::math::Vec2D<T> rhs);

#endif //#ifndef YARP_VEC2D

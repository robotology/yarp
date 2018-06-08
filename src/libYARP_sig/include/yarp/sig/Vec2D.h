/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SIG_VEC2D_H
#define YARP_SIG_VEC2D_H

#include <yarp/sig/api.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/os/Portable.h>
#include <type_traits>

namespace yarp {
namespace sig {

template <typename T>
class YARP_sig_API Vec2D : public yarp::os::Portable
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
    virtual bool read(yarp::os::ConnectionReader& connection) override;

    /**
    * Write vector to a connection.
    * return true if a Vec2D was written correctly
    */
    virtual bool write(yarp::os::ConnectionWriter& connection) override;

    virtual yarp::os::Type getType() const override
    {
        return yarp::os::Type::byName("yarp/vec2D");
    }

    //operators
    yarp::sig::Vec2D<T>& operator+=(const yarp::sig::Vec2D<T>& rhs);
    yarp::sig::Vec2D<T>& operator-=(const yarp::sig::Vec2D<T>& rhs);
    yarp::sig::Vec2D<T>& operator =(const yarp::sig::Vec2D<T>& rhs);
    bool operator == (const yarp::sig::Vec2D<T>& rhs);
    bool operator != (const yarp::sig::Vec2D<T>& rhs);
};

} // namespace math
} // namespace yarp

//operators
template <typename T>
yarp::sig::Vec2D<T> YARP_sig_API operator + (yarp::sig::Vec2D<T> lhs, const yarp::sig::Vec2D<T>& rhs);

template <typename T>
yarp::sig::Vec2D<T> YARP_sig_API operator - (yarp::sig::Vec2D<T> lhs, const yarp::sig::Vec2D<T>& rhs);

template <typename T>
yarp::sig::Vec2D<T> YARP_sig_API operator * (const yarp::sig::Matrix& lhs, yarp::sig::Vec2D<T> rhs);

#endif // YARP_MATH_VEC2D_H

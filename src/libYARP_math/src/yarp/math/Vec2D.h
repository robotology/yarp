/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MATH_VEC2D_H
#define YARP_MATH_VEC2D_H

#include <yarp/math/api.h>

#include <yarp/os/Portable.h>

#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>

#include <type_traits>

namespace yarp {
namespace math {

template <typename T>
class YARP_math_API Vec2D :
        public yarp::os::Portable
{
    static_assert(std::is_same<size_t, T>::value || std::is_same<int, T>::value || std::is_same<double, T>::value, "Vec2D can be specialized only as size_t, int, double");

public:
    T x;
    T y;

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
    bool read(yarp::os::ConnectionReader& connection) override;

    /**
    * Write vector to a connection.
    * return true if a Vec2D was written correctly
    */
    bool write(yarp::os::ConnectionWriter& connection) const override;

    yarp::os::Type getType() const override
    {
        return yarp::os::Type::byName("yarp/vec2D");
    }

    //operators
    yarp::math::Vec2D<T>& operator+=(const yarp::math::Vec2D<T>& rhs);
    yarp::math::Vec2D<T>& operator-=(const yarp::math::Vec2D<T>& rhs);
    bool operator==(const yarp::math::Vec2D<T>& rhs) const;
    bool operator!=(const yarp::math::Vec2D<T>& rhs) const;
};

} // namespace math
} // namespace yarp

// Forward declarations of specialized methods
template<> bool YARP_math_API yarp::math::Vec2D<double>::read(yarp::os::ConnectionReader& connection);
template<> bool YARP_math_API yarp::math::Vec2D<int>::read(yarp::os::ConnectionReader& connection);
template<> bool YARP_math_API yarp::math::Vec2D<size_t>::read(yarp::os::ConnectionReader& connection);
template<> bool YARP_math_API yarp::math::Vec2D<double>::write(yarp::os::ConnectionWriter& connection) const;
template<> bool YARP_math_API yarp::math::Vec2D<int>::write(yarp::os::ConnectionWriter& connection) const;
template<> bool YARP_math_API yarp::math::Vec2D<size_t>::write(yarp::os::ConnectionWriter& connection) const;

// Forward declaration of explicit instantiated template classes
YARP_math_EXTERN template class /*YARP_math_API*/ yarp::math::Vec2D<double>;
YARP_math_EXTERN template class /*YARP_math_API*/ yarp::math::Vec2D<int>;
YARP_math_EXTERN template class /*YARP_math_API*/ yarp::math::Vec2D<size_t>;



//operators
template <typename T>
yarp::math::Vec2D<T> operator+(yarp::math::Vec2D<T> lhs, const yarp::math::Vec2D<T>& rhs);

template <typename T>
yarp::math::Vec2D<T> operator-(yarp::math::Vec2D<T> lhs, const yarp::math::Vec2D<T>& rhs);

template <typename T>
yarp::math::Vec2D<T> operator*(const yarp::sig::Matrix& lhs, yarp::math::Vec2D<T> rhs);

// Forward declaration of explicit instantiated template functions
YARP_math_EXTERN template yarp::math::Vec2D<double> YARP_math_API operator + (yarp::math::Vec2D<double> lhs, const yarp::math::Vec2D<double>& rhs);
YARP_math_EXTERN template yarp::math::Vec2D<int>    YARP_math_API operator + (yarp::math::Vec2D<int> lhs, const yarp::math::Vec2D<int>& rhs);
YARP_math_EXTERN template yarp::math::Vec2D<double> YARP_math_API operator - (yarp::math::Vec2D<double> lhs, const yarp::math::Vec2D<double>& rhs);
YARP_math_EXTERN template yarp::math::Vec2D<int>    YARP_math_API operator - (yarp::math::Vec2D<int> lhs, const yarp::math::Vec2D<int>& rhs);
YARP_math_EXTERN template yarp::math::Vec2D<double> YARP_math_API operator * (const yarp::sig::Matrix& lhs, yarp::math::Vec2D<double> rhs);
YARP_math_EXTERN template yarp::math::Vec2D<int>    YARP_math_API operator * (const yarp::sig::Matrix& lhs, yarp::math::Vec2D<int> rhs);


#endif // YARP_MATH_VEC2D_H

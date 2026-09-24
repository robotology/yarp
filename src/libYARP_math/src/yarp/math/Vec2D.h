/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MATH_VEC2D_H
#define YARP_MATH_VEC2D_H

#include <yarp/math/api.h>

#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>

#include <yarp/sig/Vec2DOfDoubleData.h>
#include <yarp/sig/Vec2DOfIntData.h>
#include <yarp/sig/Vec2DOfSizetData.h>

template <typename T>
class YARP_math_API vec2D_selector
{
    static_assert(
        std::is_same_v<T, int> || std::is_same_v<T, size_t> || std::is_same_v<T, double>,
        "vec2D<T>: T must be int, size_t or double"
    );
};

namespace yarp::math
{
    template <typename Derived>
    Derived operator*(const yarp::sig::Matrix& lhs, const Derived& rhs);

    template <typename Derived, typename Scalar>
    class YARP_math_API Vec2DCommon
    {
    public:
        /**
        * Returns the Euclidean norm of the Vec2D, i.e. sqrt(x^2+y^2)
        * @return the computed Euclidean norm.
        */
        Scalar norm() const;

        /**
        * Creates a string object containing a text representation of the object. Useful for printing.
        * @return the generated string
        */
        std::string PrintToString(int precision = -1, int width = -1) const;

        Vec2DCommon() = default;
        Vec2DCommon(const yarp::sig::Vector& v);
        explicit operator yarp::sig::Vector() const;

        bool operator==(const Derived& other) const;
        bool operator!=(const Derived& other) const;
        Derived operator+(const Derived& other) const;
        Derived operator-(const Derived& other) const;
        Derived operator*(const Derived& other) const;
        Derived operator+=(const Derived& rhs);
        Derived operator-=(const Derived& rhs);
    };
}

namespace yarp::math
{
    class YARP_math_API Vec2DOfDouble : public yarp::sig::Vec2DOfDoubleData, public yarp::math::Vec2DCommon<Vec2DOfDouble, double>
    {
        public:
        using yarp::sig::Vec2DOfDoubleData::Vec2DOfDoubleData;
        using yarp::math::Vec2DCommon<Vec2DOfDouble, double>::Vec2DCommon;
    };

    class YARP_math_API Vec2DOfInt : public yarp::sig::Vec2DOfIntData, public yarp::math::Vec2DCommon<Vec2DOfInt, int>
    {
        public:
        using yarp::sig::Vec2DOfIntData::Vec2DOfIntData;
        using yarp::math::Vec2DCommon<Vec2DOfInt, int>::Vec2DCommon;
    };

    class YARP_math_API Vec2DOfSizet : public yarp::sig::Vec2DOfSizetData, public yarp::math::Vec2DCommon<Vec2DOfSizet, size_t>
    {
        public:
        using yarp::sig::Vec2DOfSizetData::Vec2DOfSizetData;
        using yarp::math::Vec2DCommon<Vec2DOfSizet, size_t>::Vec2DCommon;
    };
}


template <>
struct YARP_math_API  vec2D_selector<int> {
    using type = yarp::math::Vec2DOfInt;
};

template <>
struct YARP_math_API  vec2D_selector<size_t> {
    using type = yarp::math::Vec2DOfSizet;
};

template <>
struct YARP_math_API  vec2D_selector<double> {
    using type = yarp::math::Vec2DOfDouble;
};

namespace yarp::math {
template <typename T>
using Vec2D = typename vec2D_selector<T>::type;
}

#endif // YARP_MATH_VEC2D_H

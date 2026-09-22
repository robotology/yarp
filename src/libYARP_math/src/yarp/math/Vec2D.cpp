/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/math/Vec2D.h>

#include <yarp/os/LogComponent.h>
#include <yarp/math/Math.h>
#include <sstream>
#include <cmath>
#include <cstdio>

using namespace yarp::math;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(VEC2D, "yarp.math.Vec2D")
}

template <typename Derived, typename Scalar>
bool Vec2DCommon<Derived, typename Scalar>::operator==(const Derived& other) const
{
    const auto& v = static_cast<const Derived&>(*this);

    return v.x == other.x &&
            v.y == other.y;
}

template <typename Derived, typename Scalar>
bool  Vec2DCommon<Derived, typename Scalar>::operator!=(const Derived& other) const
{
    const auto& v = static_cast<const Derived&>(*this);

    return v.x != other.x ||
            v.y != other.y;
}

template <typename Derived, typename Scalar>
Derived Vec2DCommon<Derived, typename Scalar>::operator+(const Derived& other) const
{
    const auto& v = static_cast<const Derived&>(*this);

    Derived result;
    result.x = v.x + other.x;
    result.y = v.y + other.y;

    return result;
}

template <typename Derived, typename Scalar>
Derived Vec2DCommon<Derived, typename Scalar>::operator-(const Derived& other) const
{
    const auto& v = static_cast<const Derived&>(*this);

    Derived result;
    result.x = v.x - other.x;
    result.y = v.y - other.y;

    return result;
}

template <typename Derived, typename Scalar>
Derived Vec2DCommon<Derived, typename Scalar>::operator*(const Derived& other) const
{
    const auto& v = static_cast<const Derived&>(*this);

    Derived result;
    result.x = v.x * other.x;
    result.y = v.y * other.y;

    return result;
}

template <typename Derived, typename Scalar>
Derived Vec2DCommon<Derived, typename Scalar>::operator+=(const Derived& other)
{
    auto& v = static_cast<Derived&>(*this);
    Derived result;
    result.x = v.x += other.x;
    result.y = v.y += other.y;
    return result;
}

template <typename Derived, typename Scalar>
Derived Vec2DCommon<Derived, typename Scalar>::operator-=(const Derived& other)
{
    auto& v = static_cast<Derived&>(*this);
    Derived result;
    result.x = v.x -= other.x;
    result.y = v.y -= other.y;
    return result;
}

template <typename Derived, typename Scalar>
Vec2DCommon<Derived, typename Scalar>::Vec2DCommon(const yarp::sig::Vector& v)
{
    yCAssert(VEC2D, v.size() == 2);

    auto& ccv = static_cast<Derived&>(*this);
    ccv.x = decltype(ccv.x)(v[0]);
    ccv.y = decltype(ccv.y)(v[1]);
}


namespace yarp::math
{
    Vec2DOfInt operator*(const yarp::sig::Matrix& lhs, const Vec2DOfInt& rhs)
    {
        yCAssert(VEC2D, lhs.rows() == 2 && lhs.cols() == 2);
        Vec2DOfInt result;
        result.x = (lhs[0][0] * rhs.x + lhs[0][1] * rhs.y);
        result.y = (lhs[1][0] * rhs.x + lhs[1][1] * rhs.y);
        return result;
    }

    Vec2DOfDouble operator*(const yarp::sig::Matrix& lhs, const Vec2DOfDouble& rhs)
    {
        yCAssert(VEC2D, lhs.rows() == 2 && lhs.cols() == 2);
        Vec2DOfDouble result;
        result.x = (lhs[0][0] * rhs.x + lhs[0][1] * rhs.y);
        result.y = (lhs[1][0] * rhs.x + lhs[1][1] * rhs.y);
        return result;
    }
} // namespace yarp::math


template <typename Derived, typename Scalar>
std::string yarp::math::Vec2DCommon<Derived, Scalar>::PrintToString(int precision, int width) const
{
    const auto& ccv = static_cast<const Derived&>(*this);

    std::ostringstream stringStream;
    stringStream.precision(precision);
    stringStream.width(width);
    stringStream << std::string("x:") << ccv.x << std::string(" y:") << ccv.y;
    return stringStream.str();
}


template <typename Derived, typename Scalar>
Scalar yarp::math::Vec2DCommon<Derived, Scalar>::norm() const
{
    const auto& v = static_cast<const Derived&>(*this);
    return static_cast<Scalar>(std::sqrt(static_cast<double>(v.x * v.x + v.y * v.y)));
}

template <typename Derived, typename Scalar>
yarp::math::Vec2DCommon<Derived, typename Scalar>::operator yarp::sig::Vector() const
{
    const auto& ccv = static_cast<const Derived&>(*this);

    yarp::sig::Vector v(2);
    v[0] = double(ccv.x);
    v[1] = double(ccv.y);
    return v;
}

// Explicit instances
template class Vec2DCommon<Vec2DOfInt, int>;
template class Vec2DCommon<Vec2DOfDouble, double>;
template class Vec2DCommon<Vec2DOfSizet, size_t>;

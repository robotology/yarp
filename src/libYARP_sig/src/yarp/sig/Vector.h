/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_VECTOR_H
#define YARP_SIG_VECTOR_H

#include <cstddef> //defines size_t
#include <memory>
#include <string>
#include <algorithm>
#include <type_traits>

#include <yarp/sig/api.h>
#include <yarp/sig/VectorBase.h>
#include <yarp/sig/VectorOfDoubleData.h>
#include <yarp/sig/VectorOfInt32Data.h>
#include <yarp/sig/VectorOfSizetData.h>
#include <yarp/sig/VectorOfFloatData.h>
#include <yarp/sig/VectorOfStringData.h>

/**
* \file Vector.h contains the definition of a Vector type
*/

template <typename T>
class YARP_sig_API vector_selector
{
    static_assert(
        std::is_same_v<T, int> || std::is_same_v<T, size_t> ||
        std::is_same_v<T, double> || std::is_same_v<T, std::string> || std::is_same_v<T, float>,
        "VectorOf<T>: T must be int, size_t, float, string or double"
    );
};

namespace yarp::sig
{
    class VectorOfDouble;
    class VectorOfInt;
    class VectorOfSizet;
    class VectorOfString;
    class VectorOfFloat;
    class VectorOfSizetData;

    typedef VectorOfDouble Vector;
} // namespace yarp::sig


class YARP_sig_API yarp::sig::VectorOfDouble : public yarp::sig::VectorOfDoubleData, public yarp::sig::VectorBase<VectorOfDouble, double>
{
    friend class VectorBase<VectorOfDouble, double>;

    public:
    using yarp::sig::VectorOfDoubleData::VectorOfDoubleData;
    using yarp::sig::VectorBase<VectorOfDouble, double>::VectorBase;
    using yarp::sig::VectorBase<VectorOfDouble, double>::operator=;
    protected:
#ifndef SWIG
    auto& privVec() { return privdata; }
    const auto& privVec() const { return privdata; }
#endif
};

class YARP_sig_API yarp::sig::VectorOfInt : public yarp::sig::VectorOfInt32Data, public yarp::sig::VectorBase<VectorOfInt, int>
{
    friend class VectorBase<VectorOfInt, int>;

    public:
    using yarp::sig::VectorOfInt32Data::VectorOfInt32Data;
    using yarp::sig::VectorBase<VectorOfInt, int>::VectorBase;
    using yarp::sig::VectorBase<VectorOfInt, int>::operator=;
    protected:
#ifndef SWIG
    auto& privVec() { return privdata; }
    const auto& privVec() const { return privdata; }
#endif
};

class YARP_sig_API yarp::sig::VectorOfFloat : public yarp::sig::VectorOfFloatData, public yarp::sig::VectorBase<VectorOfFloat, float>
{
    friend class VectorBase<VectorOfFloat, float>;

    public:
    using yarp::sig::VectorOfFloatData::VectorOfFloatData;
    using yarp::sig::VectorBase<VectorOfFloat, float>::VectorBase;
    using yarp::sig::VectorBase<VectorOfFloat, float>::operator=;
    protected:
#ifndef SWIG
    auto& privVec() { return privdata; }
    const auto& privVec() const { return privdata; }
#endif
};

class YARP_sig_API yarp::sig::VectorOfString : public yarp::sig::VectorOfStringData, public yarp::sig::VectorBase<VectorOfString, std::string>
{
    friend class VectorBase<VectorOfString, std::string>;

    public:
    using yarp::sig::VectorOfStringData::VectorOfStringData;
    using yarp::sig::VectorBase<VectorOfString, std::string>::VectorBase;
    using yarp::sig::VectorBase<VectorOfString, std::string>::operator=;
    protected:
#ifndef SWIG
    auto& privVec() { return privdata; }
    const auto& privVec() const { return privdata; }
#endif
};

class YARP_sig_API yarp::sig::VectorOfSizet : public yarp::sig::VectorOfSizetData, public yarp::sig::VectorBase<VectorOfSizet, size_t>
{
    friend class VectorBase<VectorOfSizet, size_t>;

    public:
    using yarp::sig::VectorOfSizetData::VectorOfSizetData;
    using yarp::sig::VectorBase<VectorOfSizet, size_t>::VectorBase;
    using yarp::sig::VectorBase<VectorOfSizet, size_t>::operator=;
    protected:
#ifndef SWIG
    auto& privVec() { return privdata; }
    const auto& privVec() const { return privdata; }
#endif
};

template <>
struct YARP_sig_API  vector_selector<int> {
    using type = yarp::sig::VectorOfInt;
};

template <>
struct YARP_sig_API  vector_selector<double> {
    using type = yarp::sig::VectorOfDouble;
};

template <>
struct YARP_sig_API  vector_selector<float> {
    using type = yarp::sig::VectorOfFloat;
};

template <>
struct YARP_sig_API  vector_selector<std::string> {
    using type = yarp::sig::VectorOfString;
};

template <>
struct YARP_sig_API  vector_selector<size_t> {
    using type = yarp::sig::VectorOfSizet;
};

namespace yarp::sig {
template <typename T>
using VectorOf = typename vector_selector<T>::type;
}

#endif // YARP_SIG_VECTOR_H

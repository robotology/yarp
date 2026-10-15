/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/Vector.h>

#include <yarp/os/LogComponent.h>

#include <algorithm>
#include <cstdio>

using namespace yarp::sig;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(VECTOR, "yarp.sig.Vector")

// Upper bound for precision and width, to avoid huge allocations
// (e.g. toFormattedString(INT_MAX)).
constexpr int maxFormatField = 100;

template <typename T>
std::string toFormattedStringImpl(const T* data, size_t len, int precision, int width)
{
    static_assert(std::is_floating_point_v<T>, "toFormattedString requires a floating point type");

    std::string ret;
    if (len == 0 || data == nullptr) {
        return ret;
    }

    // A negative precision is treated by printf as omitted (default: 6)
    precision = std::min(precision, maxFormatField);
    const bool useWidth = (width >= 0);
    width = std::min(width, maxFormatField);
    const char separator = useWidth ? ' ' : '\t';

    // float is promoted to double when passed through varargs
    auto format = [&](char* buf, size_t bufSize, T value) {
        return useWidth ? std::snprintf(buf, bufSize, "% *.*f", width, precision, static_cast<double>(value))
                        : std::snprintf(buf, bufSize, "% .*f", precision, static_cast<double>(value));
    };

    ret.reserve(len * 12);
    char buf[64];
    for (size_t i = 0; i < len; ++i) {
        if (i != 0) {
            ret += separator;
        }
        const int n = format(buf, sizeof(buf), data[i]);
        if (n < 0) {
            yCError(VECTOR, "toFormattedString: formatting error at index %zu", i);
            continue;
        }
        if (static_cast<size_t>(n) < sizeof(buf)) {
            ret.append(buf, static_cast<size_t>(n));
        } else {
            // Very large values (e.g. 1e300) or large precision/width:
            // print directly into the output string, without truncation.
            const size_t offset = ret.size();
            ret.resize(offset + static_cast<size_t>(n) + 1);
            format(&ret[offset], static_cast<size_t>(n) + 1, data[i]);
            ret.resize(offset + static_cast<size_t>(n));
        }
    }
    return ret;
}
} // namespace

std::string VectorOfDouble::toFormattedString(int precision, int width) const
{
    return toFormattedStringImpl(data(), size(), precision, width);
}

std::string VectorOfFloat::toFormattedString(int precision, int width) const
{
    return toFormattedStringImpl(data(), size(), precision, width);
}

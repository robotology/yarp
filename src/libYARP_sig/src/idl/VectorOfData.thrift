/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.sig

typedef i32    ( yarp.type = "size_t" ) size32
typedef double ( yarp.type = "float" )  float32

struct VectorOfDoubleData
{
    1: list<double> privdata;
}
(
    yarp.data_visibility = "protected"
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)

struct VectorOfInt32Data
{
    1: list<i32> privdata;
}
(
    yarp.data_visibility = "protected"
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)

struct VectorOfSizetData
{
    1: list<size32> privdata;
}
(
    yarp.data_visibility = "protected"
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)

struct VectorOfFloatData
{
    1: list<float32> privdata;
}
(
    yarp.data_visibility = "protected"
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)

struct VectorOfStringData
{
    1: list<string> privdata;
}
(
    yarp.data_visibility = "protected"
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)

/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.sig

struct MatrixData
{
    1: i32 nrows;
    2: i32 ncols;
    3: list<double> storage;
}
(
    yarp.data_visibility = "protected"
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)

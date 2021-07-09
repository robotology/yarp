/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_CONSTSTRING_H
#define YARP_OS_CONSTSTRING_H

#include <yarp/conf/system.h>

#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/os/ConstString.h> file is deprecated")
#endif

#include <yarp/os/api.h>

#include <string>

namespace yarp {
namespace os {

YARP_DEPRECATED_TYPEDEF_MSG("Use std::string instead")
std::string ConstString;

} // namespace os
} // namespace yarp

#endif // YARP_OS_CONSTSTRING_H

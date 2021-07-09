/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef YARP_CONF_FILESYSTEM_H
#define YARP_CONF_FILESYSTEM_H

namespace yarp {
namespace conf {
namespace filesystem {
#if (defined _WIN32)
#  if (_MSC_VER >= 1920)
typedef wchar_t             value_type;
static constexpr value_type preferred_separator = L'\\';
#  else
typedef char                value_type;
static constexpr value_type preferred_separator = '\\';
#  endif
#else
typedef char                value_type;
static constexpr value_type preferred_separator = '/';
#endif


#ifndef YARP_NO_DEPRECATED // since YARP 3.5.0
#include <yarp/conf/api.h> // For YARP_DEPRECATED_TYPEDEF_MSG
YARP_DEPRECATED_MSG("Use yarp::conf::environment::path_separator instead")
#if (defined _WIN32)
#  if (_MSC_VER >= 1920)
static constexpr value_type path_separator = L';';
#  else
static constexpr value_type path_separator = ';';
#  endif
#else
static constexpr value_type path_separator = ':';
#endif
#endif // YARP_NO_DEPRECATED


} // namespace filesystem
} // namespace conf
} // namespace yarp


#endif // YARP_CONF_FILESYSTEM_H

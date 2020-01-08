/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#ifndef YARP_OS_FILESYSTEM_H
#define YARP_OS_FILESYSTEM_H

namespace yarp {
namespace conf {
namespace filesystem {
#if (defined _WIN32)
#  if (_MSC_VER >= 1920)
typedef wchar_t             value_type;
static constexpr value_type preferred_separator = L'\\';
static constexpr value_type path_separator = L';';
#  else
typedef char                value_type;
static constexpr value_type preferred_separator = '\\';
static constexpr value_type path_separator = ';';
#  endif
#else
typedef char                value_type;
static constexpr value_type preferred_separator = '/';
static constexpr value_type path_separator = ':';
#endif
} // namespace filesystem
} // namespace conf
} // namespace yarp


#endif // YARP_OS_FILESYSTEM_H

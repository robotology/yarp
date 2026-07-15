/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef YARP_CONF_FILESYSTEM_H
#define YARP_CONF_FILESYSTEM_H

namespace yarp::conf::filesystem {
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

} // namespace yarp::conf::filesystem


#endif // YARP_CONF_FILESYSTEM_H

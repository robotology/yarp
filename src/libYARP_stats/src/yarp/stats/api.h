/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_STATS_API_H
#define YARP_STATS_API_H

#include <yarp/conf/api.h>
#ifndef YARP_stats_API
#  ifdef YARP_stats_EXPORTS
#    define YARP_stats_API YARP_EXPORT
#    define YARP_stats_EXTERN YARP_EXPORT_EXTERN
#    define YARP_stats_DEPRECATED_API YARP_DEPRECATED_EXPORT
#    define YARP_stats_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_EXPORT(X)
#  else
#    define YARP_stats_API YARP_IMPORT
#    define YARP_stats_EXTERN YARP_IMPORT_EXTERN
#    define YARP_stats_DEPRECATED_API YARP_DEPRECATED_IMPORT
#    define YARP_stats_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_IMPORT(X)
#  endif
#endif

#endif // YARP_RUN_API_H

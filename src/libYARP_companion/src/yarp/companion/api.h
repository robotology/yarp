/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_COMPANION_API_H
#define YARP_COMPANION_API_H

#include <yarp/conf/api.h>
#ifndef YARP_companion_API
#  ifdef YARP_companion_EXPORTS
#    define YARP_companion_API YARP_EXPORT
#    define YARP_companion_EXTERN YARP_EXPORT_EXTERN
#    define YARP_companion_DEPRECATED_API YARP_DEPRECATED_EXPORT
#    define YARP_companion_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_EXPORT(X)
#  else
#    define YARP_companion_API YARP_IMPORT
#    define YARP_companion_EXTERN YARP_IMPORT_EXTERN
#    define YARP_companion_DEPRECATED_API YARP_DEPRECATED_IMPORT
#    define YARP_companion_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_IMPORT(X)
#  endif
#endif

#endif // YARP_COMPANION_API_H

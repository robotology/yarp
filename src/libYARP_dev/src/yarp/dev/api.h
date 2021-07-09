/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_API_H
#define YARP_DEV_API_H

#include <yarp/conf/api.h>
#ifndef YARP_dev_API
#  ifdef YARP_dev_EXPORTS
#    define YARP_dev_API YARP_EXPORT
#    define YARP_dev_EXTERN YARP_EXPORT_EXTERN
#    define YARP_dev_DEPRECATED_API YARP_DEPRECATED_EXPORT
#    define YARP_dev_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_EXPORT(X)
#  else
#    define YARP_dev_API YARP_IMPORT
#    define YARP_dev_EXTERN YARP_IMPORT_EXTERN
#    define YARP_dev_DEPRECATED_API YARP_DEPRECATED_IMPORT
#    define YARP_dev_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_IMPORT(X)
#  endif
#endif

#endif // YARP_DEV_API_H

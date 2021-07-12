/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_ROBOTINTERFACE_API_H
#define YARP_ROBOTINTERFACE_API_H

#include <yarp/conf/api.h>

#ifndef YARP_robotinterface_API
#    ifdef YARP_robotinterface_EXPORTS
#        define YARP_robotinterface_API YARP_EXPORT
#        define YARP_robotinterface_EXTERN YARP_EXPORT_EXTERN
#        define YARP_robotinterface_DEPRECATED_API YARP_DEPRECATED_EXPORT
#        define YARP_robotinterface_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_EXPORT(X)
#    else
#        define YARP_robotinterface_API YARP_IMPORT
#        define YARP_robotinterface_EXTERN YARP_IMPORT_EXTERN
#        define YARP_robotinterface_DEPRECATED_API YARP_DEPRECATED_IMPORT
#        define YARP_robotinterface_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_IMPORT(X)
#    endif
#endif

#endif // YARP_ROBOTINTERFACE_API_H

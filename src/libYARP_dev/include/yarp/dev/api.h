/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_API_H
#define YARP_DEV_API_H

#include <yarp/conf/api.h>
#ifndef YARP_dev_API
#  ifdef YARP_dev_EXPORTS
#    define YARP_dev_API YARP_EXPORT
#    define YARP_dev_EXTERN YARP_EXPORT_EXTERN
#  else
#    define YARP_dev_API YARP_IMPORT
#    define YARP_dev_EXTERN YARP_IMPORT_EXTERN
#  endif
#  define YARP_dev_DEPRECATED_API YARP_DEPRECATED_API
#endif

#endif // YARP_DEV_API_H

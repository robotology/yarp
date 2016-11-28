/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
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
#  ifdef YARP_NO_DEPRECATED
#    define YARP_dev_DEPRECATED_API YARP_DEPRECATED
#    define YARP_dev_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG(X)
#  else
#    define YARP_dev_DEPRECATED_API YARP_DEPRECATED YARP_dev_API
#    define YARP_dev_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG(X) YARP_dev_API
#  endif
#endif

#endif // YARP_DEV_API_H

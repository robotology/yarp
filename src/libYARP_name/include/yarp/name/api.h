/*
 * Copyright (C) 2011 Istituto Italiano di Tecnologia (IIT)
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_NAME_API_H
#define YARP_NAME_API_H

#include <yarp/conf/api.h>
#ifndef YARP_name_API
#  ifdef YARP_name_EXPORTS
#    define YARP_name_API YARP_EXPORT
#    define YARP_name_EXTERN YARP_EXPORT_EXTERN
#  else
#    define YARP_name_API YARP_IMPORT
#    define YARP_name_EXTERN YARP_IMPORT_EXTERN
#  endif
#  define YARP_name_DEPRECATED_API YARP_DEPRECATED_API
#  define YARP_name_DEPRECATED_API_MSG(X) YARP_DEPRECATED_API_MSG(X)
#endif

#endif // YARP_NAME_API_H

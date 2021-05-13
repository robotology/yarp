/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_ROBOTTESTINGFRAMEWORK_API_H
#define YARP_ROBOTTESTINGFRAMEWORK_API_H

#include <yarp/conf/api.h>
#ifndef YARP_robottestingframework_API
#  ifdef YARP_robottestingframework_EXPORTS
#    define YARP_robottestingframework_API YARP_EXPORT
#    define YARP_robottestingframework_EXTERN YARP_EXPORT_EXTERN
#    define YARP_robottestingframework_DEPRECATED_API YARP_DEPRECATED_EXPORT
#    define YARP_robottestingframework_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_EXPORT(X)
#  else
#    define YARP_robottestingframework_API YARP_IMPORT
#    define YARP_robottestingframework_EXTERN YARP_IMPORT_EXTERN
#    define YARP_robottestingframework_DEPRECATED_API YARP_DEPRECATED_IMPORT
#    define YARP_robottestingframework_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_IMPORT(X)
#  endif
#endif

#endif // YARP_ROBOTTESTINGFRAMEWORK_API_H

/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_COMPANION_API_H
#define YARP_COMPANION_API_H

#include <yarp/conf/api.h>
#ifndef YARP_companion_API
#  ifdef YARP_companion_EXPORTS
#    define YARP_companion_API YARP_EXPORT
#    define YARP_companion_EXTERN YARP_EXPORT_EXTERN
#  else
#    define YARP_companion_API YARP_IMPORT
#    define YARP_companion_EXTERN YARP_IMPORT_EXTERN
#  endif
#  define YARP_companion_DEPRECATED_API YARP_DEPRECATED_API
#endif

#endif // YARP_COMPANION_API_H

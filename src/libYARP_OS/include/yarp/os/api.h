/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_API_H
#define YARP_OS_API_H

#include <yarp/conf/api.h>
#ifndef YARP_OS_API
#    ifdef YARP_OS_EXPORTS
#        define YARP_OS_API YARP_EXPORT
#        define YARP_OS_EXTERN YARP_EXPORT_EXTERN
#    else
#        define YARP_OS_API YARP_IMPORT
#        define YARP_OS_EXTERN YARP_IMPORT_EXTERN
#    endif
#    define YARP_OS_DEPRECATED_API YARP_DEPRECATED_API
#    define YARP_OS_DEPRECATED_API_MSG(X) YARP_DEPRECATED_API_MSG(X)
#endif

#ifndef YARP_init_API
#    ifdef YARP_init_EXPORTS
#        define YARP_init_API YARP_EXPORT
#        define YARP_init_EXTERN YARP_EXPORT_EXTERN
#    else
#        define YARP_init_API YARP_IMPORT
#        define YARP_init_EXTERN YARP_IMPORT_EXTERN
#    endif
#    define YARP_init_DEPRECATED_API YARP_DEPRECATED_API
#    define YARP_init_DEPRECATED_API_MSG(X) YARP_DEPRECATED_API_MSG(X)
#endif

#ifndef YARP_OS_impl_API
// FIXME YARP should not export symbols from os/impl.
//       Unfortunately at the moment, methods from impl are used in other
//       internal libraries, forcing us to export them in the library
#    ifdef YARP_FILTER_impl
#        define YARP_OS_impl_API
#    else
#        define YARP_OS_impl_API YARP_OS_API
#    endif
#endif

#endif // YARP_OS_API_H

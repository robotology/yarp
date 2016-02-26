/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_SIG_API_H
#define YARP_SIG_API_H

#include <yarp/conf/api.h>
#ifndef YARP_sig_API
#  ifdef YARP_sig_EXPORTS
#    define YARP_sig_API YARP_EXPORT
#    define YARP_sig_EXTERN YARP_EXPORT_EXTERN
#  else
#    define YARP_sig_API YARP_IMPORT
#    define YARP_sig_EXTERN YARP_IMPORT_EXTERN
#  endif
#  ifdef YARP_NO_DEPRECATED
#    define YARP_sig_DEPRECATED_API YARP_DEPRECATED
#  else
#    define YARP_sig_DEPRECATED_API YARP_DEPRECATED YARP_sig_API
#  endif
#endif

#endif // YARP_SIG_API_H

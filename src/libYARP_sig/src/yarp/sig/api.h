/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_API_H
#define YARP_SIG_API_H

#include <yarp/conf/api.h>
#ifndef YARP_sig_API
#  ifdef YARP_sig_EXPORTS
#    define YARP_sig_API YARP_EXPORT
#    define YARP_sig_EXTERN YARP_EXPORT_EXTERN
#    define YARP_sig_DEPRECATED_API YARP_DEPRECATED_EXPORT
#    define YARP_sig_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_EXPORT(X)
#  else
#    define YARP_sig_API YARP_IMPORT
#    define YARP_sig_EXTERN YARP_IMPORT_EXTERN
#    define YARP_sig_DEPRECATED_API YARP_DEPRECATED_IMPORT
#    define YARP_sig_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_IMPORT(X)
#  endif
#endif

#endif // YARP_SIG_API_H

/*
 * Copyright: (C) 2017 Istituto Italiano di Tecnologia (IIT)
 * Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_RTF_API_H
#define YARP_RTF_API_H

#include <yarp/conf/api.h>
#ifndef YARP_rtf_API
#  ifdef YARP_rtf_EXPORTS
#    define YARP_rtf_API YARP_EXPORT
#    define YARP_rtf_EXTERN YARP_EXPORT_EXTERN
#  else
#    define YARP_rtf_API YARP_IMPORT
#    define YARP_rtf_EXTERN YARP_IMPORT_EXTERN
#  endif
#  define YARP_rtf_DEPRECATED_API YARP_DEPRECATED_API
#  define YARP_rtf_DEPRECATED_API_MSG(X) YARP_DEPRECATED_API_MSG(X)
#endif

#endif // YARP_RTF_API_H

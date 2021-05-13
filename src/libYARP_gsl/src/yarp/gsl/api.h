/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef YARP_GSL_API_H
#define YARP_GSL_API_H

#include <yarp/conf/api.h>
#ifndef YARP_gsl_API
#  ifdef YARP_gsl_EXPORTS
#    define YARP_gsl_API YARP_EXPORT
#    define YARP_gsl_EXTERN YARP_EXPORT_EXTERN
#    define YARP_gsl_DEPRECATED_API YARP_DEPRECATED_EXPORT
#    define YARP_gsl_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_EXPORT(X)
#  else
#    define YARP_gsl_API YARP_IMPORT
#    define YARP_gsl_EXTERN YARP_IMPORT_EXTERN
#    define YARP_gsl_DEPRECATED_API YARP_DEPRECATED_IMPORT
#    define YARP_gsl_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_IMPORT(X)
#  endif
#endif

#endif // YARP_GSL_API_H

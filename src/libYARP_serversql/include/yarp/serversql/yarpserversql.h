/*
 * Copyright (C) 2011 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_INC_YARPSERVERSQL
#define YARP_INC_YARPSERVERSQL

#include <yarp/conf/api.h>

#include <yarp/os/NameStore.h>
#include <yarp/os/Searchable.h>

// avoid dll export until libYARP_serversql becomes dynamic again
#define yarpserversql_API

#ifndef yarpserversql_API
#  ifdef yarpserversql_EXPORTS
#    define yarpserversql_API YARP_EXPORT
#    define yarpserversql_EXTERN YARP_EXPORT_EXTERN
#  else
#    define yarpserversql_API YARP_IMPORT
#    define yarpserversql_EXTERN YARP_IMPORT_EXTERN
#  endif
#  ifdef YARP_NO_DEPRECATED
#    define yarpserversql_DEPRECATED_API YARP_DEPRECATED
#  else
#    define yarpserversql_DEPRECATED_API YARP_DEPRECATED yarpserversql_API
#  endif
#endif

yarpserversql_API int yarpserver3_main(int argc, char *argv[]);

yarpserversql_API yarp::os::NameStore *yarpserver3_create(yarp::os::Searchable& options);

#endif

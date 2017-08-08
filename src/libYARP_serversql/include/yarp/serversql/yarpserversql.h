/*
 * Copyright (C) 2011 Istituto Italiano di Tecnologia (IIT)
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_SERVERSQL_YARPSERVERSQL_H
#define YARP_SERVERSQL_YARPSERVERSQL_H

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
#  define yarpserversql_DEPRECATED_API YARP_DEPRECATED_API
#  define yarpserversql_DEPRECATED_API_MSG(X) YARP_DEPRECATED_API_MSG(X)
#endif

yarpserversql_API int yarpserver_main(int argc, char *argv[]);

yarpserversql_API yarp::os::NameStore *yarpserver_create(yarp::os::Searchable& options);

#endif // YARP_SERVERSQL_YARPSERVERSQL_H

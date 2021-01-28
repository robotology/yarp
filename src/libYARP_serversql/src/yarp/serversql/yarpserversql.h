/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SERVERSQL_YARPSERVERSQL_H
#define YARP_SERVERSQL_YARPSERVERSQL_H

#include <yarp/serversql/api.h>

#include <yarp/os/NameStore.h>
#include <yarp/os/Searchable.h>

YARP_serversql_API int yarpserver_main(int argc, char *argv[]);

YARP_serversql_API yarp::os::NameStore *yarpserver_create(yarp::os::Searchable& options);

#endif // YARP_SERVERSQL_YARPSERVERSQL_H

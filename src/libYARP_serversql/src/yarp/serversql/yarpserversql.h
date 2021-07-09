/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SERVERSQL_YARPSERVERSQL_H
#define YARP_SERVERSQL_YARPSERVERSQL_H

#include <yarp/serversql/api.h>

#include <yarp/os/NameStore.h>
#include <yarp/os/Searchable.h>

YARP_serversql_API int yarpserver_main(int argc, char *argv[]);

YARP_serversql_API yarp::os::NameStore *yarpserver_create(yarp::os::Searchable& options);

#endif // YARP_SERVERSQL_YARPSERVERSQL_H

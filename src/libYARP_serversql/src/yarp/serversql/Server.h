/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#ifndef YARP_SERVERSQL_SERVER_H
#define YARP_SERVERSQL_SERVER_H

#include <yarp/serversql/api.h>

namespace yarp {
namespace serversql {

class YARP_serversql_API Server
{
public:
    void stop() { shouldStop = true; }
    int run(int argc, char* argv[]);

private:
    bool   shouldStop{false};
};

} // namespace yarpserversql
} // namespace yarp

#endif // YARP_SERVERSQL_SERVER_H

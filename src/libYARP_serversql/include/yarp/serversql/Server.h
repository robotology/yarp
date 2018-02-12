/*
 * Copyright (C) 2017 Istituto Italiano di Tecnologia (IIT)
 * Authors: Andrea Ruzzenenti
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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


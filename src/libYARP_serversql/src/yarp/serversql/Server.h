/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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

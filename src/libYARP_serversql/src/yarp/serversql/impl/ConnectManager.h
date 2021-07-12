/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/serversql/impl/ConnectThread.h>

#include <list>
#include <mutex>

namespace yarp {
namespace serversql {
namespace impl {

#ifndef YARP_SERVERSQL_IMPL_CONNECTMANAGER_H
#define YARP_SERVERSQL_IMPL_CONNECTMANAGER_H

class ConnectManager
{
private:
    std::list<ConnectThread*> con;
    std::mutex mutex;
public:

    ConnectManager();

    virtual ~ConnectManager();

    void clear();

    void disconnect(const std::string& src,
                    const std::string& dest,
                    bool srcDrop);

    void connect(const std::string& src,
                 const std::string& dest,
                 bool positive = true);
};

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_CONNECTMANAGER_H

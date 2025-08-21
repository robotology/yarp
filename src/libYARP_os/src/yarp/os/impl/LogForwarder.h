/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_LOGFORWARDER_H
#define YARP_OS_IMPL_LOGFORWARDER_H

#include <yarp/os/api.h>

#include <yarp/os/Port.h>
#include <yarp/os/PortWriterBuffer.h>

#include <mutex>
#include <string>
#include <yarp/os/PeriodicThread.h>

#include <yarp/os/BufferedPort.h>
#include <list>

namespace yarp::os::impl {

class ThreadedPort : public yarp::os::PeriodicThread
{
    std::mutex mut;
    yarp::os::BufferedPort<yarp::os::Bottle>* m_port = nullptr;
    std::list<yarp::os::Bottle> messages;

    void process();

public:
    void run() override;
    void attach(yarp::os::BufferedPort<yarp::os::Bottle>* port);
    void insert(const yarp::os::Bottle& bot);
    void terminate();
public:
    ThreadedPort();
};

//----------------------------------------------------------------

class YARP_os_impl_API LogForwarder
{
public:
    ~LogForwarder();
    static LogForwarder& getInstance();

    void forward(const std::string& message);
    std::string getLogPortName();
    static void shutdown();

private:
    LogForwarder();
    LogForwarder(LogForwarder const&) = delete;
    LogForwarder& operator=(LogForwarder const&) = delete;

    ThreadedPort   tport;
    yarp::os::BufferedPort<yarp::os::Bottle> outputPort;
    static bool started;
};

} // namespace yarp::os::impl

#endif // YARP_OS_IMPL_LOGFORWARDER_H

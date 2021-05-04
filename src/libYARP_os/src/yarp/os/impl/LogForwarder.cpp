/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/LogForwarder.h>

#include <yarp/os/Log.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/SystemInfo.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/PlatformLimits.h>

#include <sstream>

bool yarp::os::impl::LogForwarder::started{false};

yarp::os::impl::LogForwarder& yarp::os::impl::LogForwarder::getInstance()
{
    static LogForwarder instance;
    return instance;
}

yarp::os::impl::LogForwarder::~LogForwarder() = default;

yarp::os::impl::LogForwarder::LogForwarder()
{
    char hostname[HOST_NAME_MAX];
    yarp::os::gethostname(hostname, HOST_NAME_MAX);

    yarp::os::SystemInfo::ProcessInfo processInfo = yarp::os::SystemInfo::getProcessInfo();

    outputPort.setWriteOnly();
    std::string logPortName = "/log/" + std::string(hostname) + "/" + processInfo.name.substr(processInfo.name.find_last_of("\\/") + 1) + "/" + std::to_string(processInfo.pid);
    if (!outputPort.open(logPortName)) {
        printf("LogForwarder error while opening port %s\n", logPortName.c_str());
    }
    outputPort.enableBackgroundWrite(true);
    outputPort.addOutput("/yarplogger", "fast_tcp");

    started = true;
}

void yarp::os::impl::LogForwarder::forward(const std::string& message)
{
    mutex.lock();
    static Bottle b;
    b.clear();
    std::string port = "[" + outputPort.getName() + "]";
    b.addString(port);
    b.addString(message);
    outputPort.write(b);
    mutex.unlock();
}

void yarp::os::impl::LogForwarder::shutdown()
{
    if (started) {
        std::ostringstream ost;
        auto systemtime = yarp::os::SystemClock::nowSystem();
        auto networktime = (!yarp::os::NetworkBase::isNetworkInitialized() ? 0.0 : (yarp::os::Time::isSystemClock() ? systemtime : yarp::os::Time::now()));

        ost << "(level INFO)";
        ost << " (systemtime " << yarp::conf::numeric::to_string(systemtime)  << ")";
        ost << " (networktime " << yarp::conf::numeric::to_string(networktime)  << ")";

        yarp::os::impl::LogForwarder& fw = getInstance();
        fw.forward(ost.str());
        while (fw.outputPort.isWriting()) {
            yarp::os::SystemClock::delaySystem(0.2);
        }
        fw.outputPort.interrupt();
        fw.outputPort.close();
    }
}

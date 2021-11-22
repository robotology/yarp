/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/LogForwarder.h>

#include <yarp/os/Log.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/SystemInfo.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/PlatformLimits.h>

#include <yarp/conf/environment.h>

#include <sstream>

bool yarp::os::impl::LogForwarder::started{false};

yarp::os::impl::ThreadedPort::ThreadedPort() : yarp::os::PeriodicThread(0.005)
{
    this->start();
}

void yarp::os::impl::ThreadedPort::ThreadedPort::run()
{
    size_t size=0;
    do
    {
        mut.lock();
        size = messages.size();
        mut.unlock();
        if (size > 0 && m_port) { process(); }
        else {break;}
    } while (1);
}

void yarp::os::impl::ThreadedPort::process()
{
    yarp::os::Bottle& b = m_port->prepare();
    mut.lock();
    b = messages.back();
    messages.pop_back();
    mut.unlock();
    m_port->write(true);
}

void yarp::os::impl::ThreadedPort::attach(yarp::os::BufferedPort<yarp::os::Bottle>* port)
{
    m_port = port;
}

void yarp::os::impl::ThreadedPort::terminate()
{
    this->stop(); // blocking call, wait for the thread to finish
    m_port = nullptr;
}

void yarp::os::impl::ThreadedPort::insert(const yarp::os::Bottle& bot)
{
#if 0
    //if this is enabled, the bottle is sent immediately
    yarp::os::Bottle& b = m_port->prepare();
    mut.lock();
    b = bot;
    mut.unlock();
    m_port->write(true);
#else
    //if this is enabled, the bottle is sent in a queue and the thread will send it later
    mut.lock();
    messages.push_front(bot);
    mut.unlock();
#endif
}

///---------------------------------------------------------------------

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
    std::string proc_label = yarp::conf::environment::get_string("YARP_LOG_PROCESS_LABEL");

    outputPort.setWriteOnly();
    std::string logPortName = "/log/" + std::string(hostname) +
                              "/" + processInfo.name.substr(processInfo.name.find_last_of("\\/") + 1);

    if (proc_label!="") { logPortName += "[" + proc_label + "]"; }

    logPortName += "/" + std::to_string(processInfo.pid);

    if (!outputPort.open(logPortName)) {
        printf("LogForwarder error while opening port %s\n", logPortName.c_str());
    }

    outputPort.addOutput("/yarplogger", "fast_tcp");
    tport.attach(&outputPort);

    started = true;
}

std::string yarp::os::impl::LogForwarder::getLogPortName()
{
    return outputPort.getName();
}

void yarp::os::impl::LogForwarder::forward(const std::string& message)
{
    yarp::os::Bottle b;
    b.clear();
    std::string port = "[" + outputPort.getName() + "]";
    b.addString(port);
    b.addString(message);
    tport.insert(b);
}

void yarp::os::impl::LogForwarder::shutdown()
{
    if (started)
    {
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

        fw.tport.terminate();
        fw.outputPort.interrupt();
        fw.outputPort.close();

        started = false;
    }
}

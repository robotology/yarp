/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>
#include <yarp/os/Contactable.h>
#include <yarp/os/Network.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/Time.h>
#include <yarp/os/Type.h>
#include <yarp/os/impl/PlatformSignal.h>

using yarp::companion::impl::Companion;
using yarp::os::Contactable;
using yarp::os::NetworkBase;
using yarp::os::SystemClock;


namespace {
void print_callback(yarp::os::Log::LogType type,
                    const char* msg,
                    const char* file,
                    const unsigned int line,
                    const char* func,
                    double systemtime,
                    double networktime,
                    double externaltime,
                    const char* comp_name)
{
    YARP_UNUSED(type);
    YARP_UNUSED(file);
    YARP_UNUSED(line);
    YARP_UNUSED(func);
    YARP_UNUSED(systemtime);
    YARP_UNUSED(networktime);
    YARP_UNUSED(externaltime);
    YARP_UNUSED(comp_name);
    static const char* err_str = "[ERROR] ";
    static const char* warn_str = "[WARNING] ";
    static const char* no_str = "";
    printf("%s%s\n",
           ((type == yarp::os::Log::ErrorType) ? err_str : ((type == yarp::os::Log::WarningType) ? warn_str : no_str)),
           msg);
}


void companion_sigint_handler(int sig)
{
    YARP_UNUSED(sig);
    yarp::os::Time::useSystemClock();
    double now = SystemClock::nowSystem();
    static double firstCall = now;
    static bool showedMessage = false;
    static bool unregistered = false;
    if (now-firstCall<2) {
        Contactable *port = Companion::getActivePort();
        if (!showedMessage) {
            showedMessage = true;
            yCInfo(COMPANION, "Interrupting...");
        }
        if (!Companion::getUnregisterName().empty()) {
            if (!unregistered) {
                unregistered = true;
                NetworkBase::unregisterName(Companion::getUnregisterName());
                if (port != nullptr) {
                    NetworkBase::unregisterName(port->getName());
                }
                std::exit(1);
            }
        }
        if (port != nullptr) {
            port->interrupt();
        }
    } else {
        yCFatal(COMPANION, "Aborting");
    }
}

void companion_sigterm_handler(int sig)
{
    companion_sigint_handler(sig);
}

#if defined(_WIN32)
void companion_sigbreak_handler(int signum)
{
    YARP_UNUSED(signum);
    yarp::os::impl::raise(SIGINT);
}
#else
void companion_sighup_handler(int signum)
{
    YARP_UNUSED(signum);
    yarp::os::impl::raise(SIGINT);
}
#endif

} // namespace


YARP_LOG_COMPONENT(COMPANION,
                   "yarp.companion.impl.Companion",
                   yarp::os::Log::InfoType,
                   yarp::os::Log::LogTypeReserved,
                   print_callback,
                   nullptr)


Companion& Companion::getInstance()
{
    static Companion instance;
    return instance;
}


void yarp::companion::impl::Companion::installHandler()
{
    yarp::os::impl::signal(SIGINT, companion_sigint_handler);
    yarp::os::impl::signal(SIGTERM, companion_sigterm_handler);

    #if defined(_WIN32)
    yarp::os::impl::signal(SIGBREAK, companion_sigbreak_handler);
    #else
    yarp::os::impl::signal(SIGHUP, companion_sighup_handler);
    #endif
}


Companion::Companion() :
    adminMode(false),
    waitConnect(false)
{
    add("check",           &Companion::cmdCheck,          "run a simple sanity check to see if yarp is working");
    add("clean",           &Companion::cmdClean,          "try to remove inactive entries from the name server");
    add("clock",           &Companion::cmdClock,          "creates a server publishing the system time");
    add("cmake",           &Companion::cmdCMake,          "create files to help compiling YARP projects");
    add("conf",            &Companion::cmdConf,           "report location of configuration file, and optionally fix it");
    add("connect",         &Companion::cmdConnect,        "create a connection between two ports");
    add("detect",          &Companion::cmdDetect,         "search for the yarp name server");
    add("disconnect",      &Companion::cmdDisconnect,     "remove a connection between two ports");
    add("env",             &Companion::cmdEnv,            "print the value of environment variables");
    add("exists",          &Companion::cmdExists,         "check if a port or connection is alive");
    add("help",            &Companion::cmdHelp,           "get this list");
    add("latency-test",    &Companion::cmdLatencyTest,    "perform a latency test by exchanging data between a server and a client");
    add("merge",           &Companion::cmdMerge,          "concatenate input from several ports into a single unit");
    add("name",            &Companion::cmdName,           "send commands to the yarp name server");
    add("namespace",       &Companion::cmdNamespace,      "set or query the name of the yarp name server (default is /root)");
    add("ping",            &Companion::cmdPing,           "get live information about a port");
    add("plugin",          &Companion::cmdPlugin,         "check properties of a YARP plugin (device/carrier)");
    add("priority-qos",    &Companion::cmdPriorityQos,    "set/get the packet priority for a given connection");
    add("read",            &Companion::cmdRead,           "read from the network and print to standard output");
    add("readwrite",       &Companion::cmdReadWrite,      "read from the network and print to standard output, write to the network from standard input");
    add("repeat",          &Companion::cmdRepeat,         "repeats on the output port the same data received in the input port");
    add("resource",        &Companion::cmdResource,       "locates resource files (see ResourceFinder class)");
    add("rpc",             &Companion::cmdRpc,            "write commands to a port, and read replies");
    add("rpcserver",       &Companion::cmdRpcServer,      "make a test RPC server to receive and reply to Bottle-format messages");
    add("sample",          &Companion::cmdSample,         "drop or duplicate messages to achieve a constant frame-rate");
    add("stats",           &Companion::cmdStats,          "print statics about the data received from a specific port");
    add("priority-sched",  &Companion::cmdPrioritySched,  "set/get the thread policy and priority for a given connection");
    add("terminate",       &Companion::cmdTerminate,      "terminate a yarp-terminate-aware process by name");
    add("time",            &Companion::cmdTime,           "show the time");
    add("topic",           &Companion::cmdTopic,          "set a topic name");
    add("trafficgen",      &Companion::cmdTrafficGen,     "generates and streams some test data traffic on a port");
    add("version",         &Companion::cmdVersion,        "get version information");
    add("wait",            &Companion::cmdWait,           "wait for a port to be alive");
    add("where",           &Companion::cmdWhere,          "report where the yarp name server is running");
    add("write",           &Companion::cmdWrite,          "write to the network from standard input");

    // Aliases to other commands (Not shown on the help)
    add("--help",          &Companion::cmdHelp,           "");
    add("-h",              &Companion::cmdHelp,           "");
    add("qos",             &Companion::cmdPriorityQos,    "");
    add("sched",           &Companion::cmdPrioritySched,  "");
    add("--version",       &Companion::cmdVersion,        "");
}

void Companion::setAdminMode(bool admin)
{
    adminMode = admin;
}

void Companion::add(const char* name,
                    int (Companion::*fn)(int argc, char* argv[]),
                    const char* tip = nullptr)
{
    Entry e(name, fn);
    action[std::string(name)] = e;
    // maintain a record of order of keys
    names.emplace_back(name);
    if (tip != nullptr) {
        tips.emplace_back(tip);
    } else {
        tips.emplace_back("");
    }
}

int Companion::dispatch(const char* name, int argc, char* argv[])
{
    // new logic to handle some global arguments
    char** argv_copy = new char*[argc];
    char** argv_copy_org = argv_copy;
    int argc_copy = argc;
    if (!argv_copy) {
        yCError(COMPANION, "Could not copy argument list");
        return 1;
    }
    int at = 0;
    int skip = 0;
    for (int i = 0; i < argc; i++) {
        if (skip > 0) {
            skip--;
            continue;
        }
        std::string arg = argv[i];
        if (arg.find("--") == 0) {
            if (i + 1 < argc) {
                if (arg == "--type") {
                    skip = 1;
                    argType = argv[i + 1];
                    continue;
                }
            }
            if (arg == "--wait-connect") {
                skip = 0;
                waitConnect = true;
                continue;
            }
        }
        argv_copy[at] = argv[i];
        at++;
    }
    argc_copy = at;

    int v = -1;
    auto it = action.find(std::string(name));
    if (it != action.end()) {
        v = (this->*(it->second.fn))(argc_copy, argv_copy);
    } else {
        yCError(COMPANION, "Could not find command \"%s\"", name);
    }
    delete[] argv_copy_org;
    return v;
}


void Companion::applyArgs(yarp::os::Contactable& port)
{
    if (!argType.empty()) {
        port.promiseType(yarp::os::Type::byNameOnWire(argType.c_str()));
    }
}


std::string Companion::printTable( std::map<int,std::string> inputTable,int index)
{
    std::string toReturn = "\n";
    if(index==0) {
        for (auto const& line : inputTable) {
            toReturn += line.second+"\n";
        }
    } else {
        toReturn += inputTable[0]+"\n"+inputTable[index]+"\n";
    }

    return toReturn;
}

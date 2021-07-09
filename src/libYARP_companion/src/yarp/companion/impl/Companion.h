/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_COMPANION_IMPL_COMPANION_H
#define YARP_COMPANION_IMPL_COMPANION_H

#include <yarp/companion/api.h>
#include <yarp/os/PortWriter.h>
#include <yarp/os/ContactStyle.h>
#include <yarp/os/Contactable.h>
#include <yarp/os/LogComponent.h>

#include <map>
#include <string>
#include <vector>
#include <cstdio>

YARP_DECLARE_LOG_COMPONENT(COMPANION);

namespace yarp {
namespace companion {
namespace impl {

/**
 * Implementation of a standard set of YARP utilities.
 */
class YARP_companion_API Companion
{
public:
    static Companion& getInstance();

    void setAdminMode(bool admin);

    int dispatch(const char *name, int argc, char *argv[]);


    // Defined in Companion.cmdCheck.cpp
    int cmdCheck(int argc, char *argv[]);

    // Defined in Companion.cmdClean.cpp
    int cmdClean(int argc, char *argv[]);

    // Defined in Companion.cmdClock.cpp
    int cmdClock(int argc, char *argv[]);

    // Defined in Companion.cmdCMake.cpp
    int cmdCMake(int argc, char *argv[]);

    // Defined in Companion.cmdConf.cpp
    int cmdConf(int argc, char *argv[]);

    // Defined in Companion.cmdConnect.cpp
    static int connect(const char *src, const char *dest, bool silent = false);
    static int subscribe(const char *src, const char *dest, const char *mode = nullptr);
    int cmdConnect(int argc, char *argv[]);

    // Defined in Companion.cmdDisconnect.cpp
    static int disconnect(const char *src, const char *dest, bool silent = false);
    static int unsubscribe(const char *src, const char *dest);
    int cmdDisconnect(int argc, char *argv[]);

    // Defined in Companion.cmdDetect.cpp
    int detectRos(bool write);
    int cmdDetect(int argc, char *argv[]);

    // Defined in Companion.cmdEnv.cpp
    int cmdEnv(int argc, char *argv[]);

    // Defined in Companion.cmdExists.cpp
    int cmdExists(int argc, char *argv[]);

    // Defined in Companion.cmdHelp.cpp
    int cmdHelp(int argc, char *argv[]);

    // Defined in Companion.cmdLatencyTest.cpp
    int cmdLatencyTest(int argc, char* argv[]);

    // Defined in Companion.cmdMerge.cpp
    int cmdMerge(int argc, char *argv[]);

    // Defined in Companion.cmdName.cpp
    int cmdName(int argc, char *argv[]);

    // Defined in Companion.cmdNamespace.cpp
    int cmdNamespace(int argc, char *argv[]);

    // Defined in Companion.cmdPing.cpp
    int ping(const char *port, bool quiet);
    int cmdPing(int argc, char *argv[]);

    // Defined in Companion.cmdPlugin.cpp
    int cmdPlugin(int argc, char *argv[]);

    // Defined in Companion.cmdPray.cpp
    int cmdPray(int argc, char *argv[]);

    // Defined in Companion.cmdPriorityQos.cpp
    int cmdPriorityQos(int argc, char *argv[]);

    // Defined in Companion.cmdPrioritySched.cpp
    int cmdPrioritySched(int argc, char *argv[]);

    // Defined in Companion.cmdRead.cpp
    int read(const char *name, const char *src = nullptr, bool showEnvelope = false, int trim = -1);
    int cmdRead(int argc, char *argv[]);

    // Defined in Companion.cmdReadWrite.cpp
    int cmdReadWrite(int argc, char *argv[]);

    // Defined in Companion.cmdRepeat.cpp
    int cmdRepeat(int argc, char* argv[]);

    // Defined in Companion.cmdResource.cpp
    int cmdResource(int argc, char *argv[]);

    // Defined in Companion.cmdRpc.cpp
    int rpc(const char *connectionName, const char *targetName);
    int rpcClient(int argc, char *argv[]);
    int cmdRpc(int argc, char *argv[]);

    // Defined in Companion.cmdRpcServer.cpp
    int cmdRpcServer(int argc, char *argv[]);

    // Defined in Companion.cmdSample.cpp
    int cmdSample(int argc, char *argv[]);

    // Defined in Companion.cmdStats.cpp
    int cmdStats(int argc, char* argv[]);

    // Defined in Companion.cmdTerminate.cpp
    int cmdTerminate(int argc, char *argv[]);

    // Defined in Companion.cmdTime.cpp
    int cmdTime(int argc, char *argv[]);

    // Defined in Companion.cmdTopic.cpp
    int cmdTopic(int argc, char *argv[]);

    // Defined in Companion.cmdTrafficGen.cpp
    int cmdTrafficGen(int argc, char* argv[]);

    // Defined in Companion.cmdVersion.cpp
    static std::string version();
    int cmdVersion(int argc, char *argv[]);

    // Defined in Companion.cmdWait.cpp
    int cmdWait(int argc, char *argv[]);

    // Defined in Companion.cmdWhere.cpp
    int cmdWhere(int argc, char *argv[]);

    // Defined in Companion.cmdWrite.cpp
    int write(const char *name, int ntargets, char *targets[]);
    int cmdWrite(int argc, char *argv[]);

    static yarp::os::Contactable* getActivePort() { return getInstance().active_port; }
    static void setActivePort(yarp::os::Contactable* port) { getInstance().active_port = port; }

    static std::string getUnregisterName() { return getInstance().unregister_name; }
    static void setUnregisterName(const std::string& name) { getInstance().unregister_name = name; }

private:
    Companion();

    void add(const char* name,
             int (Companion::*fn)(int argc, char* argv[]),
             const char* tip);

    void applyArgs(yarp::os::Contactable& port);

    static std::string printTable(std::map<int,std::string> inputTable,int index);

    static void installHandler();

    class Entry {
    public:
        std::string name;
        int (Companion::*fn)(int argc, char *argv[]);

        Entry(const char *name, int (Companion::*fn)(int argc, char *argv[])) :
            name(name),
            fn(fn)
        {}

        Entry() :
            fn(nullptr)
        {}
    };

    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARGS(std::map<std::string, Entry>) action;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::vector<std::string>) names;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::vector<std::string>) tips;
    bool adminMode;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) argType;
    bool waitConnect;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) unregister_name;
    yarp::os::Contactable* active_port {nullptr};

};

} // namespace impl
} // namespace companion
} // namespace yarp


#endif // YARP_COMPANION_IMPL_COMPANION_H

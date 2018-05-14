/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_COMPANION_H
#define YARP_OS_IMPL_COMPANION_H

#include <string>
#include <yarp/os/PortWriter.h>
#include <yarp/os/ContactStyle.h>
#include <yarp/os/Contactable.h>

#include <map>
#include <vector>
#include <cstdio>

// ACE headers may fiddle with main
#ifdef main
#undef main
#endif

namespace yarp {
    namespace os {
        namespace impl {
            class Companion;
        }
    }
}

/**
 * Implementation of a standard set of YARP utilities.
 */
class YARP_OS_impl_API yarp::os::impl::Companion {
public:

    static std::string version();

    /**
     * The standard main method for the YARP companion utility.
     * @param argc Argument count
     * @param argv Command line arguments
     * @return 0 on success, non-zero on failure
     */

    static int main(int argc, char *argv[]);

    /**
     * Request that an output port connect to an input port.
     * @param src the name of an output port
     * @param dest the name of an input port
     * @param silent whether to print comments on the result
     * @return 0 on success, non-zero on failure
     */
    static int connect(const char *src, const char *dest,
                       bool silent = false);

    /**
     * Request that an output port disconnect from an input port.
     * @param src the name of an output port
     * @param dest the name of an input port
     * @param silent whether to print comments on the result
     * @return 0 on success, non-zero on failure
     */
    static int disconnect(const char *src, const char *dest,
                          bool silent = false);

    static int disconnectInput(const char *src, const char *dest,
                               bool silent = false);

    static int poll(const char *target, bool silent = false);

    static int wait(const char *target, bool silent = false,
                    const char *target2 = nullptr);

    static int exists(const char *target, bool silent = false) {
        ContactStyle style;
        style.quiet = silent;
        return exists(target, style);
    }

    static int exists(const char *target, const ContactStyle& style);

    /**
     * Create a port to read Bottles and prints them to standard input.
     * It assumes the Bottles consist of an integer followed by a string.
     * The integer indicates whether the "end-of-file" has been reached.
     * The string is what gets printed.
     * @param name the name which which to register the port
     * @param src name of a port to connect from, if any
     * @param showEnvelope set to true if you want envelope information
     * shown
     * @return 0 on success, non-zero on failure
     */
    int read(const char *name, const char *src = nullptr,
             bool showEnvelope = false);

    int write(const char *name, int ntargets, char *targets[]);

    int rpc(const char *connectionName, const char *targetName);

    static int forward(const char *localName, const char *targetName);

    static std::string slashify(const std::string& src) {
        if (src.length()>0) {
            if (src[0] == '/') {
                return src;
            }
        }
        return std::string("/") + src;
    }

    /**
     * Read a line of arbitrary length from standard input.
     */
    static std::string readString(bool *eof=nullptr);


    static int sendMessage(const std::string& port, yarp::os::PortWriter& writable,
                           bool silent = false) {
        std::string output;
        return sendMessage(port, writable, output, silent);
    }

    static int sendMessage(const std::string& port, yarp::os::PortWriter& writable,
                           std::string& output,
                           bool quiet);


    static Companion& getInstance() {
        return instance;
    }

    int dispatch(const char *name, int argc, char *argv[]);

    int cmdVersion(int argc, char *argv[]);

    int cmdTerminate(int argc, char *argv[]);

    int cmdName(int argc, char *argv[]);

    int cmdWhere(int argc, char *argv[]);

    int cmdConf(int argc, char *argv[]);

    int cmdHelp(int argc, char *argv[]);

    int cmdConnect(int argc, char *argv[]);

    int cmdDisconnect(int argc, char *argv[]);

    int cmdRead(int argc, char *argv[]);

    int cmdWrite(int argc, char *argv[]);

    int cmdReadWrite(int argc, char *argv[]);

    int cmdRpc(int argc, char *argv[]);

    int cmdRpc2(int argc, char *argv[]);

    int cmdRpcServer(int argc, char *argv[]);

    int cmdRegression(int argc, char *argv[]);

    int cmdCheck(int argc, char *argv[]);

    int cmdPing(int argc, char *argv[]);

    int cmdExists(int argc, char *argv[]);

    int cmdWait(int argc, char *argv[]);

    int cmdMake(int argc, char *argv[]);

    int cmdNamespace(int argc, char *argv[]);

    int cmdClean(int argc, char *argv[]);

    int cmdResource(int argc, char *argv[]);

    int cmdDetect(int argc, char *argv[]);

    int cmdDetectRos(bool write);

    int cmdTopic(int argc, char *argv[]);

    int cmdPlugin(int argc, char *argv[]);

    int cmdMerge(int argc, char *argv[]);

    int cmdSample(int argc, char *argv[]);

    int subscribe(const char *src,
                  const char *dest,
                  const char *mode = nullptr);

    int unsubscribe(const char *src, const char *dest);

    int ping(const char *port, bool quiet);

    int cmdTime(int argc, char *argv[]);

    int cmdClock(int argc, char *argv[]);

private:

    Companion();

    static Companion instance;

    void applyArgs(yarp::os::Contactable& port);

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

    std::map<std::string, Entry> action;
    std::vector<std::string> names;
    std::vector<std::string> tips;
    bool adminMode;
    std::string argType;
    bool waitConnect;

    void add(const char* name,
             int (Companion::*fn)(int argc, char* argv[]),
             const char* tip);
};

#endif // YARP_OS_IMPL_COMPANION_H

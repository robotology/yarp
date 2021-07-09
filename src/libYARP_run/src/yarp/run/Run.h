/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_RUN_RUN_H
#define YARP_RUN_RUN_H

#include <cstring>
#include <yarp/run/api.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/Property.h>


class YarpRunInfoVector;
class ZombieHunterThread;


/*
 * Typical YARP applications consist of several intercommunicating modules distributed on different machines.
 * If a yarprun server is running on each machine, distributed applications can be remotely launched,
 * monitored and terminated by yarprun commands.
 *
 * - To run a yarprun server on a machine:
 *      $ yarprun --server /SERVERPORT
 *
 * /SERVERPORT must be unique and identifies the remote machine.
 *
 * - The basic command to run a command/application on a remote machine is:
 *      $ yarprun --on /SERVERPORT --as TAG --cmd COMMAND [ARGLIST]
 *
 * /SERVERPORT is the name of the server that actually runs the command
 * TAG identifies the application process set, and must be unique
 * COMMAND is the application that has to be executed, followed by the optional argument list
 *
 * Some options can be added to the basic format of yarprun:
 *      $ yarprun --on /SERVERPORT1 --as TAG --cmd COMMAND [ARGLIST] --stdio /SERVERPORT2
 *
 * opens a remote shell window where the stdin, stdout and stderr of the application will be redirected.
 * /SERVERPORT2 specifies the machine where the IO shell will be executed, and can be either a remote machine or
 * be equal to /SERVERPORT1 itself.
 *
 * If --stdio is specified, there are two useful sub-options (linux only):
 * - --hold keep the stdio window open even if the command is terminated or aborted.
 * - --geometry WxH+X+Y set the stdio window size and position. Example: --geometry 320x240+80+20
 *
 * Other yarprun commands:
 *
 * - To terminate an application, the yarprun syntax is:
 *      $ yarprun --on /SERVERPORT --sigterm TAG
 *
 * - To send a signal to an application (usually SIGKILL) use:
 *      $ yarprun --on /SERVERPORT --kill TAG SIGNUM
 *
 * - To terminate all the applications managed by a yarprun server, use:
 *      $ yarprun --on /SERVERPORT --sigtermall
 *
 * - To check if an application is still running on a yarprun server, use:
 *      $ yarprun --on /SERVERPORT --isrunning TAG
 *
 * - To get a report of applications running on a yarprun server, use:
 *      $ yarprun --on /SERVERPORT --ps
 *
 * - To shutdown a yarprun server, use:
 *      $ yarprun --on /SERVERPORT --exit
 *
 */

namespace yarp {
namespace run {

/**
 * \class yarp::os::Run
 * \brief yarprun provides the APIs to a client-server environment that is able to run,
 * kill and monitor applications commands on a remote machin in Windows and Linux.
 */
class YARP_run_API Run
{
public:
    // API

    /**
     * Launch a yarprun server.
     * @param node is the yarprun server port name. It must be unique in the network.
     * @param command is the command to be executed by the remote server. It can include
     * an argument list and different options, in the standard yarp Property key/value mode:
     * - name COMMAND_NAME
     * - parameters ARGUMENT_LIST (optional)
     * - stdio /SERVERPORT (optional)
     * - geometry WxH+X+Y (optional)
     * - hold (optional)
     * @param keyv is the tag that will identify the running application. It must be unique in the network.
     * @return 0=success -1=failed.
     */
    static int start(const std::string &node, yarp::os::Property &command, std::string &keyv);
    /**
     * Terminate an application running on a yarprun server.
     * @param node is the yarprun server port name. It must be unique in the network.
     * @param keyv is the tag that identifies the running application. It must be unique in the network.
     * @return 0=success -1=failed.
     */
    static int sigterm(const std::string &node, const std::string &keyv);
    /**
     * Terminate all applications running on a yarprun server.
     * @param node is the yarprun server port name. It must be unique in the network.
     * @return 0=success -1=failed.
     */
    static int sigterm(const std::string &node);
    /**
     * Send a SIGNAL to an application running on a yarprun server (Linux only).
     * @param node is the yarprun server port name. It must be unique in the network.
     * @param keyv is the tag that identifies the running application. It must be unique in the network.
     * @param s is the SIGNAL number.
     * @return 0=success -1=failed.
     */
    static int kill(const std::string &node, const std::string &keyv, int s);
    /**
     * Get a report of all applications running on a yarprun server.
     * @param node is the yarprun server port name. It must be unique in the network.
     * @param processes is a list of applications running on the remote yarprun server. It must not be allocated
     * and it is responsibility of the caller to delete it.
     * @param num_processes return the number of running processes.
     * @return 0=success -1=failed.
     */
    // static int ps(const std::string &node, std::string** &processes, int &num_processes);
    /**
     * Report if an application is still running on a yarprun server.
     * @param node is the yarprun server port name. It must be unique in the network.
     * @param keyv is the tag that identifies the application. It must be unique in the network.
     * @return true=running false=terminated.
     */
    static bool isRunning(const std::string &node, std::string &keyv);

    // end API

    /**
     *
     * Send a property object to a run server, bundling up all the
     * settings usually specified on the command line.  See the
     * documentation for the "yarprun" command.
     *
     * @param config the property object to send.
     *
     * @return 0 on success, -1 on failure
     *
     */
    static int client(yarp::os::Property& config);

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    static int main(int argc, char *argv[]);
    static yarp::os::RpcServer *pServerPort;

    static bool mStresstest;

    static bool mLogged;
YARP_WARNING_PUSH
YARP_DISABLE_DLL_INTERFACE_WARNING
    static std::string mLoggerPort;

#if defined(_WIN32)
    static YarpRunInfoVector mProcessVector;
    static YarpRunInfoVector mStdioVector;
#else
    static YarpRunInfoVector *mProcessVector;
    static YarpRunInfoVector *mStdioVector;
    static ZombieHunterThread *mBraveZombieHunter;
    static void CleanZombie(int pid);
#endif
YARP_WARNING_POP
    static yarp::os::Bottle sendMsg(yarp::os::Bottle& msg, std::string target, int RETRY=20, double DELAY=0.5);

protected:
    static void Help(const char* msg="");
    static int server();
    static int executeCmdAndStdio(yarp::os::Bottle& msg, yarp::os::Bottle& result);
    static int executeCmdStdout(yarp::os::Bottle& msg, yarp::os::Bottle& result, std::string& loggerName);
    static int executeCmd(yarp::os::Bottle& msg, yarp::os::Bottle& result);
    static int userStdio(yarp::os::Bottle& msg, yarp::os::Bottle& result);

    static inline bool IS_PARENT_OF(int pid){ return pid>0; }
    static inline bool IS_NEW_PROCESS(int pid){ return !pid; }
    static inline bool IS_INVALID(int pid){ return pid<0; }

YARP_SUPPRESS_DLL_INTERFACE_WARNING
    static std::string mPortName;
    static int mProcCNT;

#if !defined(_WIN32)
    static void cleanBeforeExec();
    static void writeToPipe(int fd, std::string str);
    static int readFromPipe(int fd, char* &data, int& buffsize);
#endif

    static void cmdcpy(char* &dst, const char* src)
    {
        dst=new char[(strlen(src)/8+2)*16];
        strcpy(dst, src);
    }

    static void cmdclean(char **cmd)
    {
        while (*cmd)
        {
            delete [] *cmd++;
        }
    }

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/
};

} // namespace run
} // namespace yarp


#endif // YARP_RUN_RUN_H

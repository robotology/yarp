/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/run/Run.h>
#include <yarp/run/impl/RunCheckpoints.h>
#include <yarp/run/impl/RunProcManager.h>
#include <yarp/run/impl/RunReadWrite.h>
#include <yarp/run/impl/PlatformStdlib.h>
#include <yarp/run/impl/PlatformUnistd.h>
#include <yarp/run/impl/PlatformSysPrctl.h>

#include <yarp/conf/environment.h>
#include <yarp/conf/filesystem.h>

#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/SystemInfo.h>
#include <yarp/os/SystemInfoSerializer.h>
#include <yarp/os/Time.h>

#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/PlatformSignal.h>
#include <yarp/os/impl/PlatformStdio.h>

#include <cstdio>
#include <string>
#include <cstring>
#include <random>
#include <iomanip>

#if defined(_WIN32)
# if !defined(WIN32_LEAN_AND_MEAN)
#  define WIN32_LEAN_AND_MEAN
# endif
# include <windows.h>
#else
# define C_MAXARGS       128     // the max number of command parameters. rational?
#endif

#if defined(_WIN32)
YarpRunInfoVector yarp::run::Run::mProcessVector;
YarpRunInfoVector yarp::run::Run::mStdioVector;
inline std::string lastError2String()
{
    int error=GetLastError();
    char buff[1024];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error, 0, buff, 1024, nullptr);

    return std::string(buff);
}
#else
//#define SIGSTDIO SIGHUP
YarpRunInfoVector* yarp::run::Run::mProcessVector = nullptr;
YarpRunInfoVector* yarp::run::Run::mStdioVector = nullptr;
ZombieHunterThread* yarp::run::Run::mBraveZombieHunter = nullptr;
#endif

///////////////////////////
// OS INDEPENDENT FUNCTIONS
///////////////////////////

std::string yarp::run::Run::mPortName;
yarp::os::RpcServer* yarp::run::Run::pServerPort=nullptr;
int yarp::run::Run::mProcCNT=0;
bool yarp::run::Run::mStresstest=false;
bool yarp::run::Run::mLogged=false;
std::string yarp::run::Run::mLoggerPort("/yarplogger");

////////////////////////////////////

static RunTerminator *pTerminator = nullptr;

void sigstdio_handler(int sig)
{
    char msg[16];
    yarp::os::Time::useSystemClock();
    sprintf(msg, "SIGNAL %d", sig);
    RUNLOG(msg);

    if (pTerminator) {
        pTerminator->exit();
    }
}

////////////////////////////////////

static std::string getProcLabel(const yarp::os::Bottle& msg)
{
    auto ss = yarp::conf::string::split(msg.find("env").asString(), ';');
    for (const auto& s_iter : ss)
    {
        auto sss = yarp::conf::string::split(s_iter, '=');
        if (sss.size() == 2 && sss[0] == "YARP_LOG_PROCESS_LABEL")
        {
            return sss[1];
        }
    }
    return "";
}

/////////
int yarp::run::Run::main(int argc, char *argv[])
{
    yarp::os::Property config;
    config.fromCommand(argc, argv, false);

    // SERVER
    if (config.check("server"))
    {
        mLogged=config.check("log");

        if (mLogged)
        {
            yarp::os::Bottle botPortLogger=config.findGroup("log");

            if (botPortLogger.size()>1)
            {
                mLoggerPort=botPortLogger.get(1).asString();
            }
        }

        mPortName=std::string(config.find("server").asString());
        return server();
    }

    mPortName="";

    if (!yarp::os::Network::getLocalMode())
    {
        if (!yarp::os::Network::checkNetwork())
        {
            fprintf(stderr, "ERROR: no yarp network found.\n");

            return YARPRUN_ERROR;
        }
    }

    // READWRITE
    if (config.check("readwrite"))
    {
        std::string uuid=config.findGroup("readwrite").get(1).asString();
        std::string fPortName;
        std::string lPortName;

        if (config.check("forward"))
        {
            fPortName=config.findGroup("forward").get(1).asString();
            lPortName=config.findGroup("forward").get(2).asString();
        }

#if defined(_WIN32)
        yarp::os::impl::signal(SIGINT, sigstdio_handler);
        yarp::os::impl::signal(SIGTERM, sigstdio_handler);
        yarp::os::impl::signal(SIGBREAK, sigstdio_handler);
#elif defined(__APPLE__)
        //prctl(PR_SET_PDEATHSIG, SIGTERM);

        struct sigaction new_action;
        new_action.sa_handler=sigstdio_handler;
        sigfillset(&new_action.sa_mask);
        new_action.sa_flags=0;

        sigaction(SIGTERM, &new_action, nullptr);
        sigaction(SIGHUP, &new_action, nullptr);
        //yarp::os::impl::signal(SIGHUP, SIG_IGN);
        //yarp::os::impl::signal(SIGINT, SIG_IGN);
        yarp::os::impl::signal(SIGPIPE, SIG_IGN);

        if (getppid()==1) return 0;
#else
        yarp::run::impl::prctl(PR_SET_PDEATHSIG, SIGTERM);

        struct sigaction new_action;
        new_action.sa_handler=sigstdio_handler;
        yarp::os::impl::sigfillset(&new_action.sa_mask);
        new_action.sa_flags=0;

        yarp::os::impl::sigaction(SIGTERM, &new_action, nullptr);
        yarp::os::impl::signal(SIGHUP, SIG_IGN);
        //yarp::os::impl::signal(SIGINT, SIG_IGN);
        yarp::os::impl::signal(SIGPIPE, SIG_IGN);

        if (yarp::os::getpid() == 1) {
            return 0;
        }
#endif

        RunReadWrite rw(uuid, fPortName, lPortName);
        RunTerminator rt(&rw);
        pTerminator=&rt;
        rt.start();

        return rw.loop();
    }

    // WRITE
    if (config.check("write"))
    {
        std::string portName=config.findGroup("write").get(1).asString();

#if defined(_WIN32)
        yarp::os::impl::signal(SIGINT,  sigstdio_handler);
        yarp::os::impl::signal(SIGTERM, sigstdio_handler);
        yarp::os::impl::signal(SIGBREAK, sigstdio_handler);
#else
        struct sigaction new_action;
        new_action.sa_handler=sigstdio_handler;
        yarp::os::impl::sigfillset(&new_action.sa_mask);
        new_action.sa_flags=0;
        yarp::os::impl::sigaction(SIGTERM, &new_action, nullptr);
        //yarp::os::impl::signal(SIGINT,  SIG_IGN);
        yarp::os::impl::signal(SIGPIPE, SIG_IGN);
        yarp::os::impl::signal(SIGHUP,  SIG_IGN);
#endif

        if (config.check("log"))
        {
            std::string loggerName=config.find("log").asString();
            RunWrite w(portName, loggerName);
            RunTerminator rt(&w);
            pTerminator=&rt;
            rt.start();
            return w.loop();
        }
        else
        {
            RunWrite w(portName);
            RunTerminator rt(&w);
            pTerminator=&rt;
            rt.start();
            return w.loop();
        }

        return 0;
    }

    // READ
    if (config.check("read"))
    {
        std::string uuid=config.findGroup("read").get(1).asString();

        #if defined(_WIN32)
        yarp::os::impl::signal(SIGINT,  sigstdio_handler);
        yarp::os::impl::signal(SIGTERM, sigstdio_handler);
        yarp::os::impl::signal(SIGBREAK, sigstdio_handler);
        #else
        //yarp::os::impl::signal(SIGINT, SIG_IGN);
        yarp::os::impl::signal(SIGTERM, sigstdio_handler);
        yarp::os::impl::signal(SIGHUP, SIG_IGN);
        #endif

        RunRead r(uuid);
        RunTerminator rt(&r);
        pTerminator=&rt;
        rt.start();

        return r.loop();
    }

    // STRESSTEST
    if (config.check("stresstest"))
    {
        fprintf(stderr, "Yarprun stress test started.\n");
        fflush(stderr);

        int max_interval_ms=config.find("stresstest").asInt32();
        std::string tag_zero=config.find("as").asString();
        yarp::os::Bottle srv=config.findGroup("on");

        config.unput("as");
        config.unput("stresstest");

        std::string cmd;

        bool isCommand=false;

        if (config.check("cmd"))
        {
            isCommand=true;
            cmd=config.find("cmd").asString();
            config.unput("cmd");
        }

        unsigned int t=0, u=0;
        int term_cycle=0;

        char tag[256];
        char cmd_and_name[512];

        mStresstest=true;

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist0maxint(0, max_interval_ms -1);

        while (mStresstest)
        {
            yarp::os::SystemClock::delaySystem(0.001*(dist0maxint(mt)));

            yarp::os::Property stresser=config;

            sprintf(tag, "%s_%u", tag_zero.c_str(), t++);
            stresser.put("as", tag);

            if (isCommand)
            {
                sprintf(cmd_and_name, "%s --name /%s", cmd.c_str(), tag);
                stresser.put("cmd", cmd_and_name);
            }

            client(stresser);

            std::uniform_int_distribution<int> dist07(0, 7);
            if (isCommand && ++term_cycle>=4)
            {
                term_cycle=0;

                int r = t - (dist07(mt));

                for (int i=u; i<r; ++i)
                {
                    sprintf(tag, "%s_%u", tag_zero.c_str(), i);

                    yarp::os::Bottle as;
                    as.addString("sigterm");
                    as.addString(tag);

                    yarp::os::Bottle term;
                    term.addList()=srv;
                    term.addList()=as;

                    sendMsg(term, srv.get(1).asString());

                    ++u;
                }
            }
        }

        return 0;
    }

    // HELP
    if (config.check("help"))
    {
        Help();

        return 0;
    }

    // CLIENT (config is from keyboard)
    if (config.check("stdio")
     || config.check("cmd")
     || config.check("kill")
     || config.check("sigterm")
     || config.check("sigtermall")
     || config.check("exit")
     || config.check("isrunning")
     || config.check("ps")
     || config.check("env")
     || config.check("sysinfo")
     || config.check("which"))
    {
        int ret=client(config);

        return ret;
    }

    Help();

    return 0;
}

yarp::os::Bottle yarp::run::Run::sendMsg(yarp::os::Bottle& msg, std::string target, int RETRY, double DELAY)
{
    yarp::os::Bottle response;

    for (int r=0; r<RETRY; ++r)
    {
        yarp::os::RpcClient port;

#if 0
        if (!port.open("..."))
#else

        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dist(0, 99999);
        int number = dist(gen);
        std::ostringstream oss;
        oss << std::setw(5) << std::setfill('0') << number;
        std::string tempy = "/yarprunclient/temp" + oss.str();
        if (!port.open(tempy))
#endif
        {
            yarp::os::SystemClock::delaySystem(DELAY);
            fprintf(stderr, "retrying...");
            continue;
        }

        std::string tempportname = port.getName();
        bool bb = yarp::os::Network::isConnected(tempportname, target);

        if (!yarp::os::Network::connect(tempportname, target, "fast_tcp", false))
        {
            port.close();
            yarp::os::SystemClock::delaySystem(DELAY);
            fprintf(stderr, "retrying...");
            continue;
        }

        RUNLOG("<<<port.write(msg, response)")
        if (!port.write(msg, response))
        {
            port.close();
            yarp::os::SystemClock::delaySystem(DELAY);
            fprintf(stderr, "retrying...");
            continue;
        }
        RUNLOG(">>>port.write(msg, response)")

        port.close();

        fprintf(stderr, "RESPONSE:\n=========\n");
        for (size_t s=0; s<response.size(); ++s)
        {
            fprintf(stderr, "%s\n", response.get(s).toString().c_str());
        }

        return response;
    }

    response.addString("RESPONSE:\n");
    response.addString("=========\n");
    response.addString("Cannot connect to remote server, aborting...\n");
    for (size_t s=0; s<response.size(); ++s)
    {
        fprintf(stderr, "%s\n", response.get(s).toString().c_str());
    }
    return response;
}


///////////////////////////
// WINDOWS SERVER
#if defined(_WIN32)

///////////////////////
#else // LINUX SERVER
///////////////////////

static void sigchld_handler(int sig)
{
    YARP_UNUSED(sig);
    if (yarp::run::Run::mBraveZombieHunter)
    {
        yarp::run::Run::mBraveZombieHunter->sigchldHandler();
    }
}

#endif


/////////////////////////
// OS DEPENDENT FUNCTIONS
/////////////////////////

// WINDOWS

#if defined(_WIN32)

// CMD SERVER
int yarp::run::Run::executeCmdAndStdio(const  yarp::os::Bottle& msg, yarp::os::Bottle& result)
{
    std::string strAlias=msg.find("as").asString();
    std::string strStdio=msg.find("stdio").asString();
    std::string strStdioUUID=msg.find("stdiouuid").asString();
    //std::string strCmdUUID=mPortName+"/"+int2String(GetCurrentProcessId())+"/"+strAlias+"-"+int2String(mProcCNT++);

    // PIPES
    SECURITY_ATTRIBUTES pipe_sec_attr;
    pipe_sec_attr.nLength=sizeof(SECURITY_ATTRIBUTES);
    pipe_sec_attr.bInheritHandle=TRUE;
    pipe_sec_attr.lpSecurityDescriptor = nullptr;
    HANDLE read_from_pipe_stdin_to_cmd, write_to_pipe_stdin_to_cmd;
    CreatePipe(&read_from_pipe_stdin_to_cmd, &write_to_pipe_stdin_to_cmd, &pipe_sec_attr, 0);
    HANDLE read_from_pipe_cmd_to_stdout, write_to_pipe_cmd_to_stdout;
    CreatePipe(&read_from_pipe_cmd_to_stdout, &write_to_pipe_cmd_to_stdout, &pipe_sec_attr, 0);

    // RUN STDOUT
    PROCESS_INFORMATION stdout_process_info;
    ZeroMemory(&stdout_process_info, sizeof(PROCESS_INFORMATION));
    STARTUPINFO stdout_startup_info;
    ZeroMemory(&stdout_startup_info, sizeof(STARTUPINFO));

    stdout_startup_info.cb=sizeof(STARTUPINFO);
    stdout_startup_info.hStdError=GetStdHandle(STD_ERROR_HANDLE);
    stdout_startup_info.hStdOutput=GetStdHandle(STD_OUTPUT_HANDLE);
    stdout_startup_info.hStdInput=read_from_pipe_cmd_to_stdout;
    stdout_startup_info.dwFlags|=STARTF_USESTDHANDLES;

    BOOL bSuccess=CreateProcess(nullptr,  // command name
                                (char*)(std::string("yarprun --write ")+strStdioUUID).c_str(), // command line
                                nullptr,  // process security attributes
                                nullptr,  // primary thread security attributes
                                TRUE,          // handles are inherited
                                CREATE_NEW_PROCESS_GROUP, // creation flags
                                nullptr,  // use parent's environment
                                nullptr,  // use parent's current directory
                                &stdout_startup_info,   // STARTUPINFO pointer
                                &stdout_process_info);  // receives PROCESS_INFORMATION

    if (!bSuccess)
    {
        std::string strError=std::string("ABORTED: server=")+mPortName
                                      +std::string(" alias=")+strAlias
                                      +std::string(" cmd=stdout\n")
                                      +std::string("Can't execute stdout because ")+lastError2String()
                                      +std::string("\n");

        result.addInt32(YARPRUN_ERROR);
        result.addString(strError.c_str());
        fprintf(stderr, "%s", strError.c_str());
        fflush(stderr);

        CloseHandle(write_to_pipe_stdin_to_cmd);
        CloseHandle(read_from_pipe_stdin_to_cmd);
        CloseHandle(write_to_pipe_cmd_to_stdout);
        CloseHandle(read_from_pipe_cmd_to_stdout);

        return YARPRUN_ERROR;
    }

    // RUN STDIN

    PROCESS_INFORMATION stdin_process_info;
    ZeroMemory(&stdin_process_info, sizeof(PROCESS_INFORMATION));
    STARTUPINFO stdin_startup_info;
    ZeroMemory(&stdin_startup_info, sizeof(STARTUPINFO));

    stdin_startup_info.cb=sizeof(STARTUPINFO);
    stdin_startup_info.hStdError=write_to_pipe_stdin_to_cmd;
    stdin_startup_info.hStdOutput=write_to_pipe_stdin_to_cmd;
    stdin_startup_info.hStdInput=GetStdHandle(STD_INPUT_HANDLE);
    stdin_startup_info.dwFlags|=STARTF_USESTDHANDLES;

    bSuccess=CreateProcess(nullptr,  // command name
                           (char*)(std::string("yarprun --read ")+strStdioUUID).c_str(), // command line
                           nullptr,  // process security attributes
                           nullptr,  // primary thread security attributes
                           TRUE,          // handles are inherited
                           CREATE_NEW_PROCESS_GROUP, // creation flags
                           nullptr,  // use parent's environment
                           nullptr,  // use parent's current directory
                           &stdin_startup_info,   // STARTUPINFO pointer
                           &stdin_process_info);  // receives PROCESS_INFORMATION

    if (!bSuccess)
    {
        std::string strError=std::string("ABORTED: server=")+mPortName
                                      +std::string(" alias=")+strAlias
                                      +std::string(" cmd=stdin\n")
                                      +std::string("Can't execute stdin because ")+lastError2String()
                                      +std::string("\n");

        result.addInt32(YARPRUN_ERROR);
        result.addString(strError.c_str());
        fprintf(stderr, "%s", strError.c_str());

        TerminateProcess(stdout_process_info.hProcess, YARPRUN_ERROR);

        CloseHandle(stdout_process_info.hProcess);

        CloseHandle(write_to_pipe_stdin_to_cmd);
        CloseHandle(read_from_pipe_stdin_to_cmd);
        CloseHandle(write_to_pipe_cmd_to_stdout);
        CloseHandle(read_from_pipe_cmd_to_stdout);

        return YARPRUN_ERROR;
    }

    // RUN COMMAND

    PROCESS_INFORMATION cmd_process_info;
    ZeroMemory(&cmd_process_info, sizeof(PROCESS_INFORMATION));
    STARTUPINFO cmd_startup_info;
    ZeroMemory(&cmd_startup_info, sizeof(STARTUPINFO));

    cmd_startup_info.cb=sizeof(STARTUPINFO);
    cmd_startup_info.hStdError=write_to_pipe_cmd_to_stdout;
    cmd_startup_info.hStdOutput=write_to_pipe_cmd_to_stdout;
    cmd_startup_info.hStdInput=read_from_pipe_stdin_to_cmd;
    cmd_startup_info.dwFlags|=STARTF_USESTDHANDLES;

    yarp::os::Bottle botCmd=msg.findGroup("cmd").tail();

    std::string strCmd;
    for (int s=0; s<botCmd.size(); ++s)
    {
        strCmd+=botCmd.get(s).toString()+std::string(" ");
    }

    /*
     * setting environment variable for child process
     */
    TCHAR chNewEnv[32767];

    // Get a pointer to the env block.
    LPTCH chOldEnv = GetEnvironmentStrings();

    // copying parent env variables
    LPTSTR lpOld = (LPTSTR) chOldEnv;
    LPTSTR lpNew = (LPTSTR) chNewEnv;
    while (*lpOld)
    {
        lstrcpy(lpNew, lpOld);
        lpOld += lstrlen(lpOld) + 1;
        lpNew += lstrlen(lpNew) + 1;
    }

    // Set the YARP_IS_YARPRUN environment variable to 1, so that the child
    // process will now that is running inside yarprun.
    lstrcpy(lpNew, (LPTCH) "YARP_IS_YARPRUN=1");
    lpNew += lstrlen(lpNew) + 1;

    // Set the YARPRUN_IS_FORWARDING_LOG environment variable to 1, so that
    // the child process will now that yarprun is not logging the output.
    lstrcpy(lpNew, (LPTCH) "YARPRUN_IS_FORWARDING_LOG=1");
    lpNew += lstrlen(lpNew) + 1;

    // adding new env variables
    std::string cstrEnvName;
    if (msg.check("env"))
    {
        auto ss = yarp::conf::string::split(msg.find("env").asString(), ';');
        for (const auto& s : ss) {
            lstrcpy(lpNew, (LPTCH) s.c_str());
            lpNew += lstrlen(lpNew) + 1;
        }
    }

    // closing env block
    *lpNew = (TCHAR)0;

    bool bWorkdir=msg.check("workdir");
    std::string strWorkdir=bWorkdir?msg.find("workdir").asString()+"\\":"";

    bSuccess=CreateProcess(nullptr,  // command name
                           (char*)(strWorkdir+strCmd).c_str(), // command line
                           nullptr,  // process security attributes
                           nullptr,  // primary thread security attributes
                           TRUE,          // handles are inherited
                           CREATE_NEW_PROCESS_GROUP, // creation flags
                           (LPVOID) chNewEnv,        // use new environment list
                           bWorkdir ? strWorkdir.c_str() : nullptr, // working directory
                           &cmd_startup_info,   // STARTUPINFO pointer
                           &cmd_process_info);  // receives PROCESS_INFORMATION

    if (!bSuccess && bWorkdir)
    {
        bSuccess=CreateProcess(nullptr,  // command name
                               (char*)(strCmd.c_str()), // command line
                               nullptr,  // process security attributes
                               nullptr,  // primary thread security attributes
                               TRUE,          // handles are inherited
                               CREATE_NEW_PROCESS_GROUP, // creation flags
                               (LPVOID) chNewEnv,        // use new environment list
                               strWorkdir.c_str(), // working directory
                               &cmd_startup_info,   // STARTUPINFO pointer
                               &cmd_process_info);  // receives PROCESS_INFORMATION
    }

    // deleting old environment variable
    FreeEnvironmentStrings(chOldEnv);

    if (!bSuccess)
    {
        result.addInt32(YARPRUN_ERROR);

        DWORD nBytes;
        std::string line1=std::string("ABORTED: server=")+mPortName
                                   +std::string(" alias=")+strAlias
                                   +std::string(" cmd=")+strCmd
                                   +std::string("pid=")+int2String(cmd_process_info.dwProcessId)
                                   +std::string("\n");

        WriteFile(write_to_pipe_cmd_to_stdout, line1.c_str(), line1.length(), &nBytes, 0);

        std::string line2=std::string("Can't execute command because ")+lastError2String()+std::string("\n");
        WriteFile(write_to_pipe_cmd_to_stdout, line1.c_str(), line2.length(), &nBytes, 0);
        FlushFileBuffers(write_to_pipe_cmd_to_stdout);

        std::string out=line1+line2;
        result.addString(out.c_str());
        fprintf(stderr, "%s", out.c_str());
        fflush(stderr);

        CloseHandle(write_to_pipe_stdin_to_cmd);
        CloseHandle(read_from_pipe_stdin_to_cmd);
        CloseHandle(write_to_pipe_cmd_to_stdout);
        CloseHandle(read_from_pipe_cmd_to_stdout);

        TerminateProcess(stdout_process_info.hProcess, YARPRUN_ERROR);

        CloseHandle(stdout_process_info.hProcess);

        TerminateProcess(stdin_process_info.hProcess, YARPRUN_ERROR);

        CloseHandle(stdin_process_info.hProcess);

        return YARPRUN_ERROR;
    }

    FlushFileBuffers(write_to_pipe_cmd_to_stdout);

    // EVERYTHING IS ALL RIGHT
    YarpRunCmdWithStdioInfo* pInf = new YarpRunCmdWithStdioInfo(strAlias,
                                                   mPortName,
                                                   strStdio,
                                                   cmd_process_info.dwProcessId,
                                                   strStdioUUID,
                                                   &mStdioVector,
                                                   stdin_process_info.dwProcessId,
                                                   stdout_process_info.dwProcessId,
                                                   read_from_pipe_stdin_to_cmd,
                                                   write_to_pipe_stdin_to_cmd,
                                                   read_from_pipe_cmd_to_stdout,
                                                   write_to_pipe_cmd_to_stdout,
                                                   cmd_process_info.hProcess,
                                                   false);

    pInf->setCmd(strCmd);
    if (msg.check("env"))
    {
        pInf->setEnv(msg.find("env").asString());
    }
    mProcessVector.Add(pInf);

    result.addInt32(cmd_process_info.dwProcessId);
    std::string out=std::string("STARTED: server=")+mPortName
                             +std::string(" alias=")+strAlias
                             +std::string(" cmd=")+strCmd
                             +std::string(" pid=")+int2String(cmd_process_info.dwProcessId)
                             +std::string("\n");

    result.addString(out.c_str());
    result.addString(strStdioUUID.c_str());
    fprintf(stderr, "%s", out.c_str());

    return cmd_process_info.dwProcessId;
}

int yarp::run::Run::executeCmdStdout(const  yarp::os::Bottle& msg, yarp::os::Bottle& result, std::string& loggerName)
{
    std::string proc_label = getProcLabel(msg);

    std::string strAlias=msg.find("as").asString();
    std::string portName="/log";
    portName+=mPortName+"/";
    std::string command = msg.findGroup("cmd").get(1).asString();
    command = command.substr(0, command.find(' '));
    command = command.substr(command.find_last_of("\\/") + 1);
    portName+=command;
    if (proc_label != "") { portName += "[" + proc_label + "]"; }

    // PIPES
    SECURITY_ATTRIBUTES pipe_sec_attr;
    pipe_sec_attr.nLength=sizeof(SECURITY_ATTRIBUTES);
    pipe_sec_attr.bInheritHandle=TRUE;
    pipe_sec_attr.lpSecurityDescriptor = nullptr;
    HANDLE read_from_pipe_cmd_to_stdout, write_to_pipe_cmd_to_stdout;
    CreatePipe(&read_from_pipe_cmd_to_stdout, &write_to_pipe_cmd_to_stdout, &pipe_sec_attr, 0);

    // RUN STDOUT
    PROCESS_INFORMATION stdout_process_info;
    ZeroMemory(&stdout_process_info, sizeof(PROCESS_INFORMATION));
    STARTUPINFO stdout_startup_info;
    ZeroMemory(&stdout_startup_info, sizeof(STARTUPINFO));

    stdout_startup_info.cb=sizeof(STARTUPINFO);
    stdout_startup_info.hStdError=GetStdHandle(STD_ERROR_HANDLE);
    stdout_startup_info.hStdOutput=GetStdHandle(STD_OUTPUT_HANDLE);
    stdout_startup_info.hStdInput=read_from_pipe_cmd_to_stdout;
    stdout_startup_info.dwFlags|=STARTF_USESTDHANDLES;

    BOOL bSuccess=CreateProcess(nullptr,  // command name
                                (char*)(std::string("yarprun --log ")+loggerName+std::string(" --write ")+portName).c_str(), // command line
                                nullptr,  // process security attributes
                                nullptr,  // primary thread security attributes
                                TRUE,          // handles are inherited
                                CREATE_NEW_PROCESS_GROUP, // creation flags
                                nullptr,  // use parent's environment
                                nullptr,  // use parent's current directory
                                &stdout_startup_info,   // STARTUPINFO pointer
                                &stdout_process_info);  // receives PROCESS_INFORMATION

    if (!bSuccess)
    {
        std::string strError=std::string("ABORTED: server=")+mPortName
                                      +std::string(" alias=")+strAlias
                                      +std::string(" cmd=stdout\n")
                                      +std::string("Can't execute stdout because ")+lastError2String()
                                      +std::string("\n");

        result.addInt32(YARPRUN_ERROR);
        result.addString(strError.c_str());
        fprintf(stderr, "%s", strError.c_str());
        fflush(stderr);

        CloseHandle(write_to_pipe_cmd_to_stdout);
        CloseHandle(read_from_pipe_cmd_to_stdout);

        return YARPRUN_ERROR;
    }

    // RUN COMMAND

    PROCESS_INFORMATION cmd_process_info;
    ZeroMemory(&cmd_process_info, sizeof(PROCESS_INFORMATION));
    STARTUPINFO cmd_startup_info;
    ZeroMemory(&cmd_startup_info, sizeof(STARTUPINFO));

    cmd_startup_info.cb=sizeof(STARTUPINFO);
    cmd_startup_info.hStdError=write_to_pipe_cmd_to_stdout;
    cmd_startup_info.hStdOutput=write_to_pipe_cmd_to_stdout;
    cmd_startup_info.hStdInput=GetStdHandle(STD_INPUT_HANDLE);
    cmd_startup_info.dwFlags|=STARTF_USESTDHANDLES;

    yarp::os::Bottle botCmd=msg.findGroup("cmd").tail();

    std::string strCmd;
    for (int s=0; s<botCmd.size(); ++s)
    {
        strCmd+=botCmd.get(s).toString()+std::string(" ");
    }

    /*
     * setting environment variable for child process
     */
    TCHAR chNewEnv[32767];

    // Get a pointer to the env block.
    LPTCH chOldEnv = GetEnvironmentStrings();

    // copying parent env variables
    LPTSTR lpOld = (LPTSTR) chOldEnv;
    LPTSTR lpNew = (LPTSTR) chNewEnv;
    while (*lpOld)
    {
        lstrcpy(lpNew, lpOld);
        lpOld += lstrlen(lpOld) + 1;
        lpNew += lstrlen(lpNew) + 1;
    }

    // Set the YARP_IS_YARPRUN environment variable to 1, so that the child
    // process will now that is running inside yarprun.
    lstrcpy(lpNew, (LPTCH) "YARP_IS_YARPRUN=1");
    lpNew += lstrlen(lpNew) + 1;

    // Set the YARPRUN_IS_FORWARDING_LOG environment variable to 1, so that
    // the child process will now that yarprun is not logging the output.
    lstrcpy(lpNew, (LPTCH) "YARPRUN_IS_FORWARDING_LOG=1");
    lpNew += lstrlen(lpNew) + 1;

    // adding new env variables
    std::string cstrEnvName;
    if (msg.check("env"))
    {
        auto ss = yarp::conf::string::split(msg.find("env").asString(), ';');
        for (const auto& s : ss) {
            lstrcpy(lpNew, (LPTCH)s.c_str());
            lpNew += lstrlen(lpNew) + 1;
        }
    }

    // closing env block
    *lpNew = (TCHAR)0;

    bool bWorkdir=msg.check("workdir");
    std::string strWorkdir=bWorkdir?msg.find("workdir").asString()+"\\":"";

    bSuccess=CreateProcess(nullptr,  // command name
                           (char*)(strWorkdir+strCmd).c_str(), // command line
                           nullptr,  // process security attributes
                           nullptr,  // primary thread security attributes
                           TRUE,          // handles are inherited
                           CREATE_NEW_PROCESS_GROUP, // creation flags
                           (LPVOID) chNewEnv,        // use new environment list
                           bWorkdir?strWorkdir.c_str():nullptr, // working directory
                           &cmd_startup_info,   // STARTUPINFO pointer
                           &cmd_process_info);  // receives PROCESS_INFORMATION

    if (!bSuccess && bWorkdir)
    {
        bSuccess=CreateProcess(nullptr,  // command name
                               (char*)(strCmd.c_str()), // command line
                               nullptr,  // process security attributes
                               nullptr,  // primary thread security attributes
                               TRUE,          // handles are inherited
                               CREATE_NEW_PROCESS_GROUP, // creation flags
                               (LPVOID) chNewEnv,        // use new environment list
                               strWorkdir.c_str(), // working directory
                               &cmd_startup_info,   // STARTUPINFO pointer
                               &cmd_process_info);  // receives PROCESS_INFORMATION
    }

    // deleting old environment variable
    FreeEnvironmentStrings(chOldEnv);

    if (!bSuccess)
    {
        result.addInt32(YARPRUN_ERROR);

        DWORD nBytes;
        std::string line1=std::string("ABORTED: server=")+mPortName
                                   +std::string(" alias=")+strAlias
                                   +std::string(" cmd=")+strCmd
                                   +std::string("pid=")+int2String(cmd_process_info.dwProcessId)
                                   +std::string("\n");

        WriteFile(write_to_pipe_cmd_to_stdout, line1.c_str(), line1.length(), &nBytes, 0);

        std::string line2=std::string("Can't execute command because ")+lastError2String()+std::string("\n");
        WriteFile(write_to_pipe_cmd_to_stdout, line1.c_str(), line2.length(), &nBytes, 0);
        FlushFileBuffers(write_to_pipe_cmd_to_stdout);

        std::string out=line1+line2;
        result.addString(out.c_str());
        fprintf(stderr, "%s", out.c_str());
        fflush(stderr);

        CloseHandle(write_to_pipe_cmd_to_stdout);
        CloseHandle(read_from_pipe_cmd_to_stdout);

        TerminateProcess(stdout_process_info.hProcess, YARPRUN_ERROR);

        CloseHandle(stdout_process_info.hProcess);

        return YARPRUN_ERROR;
    }

    FlushFileBuffers(write_to_pipe_cmd_to_stdout);

    // EVERYTHING IS ALL RIGHT
    YarpRunCmdWithStdioInfo* pInf = new YarpRunCmdWithStdioInfo(strAlias,
                                                   mPortName,
                                                   portName,
                                                   cmd_process_info.dwProcessId,
                                                   stdout_process_info.dwProcessId,
                                                   read_from_pipe_cmd_to_stdout,
                                                   write_to_pipe_cmd_to_stdout,
                                                   cmd_process_info.hProcess,
                                                   false);




    pInf->setCmd(strCmd);
    if (msg.check("env"))
    {
        pInf->setEnv(msg.find("env").asString());
    }
    mProcessVector.Add(pInf);

    result.addInt32(cmd_process_info.dwProcessId);
    std::string out=std::string("STARTED: server=")+mPortName
                             +std::string(" alias=")+strAlias
                             +std::string(" cmd=")+strCmd
                             +std::string(" pid=")+int2String(cmd_process_info.dwProcessId)
                             +std::string("\n");

    result.addString(out.c_str());
    result.addString(portName.c_str());
    fprintf(stderr, "%s", out.c_str());

    return cmd_process_info.dwProcessId;
}


int yarp::run::Run::executeCmd(const  yarp::os::Bottle& msg, yarp::os::Bottle& result)
{
    std::string strAlias=msg.find("as").asString().c_str();

    // RUN COMMAND
    PROCESS_INFORMATION cmd_process_info;
    ZeroMemory(&cmd_process_info, sizeof(PROCESS_INFORMATION));
    STARTUPINFO cmd_startup_info;
    ZeroMemory(&cmd_startup_info, sizeof(STARTUPINFO));

    cmd_startup_info.cb=sizeof(STARTUPINFO);

    yarp::os::Bottle botCmd=msg.findGroup("cmd").tail();

    std::string strCmd;
    for (int s=0; s<botCmd.size(); ++s)
    {
        strCmd+=botCmd.get(s).toString()+std::string(" ");
    }

    /*
     * setting environment variable for child process
     */
    TCHAR chNewEnv[32767];

    // Get a pointer to the env block.
    LPTCH chOldEnv = GetEnvironmentStrings();

    // copying parent env variables
    LPTSTR lpOld = (LPTSTR) chOldEnv;
    LPTSTR lpNew = (LPTSTR) chNewEnv;
    while (*lpOld)
    {
        lstrcpy(lpNew, lpOld);
        lpOld += lstrlen(lpOld) + 1;
        lpNew += lstrlen(lpNew) + 1;
    }

    // Set the YARP_IS_YARPRUN environment variable to 1, so that the child
    // process will know that is running inside yarprun.
    lstrcpy(lpNew, (LPTCH) "YARP_IS_YARPRUN=1");
    lpNew += lstrlen(lpNew) + 1;

    // Set the YARPRUN_IS_FORWARDING_LOG environment variable to 0, so that
    // the child process will know that yarprun is not logging the output.
    lstrcpy(lpNew, (LPTCH) "YARPRUN_IS_FORWARDING_LOG=0");
    lpNew += lstrlen(lpNew) + 1;

    // adding new env variables
    std::string cstrEnvName;
    if (msg.check("env"))
    {
        auto ss = yarp::conf::string::split(msg.find("env").asString(), ';');
        for (const auto& s : ss) {
            lstrcpy(lpNew, (LPTCH)s.c_str());
            lpNew += lstrlen(lpNew) + 1;
        }
    }

    // closing env block
    *lpNew = (TCHAR)0;

    bool bWorkdir=msg.check("workdir");
    std::string strWorkdir=bWorkdir?msg.find("workdir").asString()+"\\":"";

    BOOL bSuccess=CreateProcess(nullptr,  // command name
                                (char*)(strWorkdir+strCmd).c_str(), // command line
                                nullptr,  // process security attributes
                                nullptr,  // primary thread security attributes
                                TRUE,          // handles are inherited
                                CREATE_NEW_PROCESS_GROUP, // creation flags
                                (LPVOID) chNewEnv, // use new environment
                                bWorkdir ? strWorkdir.c_str() : nullptr, // working directory
                                &cmd_startup_info,   // STARTUPINFO pointer
                                &cmd_process_info);  // receives PROCESS_INFORMATION

    if (!bSuccess && bWorkdir)
    {
        bSuccess=CreateProcess(nullptr,  // command name
                               (char*)(strCmd.c_str()), // command line
                               nullptr,  // process security attributes
                               nullptr,  // primary thread security attributes
                               TRUE,          // handles are inherited
                               CREATE_NEW_PROCESS_GROUP, // creation flags
                               (LPVOID) chNewEnv, // use new environment
                               strWorkdir.c_str(), // working directory
                               &cmd_startup_info,   // STARTUPINFO pointer
                               &cmd_process_info);  // receives PROCESS_INFORMATION
    }

    // deleting old environment variable
    FreeEnvironmentStrings(chOldEnv);

    if (!bSuccess)
    {
        result.addInt32(YARPRUN_ERROR);

        std::string out=std::string("ABORTED: server=")+mPortName
                                 +std::string(" alias=")+strAlias
                                 +std::string(" cmd=")+strCmd
                                 +std::string(" pid=")+int2String(cmd_process_info.dwProcessId)
                                 +std::string("\nCan't execute command because ")+lastError2String()
                                 +std::string("\n");

        result.addString(out.c_str());
        fprintf(stderr, "%s", out.c_str());
        fflush(stderr);

        return YARPRUN_ERROR;
    }

    // EVERYTHING IS ALL RIGHT
    YarpRunProcInfo* pInf = new YarpRunProcInfo(strAlias,
                                           mPortName,
                                           cmd_process_info.dwProcessId,
                                           cmd_process_info.hProcess,
                                           false);
    pInf->setCmd(strCmd);
    if (msg.check("env"))
        pInf->setEnv(msg.find("env").asString());

    mProcessVector.Add(pInf);

    result.addInt32(cmd_process_info.dwProcessId);
    std::string out=std::string("STARTED: server=")+mPortName
                             +std::string(" alias=")+strAlias
                             +std::string(" cmd=")+strCmd
                             +std::string(" pid=")+int2String(cmd_process_info.dwProcessId)
                             +std::string("\n");

    fprintf(stderr, "%s", out.c_str());

    return cmd_process_info.dwProcessId;
}

// STDIO SERVER
int yarp::run::Run::userStdio(const  yarp::os::Bottle& msg, yarp::os::Bottle& result)
{
    PROCESS_INFORMATION stdio_process_info;
    ZeroMemory(&stdio_process_info, sizeof(PROCESS_INFORMATION));

    STARTUPINFO stdio_startup_info;
    ZeroMemory(&stdio_startup_info, sizeof(STARTUPINFO));
    stdio_startup_info.cb=sizeof(STARTUPINFO);
    stdio_startup_info.wShowWindow=SW_SHOWNOACTIVATE;
    stdio_startup_info.dwFlags=STARTF_USESHOWWINDOW;

    std::string strAlias=msg.find("as").asString();
    std::string strUUID=msg.find("stdiouuid").asString();
    std::string strCmd=std::string("yarprun --readwrite ")+strUUID;
    if (msg.check("forward")) strCmd+=std::string(" --forward ")+msg.findGroup("forward").get(1).asString()+std::string(" ")+msg.findGroup("forward").get(2).asString();

    BOOL bSuccess=CreateProcess(nullptr,  // command name
                                (char*)strCmd.c_str(), // command line
                                nullptr,  // process security attributes
                                nullptr,  // primary thread security attributes
                                TRUE,          // handles are inherited
                                CREATE_NEW_CONSOLE, // creation flags
                                nullptr,  // use parent's environment
                                nullptr,  // use parent's current directory
                                &stdio_startup_info,   // STARTUPINFO pointer
                                &stdio_process_info);  // receives PROCESS_INFORMATION

    std::string out;

    if (bSuccess)
    {
        mStdioVector.Add(new YarpRunProcInfo(strAlias,
                                             mPortName,
                                             stdio_process_info.dwProcessId,
                                             stdio_process_info.hProcess,
                                             false));

        out=std::string("STARTED: server=")+mPortName
           +std::string(" alias=")+strAlias
           +std::string(" cmd=stdio pid=")+int2String(stdio_process_info.dwProcessId)
           +std::string("\n");
    }
    else
    {
        stdio_process_info.dwProcessId=YARPRUN_ERROR;

        out=std::string("ABORTED: server=")+mPortName
           +std::string(" alias=")+strAlias
           +std::string(" cmd=stdio\n")
           +std::string("Can't open stdio window because ")+lastError2String()
           +std::string("\n");
    }

    result.clear();
    result.addInt32(stdio_process_info.dwProcessId);
    result.addString(out.c_str());
    fprintf(stderr, "%s", out.c_str());
    fflush(stderr);

    return stdio_process_info.dwProcessId;
}

////////////////
#else // LINUX
////////////////
/**
 * Split a line into separate words.
 */
void splitLine(char *pLine, char **pArgs)
{
     char *pTmp = strchr(pLine, ' ');

    if (pTmp) {
        *pTmp = '\0';
        pTmp++;
        while ((*pTmp) && (*pTmp == ' ')) {
            pTmp++;
        }
        if (*pTmp == '\0') {
            pTmp = nullptr;
        }
    }
    *pArgs = pTmp;
}

/**
 * Breaks up a line into multiple arguments.
 */
void parseArguments(char *io_pLine, int *o_pArgc, char **o_pArgv)
{
    char *pNext = io_pLine;
    size_t i;
    int j;
    int quoted = 0;
    size_t len = strlen(io_pLine);

    // Protect spaces inside quotes, but lose the quotes
    for(i = 0; i < len; i++) {
        if ((!quoted) && ('"' == io_pLine[i])) {
            quoted = 1;
            io_pLine[i] = ' ';
        } else if ((quoted) && ('"' == io_pLine[i])) {
            quoted = 0;
            io_pLine[i] = ' ';
        } else if ((quoted) && (' ' == io_pLine[i])) {
            io_pLine[i] = '\1';
        }
    }

    // init
    memset(o_pArgv, 0x00, sizeof(char*) * C_MAXARGS);
    *o_pArgc = 1;
    o_pArgv[0] = io_pLine;

    while ((nullptr != pNext) && (*o_pArgc < C_MAXARGS)) {
        splitLine(pNext, &(o_pArgv[*o_pArgc]));
        pNext = o_pArgv[*o_pArgc];

        if (nullptr != o_pArgv[*o_pArgc]) {
            *o_pArgc += 1;
        }
    }

    for(j = 0; j < *o_pArgc; j++) {
        len = strlen(o_pArgv[j]);
        for(i = 0; i < len; i++) {
            if ('\1' == o_pArgv[j][i]) {
                o_pArgv[j][i] = ' ';
            }
        }
    }
}

void yarp::run::Run::CleanZombie(int pid)
{
    bool bFound=mProcessVector && mProcessVector->CleanZombie(pid);

    if (!bFound) {
        if (mStdioVector) {
            mStdioVector->CleanZombie(pid);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

int yarp::run::Run::executeCmdAndStdio(const yarp::os::Bottle& msg, yarp::os::Bottle& result)
{
    std::string strAlias=msg.find("as").asString();
    std::string strCmd=msg.find("cmd").asString();
    std::string strStdio=msg.find("stdio").asString();
    std::string strStdioUUID=msg.find("stdiouuid").asString();

    int  pipe_stdin_to_cmd[2];
    int ret_stdin_to_cmd=yarp::run::impl::pipe(pipe_stdin_to_cmd);

    int  pipe_cmd_to_stdout[2];
    int  ret_cmd_to_stdout=yarp::run::impl::pipe(pipe_cmd_to_stdout);

    int  pipe_child_to_parent[2];
    int  ret_child_to_parent=yarp::run::impl::pipe(pipe_child_to_parent);

    if (ret_child_to_parent!=0 || ret_cmd_to_stdout!=0 || ret_stdin_to_cmd!=0)
    {
        int error=errno;

        std::string out=std::string("ABORTED: server=")+mPortName
                                 +std::string(" alias=")+strAlias
                                 +std::string(" cmd=stdout\n")
                                 +std::string("Can't create pipes ")+strerror(error)
                                 +std::string("\n");

        result.addInt32(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr, "%s", out.c_str());

        return YARPRUN_ERROR;
    }

    int pid_stdout=yarp::run::impl::fork();

    if (IS_INVALID(pid_stdout))
    {
        int error=errno;

        CLOSE(pipe_stdin_to_cmd[WRITE_TO_PIPE]);
        CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
        CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);
        CLOSE(pipe_cmd_to_stdout[READ_FROM_PIPE]);
        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);
        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

        std::string out=std::string("ABORTED: server=")+mPortName
                                 +std::string(" alias=")+strAlias
                                 +std::string(" cmd=stdout\n")
                                 +std::string("Can't fork stdout process because ")+strerror(error)
                                 +std::string("\n");

        result.addInt32(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr, "%s", out.c_str());

        return YARPRUN_ERROR;
    }

    if (IS_NEW_PROCESS(pid_stdout)) // STDOUT IMPLEMENTED HERE
    {
        REDIRECT_TO(STDIN_FILENO, pipe_cmd_to_stdout[READ_FROM_PIPE]);

        CLOSE(pipe_stdin_to_cmd[WRITE_TO_PIPE]);
        CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
        CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);
        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

        //Why removing vectors and stop threads?
        //exec* never returns and memory is claimed by the system
        //furthermore after fork() only the thread which called fork() is forked!
        //            cleanBeforeExec();

        //yarp::os::impl::signal(SIGPIPE, SIG_DFL);

        int ret = yarp::run::impl::execlp("yarprun", "yarprun", "--write", strStdioUUID.c_str(), static_cast<char*>(nullptr));

        CLOSE(pipe_cmd_to_stdout[READ_FROM_PIPE]);

        if (ret==YARPRUN_ERROR)
        {
            int error=errno;

            std::string out=std::string("ABORTED: server=")+mPortName
                                     +std::string(" alias=")+strAlias
                                     +std::string(" cmd=stdout\n")
                                     +std::string("Can't execute stdout because ")+strerror(error)
                                     +std::string("\n");

            FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE], "w");
            fprintf(out_to_parent, "%s", out.c_str());
            fflush(out_to_parent);
            fclose(out_to_parent);

            fprintf(stderr, "%s", out.c_str());
        }

        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

        std::exit(ret);
    }

    if (IS_PARENT_OF(pid_stdout))
    {
        CLOSE(pipe_cmd_to_stdout[READ_FROM_PIPE]);

        fprintf(stderr, "STARTED: server=%s alias=%s cmd=stdout pid=%d\n", mPortName.c_str(), strAlias.c_str(), pid_stdout);

        int pid_stdin=yarp::run::impl::fork();

        if (IS_INVALID(pid_stdin))
        {
            int error=errno;

            CLOSE(pipe_stdin_to_cmd[WRITE_TO_PIPE]);
            CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
            CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);
            CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);
            CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

            std::string out=std::string("ABORTED: server=")+mPortName
                                     +std::string(" alias=")+strAlias
                                     +std::string(" cmd=stdin\n")
                                     +std::string("Can't fork stdin process because ")+strerror(error)
                                     +std::string("\n");

            result.addInt32(YARPRUN_ERROR);
            result.addString(out.c_str());
            fprintf(stderr, "%s", out.c_str());

            SIGNAL(pid_stdout, SIGTERM);
            fprintf(stderr, "TERMINATING stdout (%d)\n", pid_stdout);

            return YARPRUN_ERROR;
        }

        if (IS_NEW_PROCESS(pid_stdin)) // STDIN IMPLEMENTED HERE
        {
            yarp::conf::environment::set_string("YARP_QUIET", "1");
            REDIRECT_TO(STDOUT_FILENO, pipe_stdin_to_cmd[WRITE_TO_PIPE]);
            REDIRECT_TO(STDERR_FILENO, pipe_stdin_to_cmd[WRITE_TO_PIPE]);

            CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
            CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);
            CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

            //Why removing vectors and stop threads?
            //exec* never returns and memory is claimed by the system
            //furthermore after fork() only the thread which called fork() is forked!
            //            cleanBeforeExec();

            //yarp::os::impl::signal(SIGPIPE, SIG_DFL);

            int ret = yarp::run::impl::execlp("yarprun", "yarprun", "--read", strStdioUUID.c_str(), static_cast<char*>(nullptr));

            CLOSE(pipe_stdin_to_cmd[WRITE_TO_PIPE]);

            if (ret==YARPRUN_ERROR)
            {
                int error=errno;

                std::string out=std::string("ABORTED: server=")+mPortName
                                         +std::string(" alias=")+strAlias
                                         +std::string(" cmd=stdin\n")
                                         +std::string("Can't execute stdin because ")+strerror(error)
                                         +std::string("\n");


                FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE], "w");
                fprintf(out_to_parent, "%s", out.c_str());
                fflush(out_to_parent);
                fclose(out_to_parent);
                fprintf(stderr, "%s", out.c_str());
            }

            CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

            std::exit(ret);
        }

        if (IS_PARENT_OF(pid_stdin))
        {
            // connect yarp read and write
            CLOSE(pipe_stdin_to_cmd[WRITE_TO_PIPE]);

            fprintf(stderr, "STARTED: server=%s alias=%s cmd=stdin pid=%d\n", mPortName.c_str(), strAlias.c_str(), pid_stdin);

            int pid_cmd=yarp::run::impl::fork();

            if (IS_INVALID(pid_cmd))
            {
                int error=errno;

                CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
                CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);
                CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

                std::string out=std::string("ABORTED: server=")+mPortName
                                         +std::string(" alias=")+strAlias
                                         +std::string(" cmd=")+strCmd
                                         +std::string("\nCan't fork command process because ")+strerror(error)
                                         +std::string("\n");

                result.addInt32(YARPRUN_ERROR);
                result.addString(out.c_str());
                fprintf(stderr, "%s", out.c_str());

                FILE* to_yarp_stdout=fdopen(pipe_cmd_to_stdout[WRITE_TO_PIPE], "w");
                fprintf(to_yarp_stdout, "%s", out.c_str());
                fflush(to_yarp_stdout);
                fclose(to_yarp_stdout);

                SIGNAL(pid_stdout, SIGTERM);
                fprintf(stderr, "TERMINATING stdout (%d)\n", pid_stdout);
                SIGNAL(pid_stdin, SIGTERM);
                fprintf(stderr, "TERMINATING stdin (%d)\n", pid_stdin);

                CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);

                return YARPRUN_ERROR;
            }

            if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
            {
                CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

                char *cmd_str=new char[strCmd.length()+1];
                strcpy(cmd_str, strCmd.c_str());
                /*
                int nargs=CountArgs(cmd_str);
                char **arg_str=new char*[nargs+1];
                ParseCmd(cmd_str, arg_str);
                arg_str[nargs]=0;
                */
                int nargs = 0;
                char **arg_str = new char*[C_MAXARGS + 1];
                parseArguments(cmd_str, &nargs, arg_str);
                arg_str[nargs]=nullptr;

                setvbuf(stdout, nullptr, _IONBF, 0);

                REDIRECT_TO(STDIN_FILENO, pipe_stdin_to_cmd[READ_FROM_PIPE]);
                REDIRECT_TO(STDOUT_FILENO, pipe_cmd_to_stdout[WRITE_TO_PIPE]);
                REDIRECT_TO(STDERR_FILENO, pipe_cmd_to_stdout[WRITE_TO_PIPE]);

                // Set the YARP_IS_YARPRUN environment variable to 1, so that the child
                // process will now that is running inside yarprun.
                yarp::conf::environment::set_string("YARP_IS_YARPRUN", "1");

                // Set the YARPRUN_IS_FORWARDING_LOG environment variable to 1, so that
                // the child process will now that yarprun is not logging the output.
                yarp::conf::environment::set_string("YARPRUN_IS_FORWARDING_LOG", "1");

                if (msg.check("env"))
                {
                    auto ss = yarp::conf::string::split(msg.find("env").asString(), ';');
                    for (const auto& s : ss) {
                        char* szenv = new char[s.size()+1];
                        strcpy(szenv, s.c_str());
                        yarp::run::impl::putenv(szenv); // putenv doesn't make copy of the string
                    }
                    //delete [] szenv;
                }

                if (msg.check("workdir"))
                {
                    int ret = yarp::os::impl::chdir(msg.find("workdir").asString().c_str());

                    if (ret!=0)
                    {
                        int error=errno;

                        std::string out=std::string("ABORTED: server=")+mPortName
                                                 +std::string(" alias=")+strAlias
                                                 +std::string(" cmd=")+strCmd
                                                 +std::string("\nCan't execute command, cannot set working directory ")+strerror(error)
                                                 +std::string("\n");

                        FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE], "w");
                        fprintf(out_to_parent, "%s", out.c_str());
                        fflush(out_to_parent);
                        fclose(out_to_parent);
                        fprintf(stderr, "%s", out.c_str());

                        std::exit(ret);
                    }
                }

                int ret=YARPRUN_ERROR;

                char currWorkDirBuff[1024];
                char *currWorkDir=yarp::os::impl::getcwd(currWorkDirBuff, 1024);

                if (currWorkDir)
                {
                    char **cwd_arg_str=new char*[nargs+1];
                    for (int i = 1; i < nargs; ++i) {
                        cwd_arg_str[i] = arg_str[i];
                    }
                    cwd_arg_str[nargs]=nullptr;
                    cwd_arg_str[0]=new char[strlen(currWorkDir)+strlen(arg_str[0])+16];

                    strcpy(cwd_arg_str[0], currWorkDir);
                    strcat(cwd_arg_str[0], "/");
                    strcat(cwd_arg_str[0], arg_str[0]);

                    //Why removing vectors and stop threads?
                    //exec* never returns and memory is claimed by the system
                    //furthermore after fork() only the thread which called fork() is forked!
                    //            cleanBeforeExec();

                    ret = yarp::run::impl::execvp(cwd_arg_str[0], cwd_arg_str);

                    delete [] cwd_arg_str[0];
                    delete [] cwd_arg_str;
                }

                if (ret==YARPRUN_ERROR)
                {
                    //Why removing vectors and stop threads?
                    //exec* never returns and memory is claimed by the system
                    //furthermore after fork() only the thread which called fork() is forked!
                    //            cleanBeforeExec();

                    ret = yarp::run::impl::execvp(arg_str[0], arg_str);
                }

                fflush(stdout);

                CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
                CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);

                if (ret==YARPRUN_ERROR)
                {
                    int error=errno;

                    std::string out=std::string("ABORTED: server=")+mPortName
                                             +std::string(" alias=")+strAlias
                                             +std::string(" cmd=")+strCmd
                                             +std::string("\nCan't execute command because ")+strerror(error)
                                             +std::string("\n");

                    FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE], "w");
                    fprintf(out_to_parent, "%s", out.c_str());
                    fflush(out_to_parent);
                    fclose(out_to_parent);
                    fprintf(stderr, "%s", out.c_str());
                }

                delete [] cmd_str;
                delete [] arg_str;

                CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

                std::exit(ret);
            }


            if (IS_PARENT_OF(pid_cmd))
            {
                CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
                CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);
                CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

                auto* pInf = new YarpRunCmdWithStdioInfo(
                        strAlias,
                        mPortName,
                        strStdio,
                        pid_cmd,
                        strStdioUUID,
                        mStdioVector,
                        pid_stdin,
                        pid_stdout,
                        pipe_stdin_to_cmd[READ_FROM_PIPE],
                        pipe_stdin_to_cmd[WRITE_TO_PIPE],
                        pipe_cmd_to_stdout[READ_FROM_PIPE],
                        pipe_cmd_to_stdout[WRITE_TO_PIPE],
                        nullptr,
                        false
                    );

                pInf->setCmd(strCmd);

                if (msg.check("env"))
                {
                    pInf->setEnv(msg.find("env").asString());
                }

                mProcessVector->Add(pInf);

                yarp::os::SystemClock::delaySystem(0.01);

                FILE* in_from_child=fdopen(pipe_child_to_parent[READ_FROM_PIPE], "r");
                int flags=fcntl(pipe_child_to_parent[READ_FROM_PIPE], F_GETFL, 0);
                fcntl(pipe_child_to_parent[READ_FROM_PIPE], F_SETFL, flags|O_NONBLOCK);

                std::string out;

                if (in_from_child)
                {
                    char buff[1024];

                    while(true)
                    {
                        if (!fgets(buff, 1024, in_from_child) || ferror(in_from_child) || feof(in_from_child)) {
                            break;
                        }

                        out+=std::string(buff);
                    }

                    fclose(in_from_child);
                }

                if (out.length()>0)
                {
                    pid_cmd=YARPRUN_ERROR;
                }
                else
                {
                    out=std::string("STARTED: server=")+mPortName
                       +std::string(" alias=")+strAlias
                       +std::string(" cmd=")+strCmd
                       +std::string(" pid=")+int2String(pid_cmd)
                       +std::string("\n");
                }

                result.addInt32(pid_cmd);
                result.addString(out.c_str());
                result.addString(strStdioUUID.c_str());

                fprintf(stderr, "%s", out.c_str());

                CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

                return pid_cmd;
            }
        }
    }

    result.addInt32(YARPRUN_ERROR);
    result.addString("I should never reach this point!!!\n");

    return YARPRUN_ERROR;
}

int yarp::run::Run::executeCmdStdout(const yarp::os::Bottle& msg, yarp::os::Bottle& result, std::string& loggerName)
{
    std::string proc_label = getProcLabel(msg);

    std::string strAlias=msg.find("as").asString();
    std::string strCmd=msg.find("cmd").asString();

    std::string portName="/log";
    portName+=mPortName+"/";

    std::string command = strCmd;
    command = command.substr(0, command.find(' '));
    command = command.substr(command.find_last_of("\\/") + 1);

    portName+=command;
    if (proc_label != "") { portName += "[" + proc_label + "]"; }


    int  pipe_cmd_to_stdout[2];
    int  ret_cmd_to_stdout=yarp::run::impl::pipe(pipe_cmd_to_stdout);

    int  pipe_child_to_parent[2];
    int  ret_child_to_parent=yarp::run::impl::pipe(pipe_child_to_parent);

    if (ret_child_to_parent!=0 || ret_cmd_to_stdout!=0)
    {
        int error=errno;

        std::string out=std::string("ABORTED: server=")+mPortName
                                 +std::string(" alias=")+strAlias
                                 +std::string(" cmd=stdout\n")
                                 +std::string("Can't create pipes ")+strerror(error)
                                 +std::string("\n");

        result.addInt32(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr, "%s", out.c_str());

        return YARPRUN_ERROR;
    }

    int pid_stdout=yarp::run::impl::fork();

    if (IS_INVALID(pid_stdout))
    {
        int error=errno;

        CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);
        CLOSE(pipe_cmd_to_stdout[READ_FROM_PIPE]);
        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);
        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

        std::string out=std::string("ABORTED: server=")+mPortName
                                 +std::string(" alias=")+strAlias
                                 +std::string(" cmd=stdout\n")
                                 +std::string("Can't fork stdout process because ")+strerror(error)
                                 +std::string("\n");

        result.addInt32(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr, "%s", out.c_str());

        return YARPRUN_ERROR;
    }

    if (IS_NEW_PROCESS(pid_stdout)) // STDOUT IMPLEMENTED HERE
    {
        REDIRECT_TO(STDIN_FILENO, pipe_cmd_to_stdout[READ_FROM_PIPE]);

        CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);
        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

        //Why removing vectors and stop threads?
        //exec* never returns and memory is claimed by the system
        //furthermore after fork() only the thread which called fork() is forked!
        //            cleanBeforeExec();

        //yarp::os::impl::signal(SIGPIPE, SIG_DFL);

        int ret = yarp::run::impl::execlp("yarprun", "yarprun", "--write", portName.c_str(), "--log", loggerName.c_str(), static_cast<char*>(nullptr));

        CLOSE(pipe_cmd_to_stdout[READ_FROM_PIPE]);

        if (ret==YARPRUN_ERROR)
        {
            int error=errno;

            std::string out=std::string("ABORTED: server=")+mPortName
                                     +std::string(" alias=")+strAlias
                                     +std::string(" cmd=stdout\n")
                                     +std::string("Can't execute stdout because ")+strerror(error)
                                     +std::string("\n");

            FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE], "w");
            fprintf(out_to_parent, "%s", out.c_str());
            fflush(out_to_parent);
            fclose(out_to_parent);

            fprintf(stderr, "%s", out.c_str());
        }

        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

        std::exit(ret);
    }

    if (IS_PARENT_OF(pid_stdout))
    {
        CLOSE(pipe_cmd_to_stdout[READ_FROM_PIPE]);

        fprintf(stderr, "STARTED: server=%s alias=%s cmd=stdout pid=%d\n", mPortName.c_str(), strAlias.c_str(), pid_stdout);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //if (IS_PARENT_OF(pid_stdin))
        {
            int pid_cmd=yarp::run::impl::fork();

            if (IS_INVALID(pid_cmd))
            {
                int error=errno;

                CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);
                CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

                std::string out=std::string("ABORTED: server=")+mPortName
                                         +std::string(" alias=")+strAlias
                                         +std::string(" cmd=")+strCmd
                                         +std::string("\nCan't fork command process because ")+strerror(error)
                                         +std::string("\n");

                result.addInt32(YARPRUN_ERROR);
                result.addString(out.c_str());
                fprintf(stderr, "%s", out.c_str());

                FILE* to_yarp_stdout=fdopen(pipe_cmd_to_stdout[WRITE_TO_PIPE], "w");
                fprintf(to_yarp_stdout, "%s", out.c_str());
                fflush(to_yarp_stdout);
                fclose(to_yarp_stdout);

                SIGNAL(pid_stdout, SIGTERM);
                fprintf(stderr, "TERMINATING stdout (%d)\n", pid_stdout);
                CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);

                return YARPRUN_ERROR;
            }

            if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
            {
                CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

                char *cmd_str=new char[strCmd.length()+1];
                strcpy(cmd_str, strCmd.c_str());
                /*
                int nargs=CountArgs(cmd_str);
                char **arg_str=new char*[nargs+1];
                ParseCmd(cmd_str, arg_str);
                arg_str[nargs]=0;
                */
                int nargs = 0;
                char **arg_str = new char*[C_MAXARGS + 1];
                parseArguments(cmd_str, &nargs, arg_str);
                arg_str[nargs]=nullptr;

                setvbuf(stdout, nullptr, _IONBF, 0);

                REDIRECT_TO(STDOUT_FILENO, pipe_cmd_to_stdout[WRITE_TO_PIPE]);
                REDIRECT_TO(STDERR_FILENO, pipe_cmd_to_stdout[WRITE_TO_PIPE]);

                // Set the YARP_IS_YARPRUN environment variable to 1, so that the child
                // process will now that is running inside yarprun.
                yarp::conf::environment::set_string("YARP_IS_YARPRUN", "1");

                // Set the YARPRUN_IS_FORWARDING_LOG environment variable to 1, so that
                // the child process will now that yarprun is not logging the output.
                yarp::conf::environment::set_string("YARPRUN_IS_FORWARDING_LOG", "1");

                if (msg.check("env"))
                {
                    auto ss = yarp::conf::string::split(msg.find("env").asString(), ';');
                    for (const auto& s : ss) {
                        char* szenv = new char[s.size()+1];
                        strcpy(szenv, s.c_str());
                        yarp::run::impl::putenv(szenv); // putenv doesn't make copy of the string
                    }
                    //delete [] szenv;
                }

                if (msg.check("workdir"))
                {
                    int ret = yarp::os::impl::chdir(msg.find("workdir").asString().c_str());

                    if (ret!=0)
                    {
                        int error=errno;

                        std::string out=std::string("ABORTED: server=")+mPortName
                                                 +std::string(" alias=")+strAlias
                                                 +std::string(" cmd=")+strCmd
                                                 +std::string("\nCan't execute command, cannot set working directory ")+strerror(error)
                                                 +std::string("\n");

                        FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE], "w");
                        fprintf(out_to_parent, "%s", out.c_str());
                        fflush(out_to_parent);
                        fclose(out_to_parent);
                        fprintf(stderr, "%s", out.c_str());

                        std::exit(ret);
                    }
                }

                int ret=YARPRUN_ERROR;

                char currWorkDirBuff[1024];
                char *currWorkDir=getcwd(currWorkDirBuff, 1024);

                if (currWorkDir)
                {
                    char **cwd_arg_str=new char*[nargs+1];
                    for (int i = 1; i < nargs; ++i) {
                        cwd_arg_str[i] = arg_str[i];
                    }
                    cwd_arg_str[nargs]=nullptr;
                    cwd_arg_str[0]=new char[strlen(currWorkDir)+strlen(arg_str[0])+16];

                    strcpy(cwd_arg_str[0], currWorkDir);
                    strcat(cwd_arg_str[0], "/");
                    strcat(cwd_arg_str[0], arg_str[0]);

                    //Why removing vectors and stop threads?
                    //exec* never returns and memory is claimed by the system
                    //furthermore after fork() only the thread which called fork() is forked!
                    //            cleanBeforeExec();

                    ret = yarp::run::impl::execvp(cwd_arg_str[0], cwd_arg_str);

                    delete [] cwd_arg_str[0];
                    delete [] cwd_arg_str;
                }

                if (ret==YARPRUN_ERROR)
                {
                    //Why removing vectors and stop threads?
                    //exec* never returns and memory is claimed by the system
                    //furthermore after fork() only the thread which called fork() is forked!
                    //            cleanBeforeExec();

                    ret = yarp::run::impl::execvp(arg_str[0], arg_str);
                }

                fflush(stdout);

                CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);

                if (ret==YARPRUN_ERROR)
                {
                    int error=errno;

                    std::string out=std::string("ABORTED: server=")+mPortName
                                             +std::string(" alias=")+strAlias
                                             +std::string(" cmd=")+strCmd
                                             +std::string("\nCan't execute command because ")+strerror(error)
                                             +std::string("\n");

                    FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE], "w");
                    fprintf(out_to_parent, "%s", out.c_str());
                    fflush(out_to_parent);
                    fclose(out_to_parent);
                    fprintf(stderr, "%s", out.c_str());
                }

                delete [] cmd_str;
                delete [] arg_str;

                CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

                std::exit(ret);
            }


            if (IS_PARENT_OF(pid_cmd))
            {
                CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);
                CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

                auto* pInf = new YarpRunCmdWithStdioInfo(
                        strAlias,
                        mPortName,
                        portName,
                        pid_cmd,
                        pid_stdout,
                        pipe_cmd_to_stdout[READ_FROM_PIPE],
                        pipe_cmd_to_stdout[WRITE_TO_PIPE],
                        nullptr,
                        false
                    );

                pInf->setCmd(strCmd);

                if (msg.check("env"))
                {
                    pInf->setEnv(msg.find("env").asString());
                }

                mProcessVector->Add(pInf);

                yarp::os::SystemClock::delaySystem(0.01);

                FILE* in_from_child=fdopen(pipe_child_to_parent[READ_FROM_PIPE], "r");
                int flags=fcntl(pipe_child_to_parent[READ_FROM_PIPE], F_GETFL, 0);
                fcntl(pipe_child_to_parent[READ_FROM_PIPE], F_SETFL, flags|O_NONBLOCK);

                std::string out;

                if (in_from_child)
                {
                    char buff[1024];

                    while(true)
                    {
                        if (!fgets(buff, 1024, in_from_child) || ferror(in_from_child) || feof(in_from_child)) {
                            break;
                        }

                        out+=std::string(buff);
                    }

                    fclose(in_from_child);
                }

                if (out.length()>0)
                {
                    pid_cmd=YARPRUN_ERROR;
                }
                else
                {
                    out=std::string("STARTED: server=")+mPortName
                       +std::string(" alias=")+strAlias
                       +std::string(" cmd=")+strCmd
                       +std::string(" pid=")+int2String(pid_cmd)
                       +std::string("\n");
                }

                result.addInt32(pid_cmd);
                result.addString(out.c_str());

                fprintf(stderr, "%s", out.c_str());

                CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

                return pid_cmd;
            }
        }
    }

    result.addInt32(YARPRUN_ERROR);
    result.addString("I should never reach this point!!!\n");

    return YARPRUN_ERROR;
}

int yarp::run::Run::userStdio(const yarp::os::Bottle& msg, yarp::os::Bottle& result)
{
    std::string strAlias=msg.find("as").asString();
    std::string strUUID=msg.find("stdiouuid").asString();

    std::string strCmd;

    if (msg.check("forward"))
    {
        strCmd=std::string("/bin/bash -l -c \"yarprun --readwrite ")+strUUID
              +std::string(" --forward ")+msg.findGroup("forward").get(1).asString()+std::string(" ")+msg.findGroup("forward").get(2).asString()+std::string("\"");
    }
    else
    {
        strCmd=std::string("/bin/bash -l -c \"yarprun --readwrite ")+strUUID+"\"";
    }

    int pipe_child_to_parent[2];

    if (yarp::run::impl::pipe(pipe_child_to_parent))
    {
        int error=errno;

        std::string out=std::string("ABORTED: server=")+mPortName
                                 +std::string(" alias=")+strAlias
                                 +std::string(" cmd=stdio\nCan't create pipe ")+strerror(error)
                                 +std::string("\n");

        result.clear();
        result.addInt32(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr, "%s", out.c_str());

        return YARPRUN_ERROR;
    }

    int c=0;
    char *command[16];
    for (auto & i : command) {
        i = nullptr;
    }

    cmdcpy(command[c++], "xterm");
    cmdcpy(command[c++], msg.check("hold")?"-hold":"+hold");

    if (msg.check("geometry"))
    {
        cmdcpy(command[c++], "-geometry");
        cmdcpy(command[c++], msg.find("geometry").asString().c_str());
    }

    cmdcpy(command[c++], "-title");
    cmdcpy(command[c++], strAlias.c_str());

    cmdcpy(command[c++], "-e");
    cmdcpy(command[c++], strCmd.c_str());

    int pid_cmd=yarp::run::impl::fork();

    if (IS_INVALID(pid_cmd))
    {
        int error=errno;

        std::string out=std::string("ABORTED: server=")+mPortName
                                 +std::string(" alias=")+strAlias
                                 +std::string(" cmd=stdio\nCan't fork stdout process because ")+strerror(error)
                                 +std::string("\n");

        result.clear();
        result.addInt32(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr, "%s", out.c_str());

        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);
        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

        cmdclean(command);

        return YARPRUN_ERROR;
    }

    if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
    {
        //yarp::os::impl::signal(SIGPIPE, SIG_IGN);

        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

        REDIRECT_TO(STDERR_FILENO, pipe_child_to_parent[WRITE_TO_PIPE]);

        //Why removing vectors and stop threads?
        //exec* never returns and memory is claimed by the system
        //furthermore after fork() only the thread which called fork() is forked!
        //            cleanBeforeExec();

        //yarp::os::impl::signal(SIGHUP, rwSighupHandler);

        int ret = yarp::run::impl::execvp("xterm", command);

        cmdclean(command);

        if (ret==YARPRUN_ERROR)
        {
            int error=errno;

            std::string out=std::string("ABORTED: server=")+mPortName
                                     +std::string(" alias=")+strAlias
                                     +std::string(" cmd=xterm\nCan't execute command because ")+strerror(error)
                                     +std::string("\n");

            FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE], "w");

            fprintf(out_to_parent, "%s", out.c_str());
            fflush(out_to_parent);
            fclose(out_to_parent);

            fprintf(stderr, "%s", out.c_str());
        }

        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

        std::exit(ret);
    }

    if (IS_PARENT_OF(pid_cmd))
    {
        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

        mStdioVector->Add(new YarpRunProcInfo(strAlias, mPortName, pid_cmd, nullptr, msg.check("hold")));

        result.clear();

        cmdclean(command);

        yarp::os::SystemClock::delaySystem(0.01);

        FILE* in_from_child=fdopen(pipe_child_to_parent[READ_FROM_PIPE], "r");
        int flags=fcntl(pipe_child_to_parent[READ_FROM_PIPE], F_GETFL, 0);
        fcntl(pipe_child_to_parent[READ_FROM_PIPE], F_SETFL, flags|O_NONBLOCK);
        std::string out;

        if (in_from_child)
        {
            char buff[1024];

            while(true)
            {
                if (!fgets(buff, 1024, in_from_child) || ferror(in_from_child) || feof(in_from_child)) {
                    break;
                }

                out+=std::string(buff);
            }

            fclose(in_from_child);
        }

        result.clear();

        //if (out.length()>0)
        if (out.substr(0, 14)=="xterm Xt error" || out.substr(0, 7)=="ABORTED")
        {
            pid_cmd=YARPRUN_ERROR;
        }
        else
        {
            out=std::string("STARTED: server=")+mPortName
               +std::string(" alias=")+strAlias
               +std::string(" cmd=xterm pid=")+int2String(pid_cmd)
               +std::string("\n");

        }

        fprintf(stderr, "%s", out.c_str());

        result.addInt32(pid_cmd);
        result.addString(out.c_str());

        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

        return pid_cmd;
    }

    result.clear();
    result.addInt32(YARPRUN_ERROR);

    return YARPRUN_ERROR;
}

int yarp::run::Run::executeCmd(const yarp::os::Bottle& msg, yarp::os::Bottle& result)
{
    std::string strAlias(msg.find("as").asString());
    std::string strCmd(msg.find("cmd").toString());

    int  pipe_child_to_parent[2];
    int ret_pipe_child_to_parent=yarp::run::impl::pipe(pipe_child_to_parent);

    if (ret_pipe_child_to_parent!=0)
    {
        int error=errno;

        std::string out=std::string("ABORTED: server=")+mPortName
                                 +std::string(" alias=")+strAlias
                                 +std::string(" cmd=stdio\nCan't create pipe ")+strerror(error)
                                 +std::string("\n");


        result.addInt32(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr, "%s", out.c_str());

        return YARPRUN_ERROR;
    }

    int pid_cmd=yarp::run::impl::fork();

    if (IS_INVALID(pid_cmd))
    {
        int error=errno;

        std::string out=std::string("ABORTED: server=")+mPortName
                                 +std::string(" alias=")+strAlias
                                 +std::string(" cmd=")+strCmd
                                 +std::string("\nCan't fork command process because ")+strerror(error)
                                 +std::string("\n");

        result.addInt32(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr, "%s", out.c_str());

        return YARPRUN_ERROR;
    }

    if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
    {
        int saved_stderr = yarp::run::impl::dup(STDERR_FILENO);
        int null_file=open("/dev/null", O_WRONLY);
        if (null_file >= 0)
        {
            REDIRECT_TO(STDOUT_FILENO, null_file);
            REDIRECT_TO(STDERR_FILENO, null_file);
            close(null_file);
        }
        char *cmd_str=new char[strCmd.length()+1];
        strcpy(cmd_str, strCmd.c_str());
        /*
        int nargs=CountArgs(cmd_str);
        char **arg_str=new char*[nargs+1];
        ParseCmd(cmd_str, arg_str);
        arg_str[nargs]=0;
        */
        int nargs = 0;
        char **arg_str = new char*[C_MAXARGS + 1];
        parseArguments(cmd_str, &nargs, arg_str);
        arg_str[nargs]=nullptr;

        // Set the YARP_IS_YARPRUN environment variable to 1, so that the child
        // process will now that is running inside yarprun.
        yarp::conf::environment::set_string("YARP_IS_YARPRUN", "1");

        // Set the YARPRUN_IS_FORWARDING_LOG environment variable to 0, so that
        // the child process will now that yarprun is not logging the output.
        yarp::conf::environment::set_string("YARPRUN_IS_FORWARDING_LOG", "0");

        if (msg.check("env"))
        {
            auto ss = yarp::conf::string::split(msg.find("env").asString(), ';');
            for (const auto& s : ss) {
                char* szenv = new char[s.size()+1];
                strcpy(szenv, s.c_str());
                yarp::run::impl::putenv(szenv); // putenv doesn't make copy of the string
            }
        }

        if (msg.check("workdir"))
        {
            int ret = yarp::os::impl::chdir(msg.find("workdir").asString().c_str());

            if (ret!=0)
            {
                int error=errno;

                std::string out=std::string("ABORTED: server=")+mPortName
                                         +std::string(" alias=")+strAlias
                                         +std::string(" cmd=")+strCmd
                                         +std::string("\nCan't execute command, cannot set working directory ")+strerror(error)
                                         +std::string("\n");

                FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE], "w");
                fprintf(out_to_parent, "%s", out.c_str());
                fflush(out_to_parent);
                fclose(out_to_parent);

                REDIRECT_TO(STDERR_FILENO, saved_stderr);
                fprintf(stderr, "%s", out.c_str());
            }
        }

        int ret=YARPRUN_ERROR;

        char currWorkDirBuff[1024];
        char *currWorkDir=getcwd(currWorkDirBuff, 1024);

        if (currWorkDir)
        {
            char **cwd_arg_str=new char*[nargs+1];
            for (int i = 1; i < nargs; ++i) {
                cwd_arg_str[i] = arg_str[i];
            }
            cwd_arg_str[nargs]=nullptr;
            cwd_arg_str[0]=new char[strlen(currWorkDir)+strlen(arg_str[0])+16];


            strcpy(cwd_arg_str[0], currWorkDir);
            strcat(cwd_arg_str[0], "/");
            strcat(cwd_arg_str[0], arg_str[0]);

            //Why removing vectors and stop threads?
            //exec* never returns and memory is claimed by the system
            //furthermore after fork() only the thread which called fork() is forked!
//            cleanBeforeExec();

            ret = yarp::run::impl::execvp(cwd_arg_str[0], cwd_arg_str);

            delete [] cwd_arg_str[0];
            delete [] cwd_arg_str;
        }

        if (ret==YARPRUN_ERROR)
        {
            //Why removing vectors and stop threads?
            //exec* never returns and memory is claimed by the system
            //furthermore after fork() only the thread which called fork() is forked!
            //            cleanBeforeExec();
            ret = yarp::run::impl::execvp(arg_str[0], arg_str);
        }

        if (ret==YARPRUN_ERROR)
        {
            int error=errno;

            std::string out=std::string("ABORTED: server=")+mPortName
                                     +std::string(" alias=")+strAlias
                                     +std::string(" cmd=")+strCmd
                                     +std::string("\nCan't execute command because ")+strerror(error)
                                     +std::string("\n");

            FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE], "w");
            fprintf(out_to_parent, "%s", out.c_str());
            fflush(out_to_parent);
            fclose(out_to_parent);

            if (saved_stderr >= 0)
            {
                REDIRECT_TO(STDERR_FILENO, saved_stderr);
            }
            fprintf(stderr, "%s", out.c_str());
        }

        delete [] cmd_str;
        delete [] arg_str;

        std::exit(ret);
    }

    if (IS_PARENT_OF(pid_cmd))
    {
        auto* pInf = new YarpRunProcInfo(strAlias, mPortName, pid_cmd, nullptr, false);
        pInf->setCmd(strCmd);
        if (msg.check("env")) {
            pInf->setEnv(msg.find("env").asString());
        }
        mProcessVector->Add(pInf);
        char pidstr[16];
        sprintf(pidstr, "%d", pid_cmd);

        yarp::os::SystemClock::delaySystem(0.01);

        FILE* in_from_child=fdopen(pipe_child_to_parent[READ_FROM_PIPE], "r");
        int flags=fcntl(pipe_child_to_parent[READ_FROM_PIPE], F_GETFL, 0);
        fcntl(pipe_child_to_parent[READ_FROM_PIPE], F_SETFL, flags|O_NONBLOCK);

        std::string out;

        if (in_from_child)
        {
            char buff[1024];

            while(true)
            {
                if (!fgets(buff, 1024, in_from_child) || ferror(in_from_child) || feof(in_from_child)) {
                    break;
                }

                out+=std::string(buff);
            }

            fclose(in_from_child);
        }

        if (out.length()>0)
        {
            pid_cmd=YARPRUN_ERROR;
        }
        else
        {
            out=std::string("STARTED: server=")+mPortName
               +std::string(" alias=")+strAlias
               +std::string(" cmd=")+strCmd
               +std::string(" pid=")+int2String(pid_cmd)
               +std::string("\n");
        }

        fprintf(stderr, "%s", out.c_str());

        result.addInt32(pid_cmd);
        result.addString(out.c_str());

        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);
        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

        return pid_cmd;
    }

    result.addInt32(YARPRUN_ERROR);

    return YARPRUN_ERROR;
}

#endif

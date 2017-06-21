/*
 * Copyright (C) 2007-2009 RobotCub Consortium
 * Author: Alessandro Scalzo <alessandro.scalzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/Run.h>

#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/SystemInfo.h>
#include <yarp/os/SystemInfoSerializer.h>
#include <yarp/os/Time.h>

#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/SplitString.h>
#include <yarp/os/impl/PlatformSysPrctl.h>
#include <yarp/os/impl/PlatformSignal.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformUnistd.h>
#include <yarp/os/impl/RunCheckpoints.h>
#include <yarp/os/impl/RunProcManager.h>
#include <yarp/os/impl/RunReadWrite.h>

#include <cstdio>
#include <string>
#include <cstring>

#if defined(_WIN32)
# if !defined(WIN32_LEAN_AND_MEAN)
#  define WIN32_LEAN_AND_MEAN
# endif
# include <windows.h>
#else
# define C_MAXARGS       128     // the max number of command parameters. rational?
#endif

#if defined(_WIN32)
YarpRunInfoVector yarp::os::Run::mProcessVector;
YarpRunInfoVector yarp::os::Run::mStdioVector;
inline yarp::os::ConstString lastError2String()
{
    int error=GetLastError();
    char buff[1024];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, YARP_NULLPTR, error, 0, buff, 1024, YARP_NULLPTR);

    return yarp::os::ConstString(buff);
}
#else
//#define SIGSTDIO SIGHUP
#define READ_FROM_PIPE 0
#define WRITE_TO_PIPE  1
#define REDIRECT_TO(from, to) yarp::os::impl::dup2(to, from)
YarpRunInfoVector* yarp::os::Run::mProcessVector = YARP_NULLPTR;
YarpRunInfoVector* yarp::os::Run::mStdioVector = YARP_NULLPTR;
ZombieHunterThread* yarp::os::Run::mBraveZombieHunter = YARP_NULLPTR;
#endif

///////////////////////////
// OS INDEPENDENT FUNCTIONS
///////////////////////////

yarp::os::ConstString yarp::os::Run::mPortName;
yarp::os::RpcServer* yarp::os::Run::pServerPort=YARP_NULLPTR;
int yarp::os::Run::mProcCNT=0;
bool yarp::os::Run::mStresstest=false;
bool yarp::os::Run::mLogged=false;
yarp::os::ConstString yarp::os::Run::mLoggerPort("/yarplogger");

////////////////////////////////////

static RunTerminator *pTerminator = YARP_NULLPTR;

void sigstdio_handler(int sig)
{
    char msg[16];
    yarp::os::Time::useSystemClock();
    sprintf(msg, "SIGNAL %d", sig);
    RUNLOG(msg);

    if (pTerminator) pTerminator->exit();
}

////////////////////////////////////

static char slash = yarp::os::NetworkBase::getDirectorySeparator()[0];
////// adapted from libYARP_OS: ResourceFinder.cpp
static yarp::os::Bottle parsePaths(const yarp::os::ConstString& txt) {
    char slash = yarp::os::NetworkBase::getDirectorySeparator()[0];
    char sep = yarp::os::NetworkBase::getPathSeparator()[0];
    yarp::os::Bottle result;
    const char *at = txt.c_str();
    int slash_tweak = 0;
    int len = 0;
    for (yarp::os::ConstString::size_type i=0; i<txt.length(); i++) {
        char ch = txt[i];
        if (ch==sep) {
            result.addString(yarp::os::ConstString(at, len-slash_tweak));
            at += len+1;
            len = 0;
            slash_tweak = 0;
            continue;
        }
        slash_tweak = (ch==slash && len>0)?1:0;
        len++;
    }
    if (len>0) {
        result.addString(yarp::os::ConstString(at, len-slash_tweak));
    }
    return result;
}

static bool fileExists(const char *fname) {
        FILE *fp = YARP_NULLPTR;
        fp = fopen(fname, "r");
        if (!fp) {
            return false;
        } else {
            fclose(fp);
            return true;
        }
    }


/////////
int yarp::os::Run::main(int argc, char *argv[])
{
    Property config;
    config.fromCommand(argc, argv, false);

    // SERVER
    if (config.check("server"))
    {
        mLogged=config.check("log");

        if (mLogged)
        {
            Bottle botPortLogger=config.findGroup("log");

            if (botPortLogger.size()>1)
            {
                mLoggerPort=botPortLogger.get(1).asString();
            }
        }

        mPortName=yarp::os::ConstString(config.find("server").asString());

        return server();
    }

    if (config.check("echo"))
    {
        char line[1024];
        fprintf(stderr, "Program echo started.\n");
        fflush(stderr);

        while(true)
        {
            int ret=scanf("%s", line);

            if (ret>0)
            {
                fprintf(stderr, "%s\n", line);
                fflush(stderr);
            }
        }

        return 0;
    }

    //yarp::os::Network yarp;

    mPortName="";

    if (!yarp::os::Network::getLocalMode())
    {
        if (!yarp::os::Network::checkNetwork())
        {
            fprintf(stderr, "ERROR: no yarp network found.\n");

            return YARPRUN_ERROR;
        }
    }

    //////////////////////////////////////////////////////////

    if (config.check("readwrite"))
    {
        yarp::os::impl::Logger::get().setVerbosity(-1);
        yarp::os::ConstString uuid=config.findGroup("readwrite").get(1).asString();
        yarp::os::ConstString fPortName("");
        yarp::os::ConstString lPortName("");

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

        sigaction(SIGTERM, &new_action, YARP_NULLPTR);
        sigaction(SIGHUP, &new_action, YARP_NULLPTR);
        //yarp::os::impl::signal(SIGHUP, SIG_IGN);
        //yarp::os::impl::signal(SIGINT, SIG_IGN);
        yarp::os::impl::signal(SIGPIPE, SIG_IGN);

        if (getppid()==1) return 0;
#else
        yarp::os::impl::prctl(PR_SET_PDEATHSIG, SIGTERM);

        struct sigaction new_action;
        new_action.sa_handler=sigstdio_handler;
        yarp::os::impl::sigfillset(&new_action.sa_mask);
        new_action.sa_flags=0;

        yarp::os::impl::sigaction(SIGTERM, &new_action, YARP_NULLPTR);
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

    if (config.check("write"))
    {
        yarp::os::impl::Logger::get().setVerbosity(-1);
        yarp::os::ConstString portName=config.findGroup("write").get(1).asString();

#if defined(_WIN32)
        yarp::os::impl::signal(SIGINT,  sigstdio_handler);
        yarp::os::impl::signal(SIGTERM, sigstdio_handler);
        yarp::os::impl::signal(SIGBREAK, sigstdio_handler);
#else
        struct sigaction new_action;
        new_action.sa_handler=sigstdio_handler;
        yarp::os::impl::sigfillset(&new_action.sa_mask);
        new_action.sa_flags=0;
        yarp::os::impl::sigaction(SIGTERM, &new_action, YARP_NULLPTR);
        //yarp::os::impl::signal(SIGINT,  SIG_IGN);
        yarp::os::impl::signal(SIGPIPE, SIG_IGN);
        yarp::os::impl::signal(SIGHUP,  SIG_IGN);
#endif

        if (config.check("log"))
        {
            yarp::os::ConstString loggerName=config.find("log").asString();
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

    if (config.check("read"))
    {
        yarp::os::impl::Logger::get().setVerbosity(-1);
        yarp::os::ConstString uuid=config.findGroup("read").get(1).asString();

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

    //////////////////////////////////////////////////////////

    if (config.check("stresstest"))
    {
        fprintf(stderr, "Yarprun stress test started.\n");
        fflush(stderr);

        int max_interval_ms=config.find("stresstest").asInt();
        yarp::os::ConstString tag_zero=config.find("as").asString();
        yarp::os::Bottle srv=config.findGroup("on");

        config.unput("as");
        config.unput("stresstest");

        yarp::os::ConstString cmd;

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

        while (mStresstest)
        {
            yarp::os::SystemClock::delaySystem(0.001*(rand() % max_interval_ms));

            Property stresser=config;

            sprintf(tag, "%s_%u", tag_zero.c_str(), t++);
            stresser.put("as", tag);

            if (isCommand)
            {
                sprintf(cmd_and_name, "%s --name /%s", cmd.c_str(), tag);
                stresser.put("cmd", cmd_and_name);
            }

            client(stresser);

            if (isCommand && ++term_cycle>=4)
            {
                term_cycle=0;

                int r=t-(rand()%8);

                for (int i=u; i<r; ++i)
                {
                    sprintf(tag, "%s_%u", tag_zero.c_str(), i);

                    Bottle as;
                    as.addString("sigterm");
                    as.addString(tag);

                    Bottle term;
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

yarp::os::Bottle yarp::os::Run::sendMsg(Bottle& msg, yarp::os::ConstString target, int RETRY, double DELAY)
{
    Bottle response;

    for (int r=0; r<RETRY; ++r)
    {
        yarp::os::RpcClient port;

        if (!port.open("..."))
        {
            yarp::os::SystemClock::delaySystem(DELAY);
            continue;
        }

        if (!yarp::os::Network::connect(port.getName(), target))
        {
            port.close();
            yarp::os::SystemClock::delaySystem(DELAY);
            continue;
        }

        RUNLOG("<<<port.write(msg, response)")
        if (!port.write(msg, response))
        {
            port.close();
            yarp::os::SystemClock::delaySystem(DELAY);
            continue;
        }
        RUNLOG(">>>port.write(msg, response)")

        yarp::os::Network::disconnect(port.getName().c_str(), target.c_str());
        port.close();

        fprintf(stderr, "RESPONSE:\n=========\n");
        for (int s=0; s<response.size(); ++s)
        {
            fprintf(stderr, "%s\n", response.get(s).toString().c_str());
        }

        return response;
    }

    response.addString("RESPONSE:\n");
    response.addString("=========\n");
    response.addString("Cannot connect to remote server, aborting...\n");
    for (int s=0; s<response.size(); ++s)
    {
        fprintf(stderr, "%s\n", response.get(s).toString().c_str());
    }
    return response;
}

void sigint_handler(int sig)
{
    yarp::os::Run::mStresstest=false;

    if (yarp::os::Run::pServerPort)
    {
        yarp::os::RpcServer *pClose=yarp::os::Run::pServerPort;
        yarp::os::Run::pServerPort = YARP_NULLPTR;
        pClose->close();
    }
    //else
    //{
    //}
}

///////////////////////////
// WINDOWS SERVER
#if defined(_WIN32)
int yarp::os::Run::server()
{
    yarp::os::Semaphore serializer(1);

    RpcServer port;

    if (!port.open(mPortName.c_str()))
    {
        yError() << "Yarprun failed to open port: " << mPortName.c_str();
        return YARPRUN_ERROR;
    }

    yarp::os::Bottle cmd, reply;
    cmd.addString("set");
    cmd.addString(port.getName());
    cmd.addString("yarprun");
    cmd.addString("true");
    yarp::os::impl::NameClient::getNameClient().send(cmd, reply);

    yInfo() << "Yarprun succesfully started on port: " << mPortName.c_str();

    pServerPort=&port;

    yarp::os::impl::signal(SIGINT, sigint_handler);
    yarp::os::impl::signal(SIGTERM, sigint_handler);

    // Enabling cpu load collector on windows
    //yarp::os::impl::SystemInfo::enableCpuLoadCollector();

    while (pServerPort)
    {
        Bottle msg;

        RUNLOG("<<<port.read(msg, true)")
        if (!port.read(msg, true)) break;
        RUNLOG(">>>port.read(msg, true)")

        if (!pServerPort) break;

        //printf("<<< %s >>>\n", msg.toString().c_str());
        //fflush(stdout);

///////////////////////////////////////////////////

        // command with stdio management
        if (msg.check("stdio"))
        {
            yarp::os::ConstString strOnPort=msg.find("on").asString();
            yarp::os::ConstString strStdioPort=msg.find("stdio").asString();

            if (strOnPort==mPortName)
            {
                yarp::os::ConstString strUUID=mPortName+"/"+int2String(getpid())+"/"+msg.find("as").asString()+"-"+int2String(mProcCNT++);
                Bottle botUUID;
                botUUID.addString("stdiouuid");
                botUUID.addString(strUUID.c_str());
                msg.addList()=botUUID;

                if (mLogged || msg.check("log"))
                {
                    yarp::os::ConstString strAlias=msg.find("as").asString();
                    yarp::os::ConstString portName="/log";
                    portName+=mPortName+"/";
                    yarp::os::ConstString command=msg.findGroup("cmd").get(1).asString();
                    int space=command.find(" ");
                    if (space!=ConstString::npos) command=command.substr(0, space);
                    portName+=command;

                    Bottle botFwd;
                    botFwd.addString("forward");
                    botFwd.addString(portName.c_str());
                    if (msg.check("log"))
                    {
                        yarp::os::Bottle botLogger=msg.findGroup("log");

                        if (botLogger.size()>1)
                        {
                            botFwd.addString(botLogger.get(1).asString());
                        }
                        else
                        {
                            botFwd.addString(mLoggerPort);
                        }
                    }
                    else
                    {
                        botFwd.addString(mLoggerPort);
                    }
                    msg.addList()=botFwd;
                }

                Bottle cmdResult;
                if (executeCmdAndStdio(msg, cmdResult)>0)
                {
                    if (strStdioPort==mPortName)
                    {
                        Bottle stdioResult;
                        userStdio(msg, stdioResult);
                        cmdResult.append(stdioResult);
                    }
                    else
                    {
                        cmdResult.append(sendMsg(msg, strStdioPort));
                    }
                }

                port.reply(cmdResult);
            }
            else
            {
                Bottle stdioResult;
                userStdio(msg, stdioResult);
                port.reply(stdioResult);
            }

            continue;
        }

        // without stdio
        if (msg.check("cmd"))
        {
            Bottle cmdResult;

            if (msg.check("log"))
            {
                yarp::os::Bottle botLogger=msg.findGroup("log");

                if (botLogger.size()>1)
                {
                    yarp::os::ConstString loggerName=botLogger.get(1).asString();
                    executeCmdStdout(msg, cmdResult, loggerName);
                }
                else
                {
                    executeCmdStdout(msg, cmdResult, mLoggerPort);
                }
            }
            else if (mLogged)
            {
                executeCmdStdout(msg, cmdResult, mLoggerPort);
            }
            else
            {
                executeCmd(msg, cmdResult);
            }
            port.reply(cmdResult);
            continue;
        }

        if (msg.check("kill"))
        {
            yarp::os::ConstString alias(msg.findGroup("kill").get(1).asString());
            int sig=msg.findGroup("kill").get(2).asInt();
            Bottle result;
            result.addString(mProcessVector.Signal(alias, sig)?"kill OK":"kill FAILED");
            port.reply(result);
            continue;
        }

        if (msg.check("sigterm"))
        {
            yarp::os::ConstString alias(msg.find("sigterm").asString());
            Bottle result;
            result.addString(mProcessVector.Signal(alias, SIGTERM)?"sigterm OK":"sigterm FAILED");
            port.reply(result);
            continue;
        }

        if (msg.check("sigtermall"))
        {
            mProcessVector.Killall(SIGTERM);
            Bottle result;
            result.addString("sigtermall OK");
            port.reply(result);
            continue;
        }

        if (msg.check("ps"))
        {
            Bottle result;
            result.append(mProcessVector.PS());
            port.reply(result);
            continue;
        }

        if (msg.check("isrunning"))
        {
            yarp::os::ConstString alias(msg.find("isrunning").asString());
            Bottle result;
            result.addString(mProcessVector.IsRunning(alias)?"running":"not running");
            port.reply(result);
            continue;
        }

        if (msg.check("killstdio"))
        {
            yarp::os::ConstString alias(msg.find("killstdio").asString());
            mStdioVector.Signal(alias, SIGTERM);
            Bottle result;
            result.addString("killstdio OK");
            port.reply(result);
            continue;
        }

////////////////////////////////////////////////////////////////////

        if (msg.check("sysinfo"))
        {
            yarp::os::SystemInfoSerializer sysinfo;
            port.reply(sysinfo);
            continue;
        }

        if (msg.check("which"))
        {
            ConstString fileName=msg.find("which").asString();
            if (fileName!="")
            {
                yarp::os::Bottle possiblePaths = parsePaths(yarp::os::NetworkBase::getEnvironment("PATH"));
                for (int i=0; i<possiblePaths.size(); ++i)
                {
                    ConstString guessString=possiblePaths.get(i).asString() + slash + fileName;
                    const char* guess=guessString.c_str();
                    if (fileExists (guess))
                    {
                        fileName= "\"" + std::string(guess) + "\"";
                        break;
                    }
                }
            }
            yarp::os::Value fileNameWriter(fileName);
            port.reply(fileNameWriter);
            continue;
        }

        if (msg.check("exit"))
        {
            pServerPort=0;
            Bottle result;
            result.addString("exit OK");
            port.reply(result);
            port.close();
        }
    }


    Run::mStdioVector.Killall(SIGTERM);

    Run::mProcessVector.Killall(SIGTERM);

    return 0;
}

///////////////////////
#else // LINUX SERVER
///////////////////////

void yarp::os::Run::cleanBeforeExec()
{
    // zombie hunter stop

    //yarp::os::impl::signal(SIGPIPE, SIG_IGN);
    //yarp::os::impl::signal(SIGCHLD, SIG_DFL);
    //yarp::os::impl::signal(SIGINT, SIG_DFL);
    //yarp::os::impl::signal(SIGTERM, SIG_DFL);

    if (mProcessVector)
    {
        YarpRunInfoVector *p=mProcessVector;
        mProcessVector = YARP_NULLPTR;
        delete p;
    }
    if (mStdioVector)
    {
        YarpRunInfoVector *p=mStdioVector;
        mStdioVector = YARP_NULLPTR;
        delete p;
    }
    if (mBraveZombieHunter)
    {
        ZombieHunterThread *p=mBraveZombieHunter;
        mBraveZombieHunter = YARP_NULLPTR;
        p->stop();
        delete p;
    }

    //yarp::os::Network::fini();
}

void yarp::os::Run::writeToPipe(int fd, yarp::os::ConstString str)
{
    int len=str.length()+1;

    ssize_t warn_suppress = write(fd, &len, 4);
    warn_suppress = write(fd, str.c_str(), len);
    YARP_UNUSED(warn_suppress);
}

int yarp::os::Run::readFromPipe(int fd, char* &data, int& buffsize)
{
    int len=0;
    char* buff=(char*)&len;

    for (int c=4, r=0; c>0; c-=r)
    {
        r=read(fd, buff, c);

        if (r<1) return -1;

        buff+=r;
    }

    if (len<=0) return 0;

    if (len>buffsize)
    {
        delete [] data;
        data=new char[buffsize=1024+(len/1024)*1024];
    }

    buff=data;

    for (int c=len, r=0; c>0; c-=r)
    {
        r=read(fd, buff, c);

        if (r<1) return -1;

        buff+=r;
    }

    return len;
}

static void sigchld_handler(int sig)
{
    if (yarp::os::Run::mBraveZombieHunter)
    {
        yarp::os::Run::mBraveZombieHunter->sigchldHandler();
    }
}

int yarp::os::Run::server()
{
    int pipe_server2manager[2];
    int pipe_manager2server[2];

    if (yarp::os::impl::pipe(pipe_server2manager))
    {
        fprintf(stderr, "Can't open pipe because %s\n", strerror(errno));
        fflush(stderr);

        return YARPRUN_ERROR;
    }

    if (yarp::os::impl::pipe(pipe_manager2server))
    {
        fprintf(stderr, "Can't open pipe because %s\n", strerror(errno));
        fflush(stderr);

        return YARPRUN_ERROR;
    }

    int pid_process_manager=yarp::os::fork();

    if (IS_INVALID(pid_process_manager))
    {
        int error=errno;

        CLOSE(pipe_server2manager[WRITE_TO_PIPE]);
        CLOSE(pipe_server2manager[READ_FROM_PIPE]);
        CLOSE(pipe_manager2server[WRITE_TO_PIPE]);
        CLOSE(pipe_manager2server[READ_FROM_PIPE]);

        fprintf(stderr, "Can't fork process manager because %s\n", strerror(error));
        fflush(stderr);

        return YARPRUN_ERROR;
    }

    if (IS_PARENT_OF(pid_process_manager))
    {
        yarp::os::impl::signal(SIGPIPE, SIG_IGN);

        CLOSE(pipe_server2manager[READ_FROM_PIPE]);
        CLOSE(pipe_manager2server[WRITE_TO_PIPE]);

        //yarp::os::Network::init();
        yarp::os::impl::Logger::get().setVerbosity(-1);

        yarp::os::RpcServer port;

        if (!port.open(mPortName.c_str()))
        {
            yError() << "Yarprun failed to open port: " << mPortName.c_str();

            if (mPortName[0]!='/') yError("Invalid port name '%s', it should start with '/'\n", mPortName.c_str());
            return YARPRUN_ERROR;
        }
        yarp::os::Bottle cmd, reply;
        cmd.addString("set");
        cmd.addString(port.getName());
        cmd.addString("yarprun");
        cmd.addString("true");

        yarp::os::impl::NameClient::getNameClient().send(cmd, reply);

        yInfo() << "Yarprun succesfully started on port: " << mPortName.c_str();

        pServerPort=&port;

        yarp::os::impl::signal(SIGINT, sigint_handler);
        yarp::os::impl::signal(SIGTERM, sigint_handler);

        int rsp_size=1024;
        char *rsp_str=new char[rsp_size];

        Bottle msg, response;

        while (pServerPort)
        {
            RUNLOG("<<<port.read(msg, true)")
            if (!port.read(msg, true)) break;
            RUNLOG(">>>port.read(msg, true)")

            if (!pServerPort) break;

            if (msg.check("sysinfo"))
            {
                yarp::os::SystemInfoSerializer sysinfo;
                port.reply(sysinfo);
                continue;
            }

            if (msg.check("which"))
            {
                ConstString fileName=msg.find("which").asString();
                if (fileName!="")
                {
                    yarp::os::Bottle possiblePaths = parsePaths(yarp::os::NetworkBase::getEnvironment("PATH"));
                    for (int i=0; i<possiblePaths.size(); ++i)
                    {
                        ConstString guessString=possiblePaths.get(i).asString() + slash + fileName;
                        const char* guess=guessString.c_str();
                        if (fileExists (guess))
                        {
                            fileName = guess;
                            break;
                        }
                    }
                }
                yarp::os::Value fileNameWriter(fileName);
                port.reply(fileNameWriter);
                continue;
            }

            if (msg.check("exit"))
            {
                pServerPort = YARP_NULLPTR;
                Bottle result;
                result.addString("exit OK");
                port.reply(result);
                port.close();
                break;
            }

            RUNLOG("<<<writeToPipe")
            writeToPipe(pipe_server2manager[WRITE_TO_PIPE], msg.toString());
            RUNLOG(">>>writeToPipe")

            RUNLOG("<<<readFromPipe")
            int nread=readFromPipe(pipe_manager2server[READ_FROM_PIPE], rsp_str, rsp_size);
            RUNLOG(">>>readFromPipe")

            if (nread<0)
            {
                fprintf(stderr, "ERROR: broken pipe between server and manager\n");
                fflush(stderr);
                break;
            }

            if (nread)
            {
                response.fromString(rsp_str);
                port.reply(response);
            }
        }

        //yarp::os::Network::fini();

        CLOSE(pipe_server2manager[WRITE_TO_PIPE]);
        CLOSE(pipe_manager2server[READ_FROM_PIPE]);

        delete [] rsp_str;

        return 0;
    }

    if (IS_NEW_PROCESS(pid_process_manager))
    {
        yarp::os::impl::signal(SIGPIPE, SIG_IGN);

        CLOSE(pipe_server2manager[WRITE_TO_PIPE]);
        CLOSE(pipe_manager2server[READ_FROM_PIPE]);

        //yarp::os::Network::init();

        mProcessVector=new YarpRunInfoVector;
        mStdioVector=new YarpRunInfoVector;

        mBraveZombieHunter=new ZombieHunterThread;
        mBraveZombieHunter->start();

        yarp::os::impl::signal(SIGCHLD, sigchld_handler);
        //yarp::os::impl::signal(SIGINT, SIG_IGN);
        //yarp::os::impl::signal(SIGTERM, SIG_IGN);

        int msg_size=1024;
        char *msg_str=new char[msg_size];

        Bottle msg;

        //while(readFromPipe(pipe_server2manager[READ_FROM_PIPE], msg_str, msg_size)>0)
        while (true)
        {
            RUNLOG("<<<readFromPipe")
            if (readFromPipe(pipe_server2manager[READ_FROM_PIPE], msg_str, msg_size)<=0) break;
            RUNLOG(">>>readFromPipe")

            //printf("<<< %s >>>\n", msg_str);
            //fflush(stdout);

            msg.fromString(msg_str);

            // command with stdio management
            if (msg.check("stdio"))
            {
                yarp::os::ConstString strOnPort=msg.find("on").asString();
                yarp::os::ConstString strStdioPort=msg.find("stdio").asString();

                if (strOnPort==mPortName)
                {
                    yarp::os::ConstString strUUID=mPortName+"/"+int2String(getpid())+"/"+msg.find("as").asString()+"-"+int2String(mProcCNT++);
                    Bottle botUUID;
                    botUUID.addString("stdiouuid");
                    botUUID.addString(strUUID.c_str());
                    msg.addList()=botUUID;

                    if (mLogged || msg.check("log"))
                    {
                        yarp::os::ConstString strAlias=msg.find("as").asString();
                        yarp::os::ConstString portName="/log";
                        portName+=mPortName+"/";
                        yarp::os::ConstString command=msg.findGroup("cmd").get(1).asString();
                        size_t space=command.find(' ');
                        if (space!=ConstString::npos) command=command.substr(0, space);
                        portName+=command;

                        Bottle botFwd;
                        botFwd.addString("forward");
                        botFwd.addString(portName.c_str());
                        if (msg.check("log"))
                        {
                            yarp::os::Bottle botLogger=msg.findGroup("log");

                            if (botLogger.size()>1)
                            {
                                botFwd.addString(botLogger.get(1).asString());
                            }
                            else
                            {
                                botFwd.addString(mLoggerPort);
                            }
                        }
                        else
                        {
                            botFwd.addString(mLoggerPort);
                        }
                        msg.addList()=botFwd;

                        yarp::os::ContactStyle style;
                        style.persistent=true;
                        yarp::os::Network::connect(portName.c_str(), mLoggerPort.c_str(), style);
                    }

                    Bottle cmdResult;
                    if (executeCmdAndStdio(msg, cmdResult)>0)
                    {
                        if (strStdioPort==mPortName)
                        {
                            Bottle stdioResult;
                            userStdio(msg, stdioResult);
                            cmdResult.append(stdioResult);
                        }
                        else
                        {
                            cmdResult.append(sendMsg(msg, strStdioPort));
                        }
                    }

                    RUNLOG("<<<writeToPipe")
                    writeToPipe(pipe_manager2server[WRITE_TO_PIPE], cmdResult.toString());
                    RUNLOG(">>>writeToPipe")
                }
                else
                {
                    Bottle stdioResult;
                    userStdio(msg, stdioResult);
                    RUNLOG("<<<writeToPipe")
                    writeToPipe(pipe_manager2server[WRITE_TO_PIPE], stdioResult.toString());
                    RUNLOG(">>>writeToPipe")
                }

                continue;
            }

            // without stdio
            if (msg.check("cmd"))
            {
                Bottle cmdResult;

                if (msg.check("log"))
                {
                    yarp::os::Bottle botLogger=msg.findGroup("log");

                    if (botLogger.size()>1)
                    {
                        yarp::os::ConstString loggerName=botLogger.get(1).asString();
                        executeCmdStdout(msg, cmdResult, loggerName);
                    }
                    else
                    {
                       executeCmdStdout(msg, cmdResult, mLoggerPort);
                    }
                }
                else if (mLogged)
                {
                    executeCmdStdout(msg, cmdResult, mLoggerPort);
                }
                else
                {
                    executeCmd(msg, cmdResult);
                }

                RUNLOG("<<<writeToPipe")
                writeToPipe(pipe_manager2server[WRITE_TO_PIPE], cmdResult.toString());
                RUNLOG(">>>writeToPipe")
                continue;
            }

            if (msg.check("kill"))
            {
                yarp::os::ConstString alias(msg.findGroup("kill").get(1).asString());
                int sig=msg.findGroup("kill").get(2).asInt();
                Bottle result;
                result.addString(mProcessVector->Signal(alias, sig)?"kill OK":"kill FAILED");
                RUNLOG("<<<writeToPipe")
                writeToPipe(pipe_manager2server[WRITE_TO_PIPE], result.toString());
                RUNLOG(">>>writeToPipe")
                continue;
            }

            if (msg.check("sigterm"))
            {
                yarp::os::ConstString alias(msg.find("sigterm").asString());
                Bottle result;
                result.addString(mProcessVector->Signal(alias, SIGTERM)?"sigterm OK":"sigterm FAILED");
                RUNLOG("<<<writeToPipe")
                writeToPipe(pipe_manager2server[WRITE_TO_PIPE], result.toString());
                RUNLOG(">>>writeToPipe")
                continue;
            }

            if (msg.check("sigtermall"))
            {
                mProcessVector->Killall(SIGTERM);
                Bottle result;
                result.addString("sigtermall OK");

                RUNLOG("<<<writeToPipe")
                writeToPipe(pipe_manager2server[WRITE_TO_PIPE], result.toString());
                RUNLOG(">>>writeToPipe")
                continue;
            }

            if (msg.check("ps"))
            {
                Bottle result;
                result.append(mProcessVector->PS());
                RUNLOG("<<<writeToPipe")
                writeToPipe(pipe_manager2server[WRITE_TO_PIPE], result.toString());
                RUNLOG(">>>writeToPipe")
                continue;
            }

            if (msg.check("isrunning"))
            {
                yarp::os::ConstString alias(msg.find("isrunning").asString());
                Bottle result;
                result.addString(mProcessVector->IsRunning(alias)?"running":"not running");
                RUNLOG("<<<writeToPipe")
                writeToPipe(pipe_manager2server[WRITE_TO_PIPE], result.toString());
                RUNLOG(">>>writeToPipe")
                continue;
            }

            if (msg.check("killstdio"))
            {
                yarp::os::ConstString alias(msg.find("killstdio").asString());
                mStdioVector->Signal(alias, SIGTERM);
                Bottle result;
                result.addString("killstdio OK");
                RUNLOG("<<<writeToPipe")
                writeToPipe(pipe_manager2server[WRITE_TO_PIPE], result.toString());
                RUNLOG(">>>writeToPipe")
                continue;
            }
        }

        mStdioVector->Killall(SIGTERM);

        mProcessVector->Killall(SIGTERM);

        if (mBraveZombieHunter)
        {
            mBraveZombieHunter->stop();
            delete mBraveZombieHunter;
            mBraveZombieHunter = YARP_NULLPTR;
        }

        delete mProcessVector;

        delete mStdioVector;

        //yarp::os::Network::fini();

        CLOSE(pipe_server2manager[READ_FROM_PIPE]);
        CLOSE(pipe_manager2server[WRITE_TO_PIPE]);

        delete [] msg_str;
    }

    return 0;
} // LINUX SERVER
#endif




// CLIENT
int yarp::os::Run::client(yarp::os::Property& config)
{
    // WITH STDIO
    //
    if (config.check("cmd") && config.check("stdio"))
    {
        ///////////////
        // syntax check
        if (config.find("stdio").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote stdio server\n");
            return YARPRUN_ERROR;
        }
        if (config.find("cmd").asString()=="")
        {
            Help("SYNTAX ERROR: missing command\n");
            return YARPRUN_ERROR;
        }
        if (!config.check("as") || config.find("as").asString()=="")
        {
            Help("SYNTAX ERROR: missing tag\n");
            return YARPRUN_ERROR;
        }
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            return YARPRUN_ERROR;
        }
        //
        ///////////////

        printf("*********** %s ************\n", config.toString().c_str());

        yarp::os::Bottle msg;
        msg.addList()=config.findGroup("stdio");
        msg.addList()=config.findGroup("cmd");
        msg.addList()=config.findGroup("as");
        msg.addList()=config.findGroup("on");

        if (config.check("workdir")) msg.addList()=config.findGroup("workdir");
        if (config.check("geometry")) msg.addList()=config.findGroup("geometry");
        if (config.check("hold")) msg.addList()=config.findGroup("hold");
        if (config.check("env")) msg.addList()=config.findGroup("env");
        if (config.check("log")) msg.addList()=config.findGroup("log");
        /*
        {
            Bottle log;
            log.addString("log");
            log.addString("log");
            msg.addList()=log;
        }
        */

        ConstString on=config.find("on").asString();

        Bottle response=sendMsg(msg, on);

        if (!response.size()) return YARPRUN_ERROR;

        if (response.get(0).asInt()<=0) return 2;

        return 0;
    }

    // NO STDIO
    //
    if (config.check("cmd"))
    {
        ///////////////
        // syntax check
        if (config.find("cmd").asString()=="")
        {
            Help("SYNTAX ERROR: missing command\n");
            return YARPRUN_ERROR;
        }
        if (!config.check("as") || config.find("as").asString()=="")
        {
            Help("SYNTAX ERROR: missing tag\n");
            return YARPRUN_ERROR;
        }
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            return YARPRUN_ERROR;
        }
        //
        ///////////////

        yarp::os::Bottle msg;
        msg.addList()=config.findGroup("cmd");
        msg.addList()=config.findGroup("as");

        if (config.check("workdir")) msg.addList()=config.findGroup("workdir");
        if (config.check("log")) msg.addList()=config.findGroup("log");
        /*
        {
            Bottle log;
            log.addString("log");
            log.addString("log");
            msg.addList()=log;
        }
        */
        if (config.check("env")) msg.addList()=config.findGroup("env");

        Bottle response=sendMsg(msg, config.find("on").asString());

        if (!response.size()) return YARPRUN_ERROR;

        if (response.get(0).asInt()<=0) return 2;

        return 0;
    }





    // client -> cmd server
    if (config.check("kill"))
    {
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            return YARPRUN_ERROR;
        }
        if (config.findGroup("kill").get(1).asString()=="")
        {
            Help("SYNTAX ERROR: missing tag\n");
            return YARPRUN_ERROR;
        }
        if (config.findGroup("kill").get(2).asInt()==0)
        {
            Help("SYNTAX ERROR: missing signum\n");
            return YARPRUN_ERROR;
        }

        yarp::os::Bottle msg;
        msg.addList()=config.findGroup("kill");

        Bottle response=sendMsg(msg, config.find("on").asString());

        if (!response.size())
        {
            return YARPRUN_ERROR;
        }

        return response.get(0).asString()=="kill OK"?0:2;
    }

    // client -> cmd server
    if (config.check("sigterm"))
    {
        if (config.find("sigterm").asString()=="")
        {
            Help("SYNTAX ERROR: missing tag");
            return YARPRUN_ERROR;
        }
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            return YARPRUN_ERROR;
        }

        yarp::os::Bottle msg;
        msg.addList()=config.findGroup("sigterm");

        Bottle response=sendMsg(msg, config.find("on").asString());

        if (!response.size())
        {
            return YARPRUN_ERROR;
        }

        return response.get(0).asString()=="sigterm OK"?0:2;
    }

    // client -> cmd server
    if (config.check("sigtermall"))
    {
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            return YARPRUN_ERROR;
        }

        yarp::os::Bottle msg;
        msg.addList()=config.findGroup("sigtermall");

        Bottle response=sendMsg(msg, config.find("on").asString());

        if (!response.size())
        {
            return YARPRUN_ERROR;
        }

        return 0;
    }

    if (config.check("ps"))
    {
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            return YARPRUN_ERROR;
        }

        yarp::os::Bottle msg;
        msg.addList()=config.findGroup("ps");

        Bottle response=sendMsg(msg, config.find("on").asString());

        if (!response.size())
        {
            return YARPRUN_ERROR;
        }

        return 0;
    }

    if (config.check("isrunning"))
    {
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            return YARPRUN_ERROR;
        }

        if (config.find("isrunning").asString()=="")
        {
            Help("SYNTAX ERROR: missing tag\n");
            return YARPRUN_ERROR;
        }

        yarp::os::Bottle msg;
        msg.addList()=config.findGroup("isrunning");

        Bottle response=sendMsg(msg, config.find("on").asString());

        if (!response.size())
        {
            return YARPRUN_ERROR;
        }

        return response.get(0).asString()=="running"?0:2;
    }

    if (config.check("sysinfo"))
    {
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            return YARPRUN_ERROR;
        }

        yarp::os::Bottle msg;
        msg.addList()=config.findGroup("sysinfo");

        yarp::os::RpcClient port;
        //port.setTimeout(5.0);
        if (!port.open("..."))
        {
            fprintf(stderr, "RESPONSE:\n=========\n");
            fprintf(stderr, "Cannot open port, aborting...\n");

            return YARPRUN_ERROR;
        }

        bool connected = yarp::os::Network::connect(port.getName(), config.find("on").asString());

        if (!connected)
        {
            fprintf(stderr, "RESPONSE:\n=========\n");
            fprintf(stderr, "Cannot connect to remote server, aborting...\n");
            port.close();
            //yarp::os::Network::unregisterName(port.getName());
            return YARPRUN_ERROR;
        }

        yarp::os::SystemInfoSerializer info;

        RUNLOG("<<<port.write(msg, info)")
        int ret = port.write(msg, info);
        RUNLOG(">>>port.write(msg, info)")
        yarp::os::Network::disconnect(port.getName().c_str(), config.find("on").asString());
        port.close();
        //yarp::os::Network::unregisterName(port.getName());
        fprintf(stdout, "RESPONSE:\n=========\n\n");

        if (!ret)
        {
            fprintf(stdout, "No response. (timeout)\n");

            return YARPRUN_ERROR;
        }

        fprintf(stdout, "Platform name    : %s\n", info.platform.name.c_str());
        fprintf(stdout, "Platform dist    : %s\n", info.platform.distribution.c_str());
        fprintf(stdout, "Platform release : %s\n", info.platform.release.c_str());
        fprintf(stdout, "Platform code    : %s\n", info.platform.codename.c_str());
        fprintf(stdout, "Platform kernel  : %s\n\n", info.platform.kernel.c_str());

        fprintf(stdout, "User Id        : %d\n", info.user.userID);
        fprintf(stdout, "User name      : %s\n", info.user.userName.c_str());
        fprintf(stdout, "User real name : %s\n", info.user.realName.c_str());
        fprintf(stdout, "User home dir  : %s\n\n", info.user.homeDir.c_str());

        fprintf(stdout, "Cpu load Ins.: %d\n", info.load.cpuLoadInstant);
        fprintf(stdout, "Cpu load 1   : %.2lf\n", info.load.cpuLoad1);
        fprintf(stdout, "Cpu load 5   : %.2lf\n", info.load.cpuLoad5);
        fprintf(stdout, "Cpu load 15  : %.2lf\n\n", info.load.cpuLoad15);

        fprintf(stdout, "Memory total : %dM\n", info.memory.totalSpace);
        fprintf(stdout, "Memory free  : %dM\n\n", info.memory.freeSpace);

        fprintf(stdout, "Storage total : %dM\n", info.storage.totalSpace);
        fprintf(stdout, "Storage free  : %dM\n\n", info.storage.freeSpace);

        fprintf(stdout, "Processor model     : %s\n", info.processor.model.c_str());
        fprintf(stdout, "Processor model num : %d\n", info.processor.modelNumber);
        fprintf(stdout, "Processor family    : %d\n", info.processor.family);
        fprintf(stdout, "Processor vendor    : %s\n", info.processor.vendor.c_str());
        fprintf(stdout, "Processor arch      : %s\n", info.processor.architecture.c_str());
        fprintf(stdout, "Processor cores     : %d\n", info.processor.cores);
        fprintf(stdout, "Processor siblings  : %d\n", info.processor.siblings);
        fprintf(stdout, "Processor Mhz       : %.2lf\n\n", info.processor.frequency);

        fprintf(stdout, "Environment variables  :\n%s\n", info.platform.environmentVars.toString().c_str());
        //fprintf(stdout, "Network IP4 : %s\n", info.network.ip4.c_str());
        //fprintf(stdout, "Network IP6 : %s\n", info.network.ip6.c_str());
        //fprintf(stdout, "Network mac : %s\n\n", info.network.mac.c_str());

        return 0;
    }

    if (config.check("which"))
    {
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");

            return YARPRUN_ERROR;
        }

        yarp::os::Bottle msg;
        msg.addList()=config.findGroup("which");

        Bottle response=sendMsg(msg, config.find("on").asString());

        if (!response.size())
        {
            return YARPRUN_ERROR;
        }
        return 0;
    }

    if (config.check("exit"))
    {
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");

            return YARPRUN_ERROR;
        }

        yarp::os::Bottle msg;
        msg.addList()=config.findGroup("exit");

        Bottle response=sendMsg(msg, config.find("on").asString());

        if (!response.size())
        {
            return YARPRUN_ERROR;
        }

        return 0;
    }

    return 0;
}

void yarp::os::Run::Help(const char *msg)
{
    fprintf(stderr, "%s", msg);
    fprintf(stderr, "\nUSAGE:\n\n");
    fprintf(stderr, "yarp run --server SERVERPORT\nrun a server on the local machine\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --as TAG --cmd COMMAND [ARGLIST] [--workdir WORKDIR] [--env ENVIRONMENT]\nrun a command on SERVERPORT server\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --as TAG --stdio STDIOSERVERPORT [--hold] [--geometry WxH+X+Y] --cmd COMMAND [ARGLIST] [--workdir WORKDIR] [--env ENVIRONMENT]\n");
    fprintf(stderr, "run a command on SERVERPORT server sending I/O to STDIOSERVERPORT server\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --kill TAG SIGNUM\nsend SIGNUM signal to TAG command\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --sigterm TAG\nterminate TAG command\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --sigtermall\nterminate all commands\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --ps\nreport commands running on SERVERPORT\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --isrunning TAG\nTAG command is running?\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --sysinfo\nreport system information of SERVERPORT\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --exit\nstop SERVERPORT server\n\n");
}

/////////////////////////
// OS DEPENDENT FUNCTIONS
/////////////////////////

// WINDOWS

#if defined(_WIN32)

// CMD SERVER
int yarp::os::Run::executeCmdAndStdio(Bottle& msg, Bottle& result)
{
    yarp::os::ConstString strAlias=msg.find("as").asString();
    yarp::os::ConstString strStdio=msg.find("stdio").asString();
    yarp::os::ConstString strStdioUUID=msg.find("stdiouuid").asString();
    //yarp::os::ConstString strCmdUUID=mPortName+"/"+int2String(GetCurrentProcessId())+"/"+strAlias+"-"+int2String(mProcCNT++);

    // PIPES
    SECURITY_ATTRIBUTES pipe_sec_attr;
    pipe_sec_attr.nLength=sizeof(SECURITY_ATTRIBUTES);
    pipe_sec_attr.bInheritHandle=TRUE;
    pipe_sec_attr.lpSecurityDescriptor = YARP_NULLPTR;
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

    BOOL bSuccess=CreateProcess(YARP_NULLPTR,  // command name
                                (char*)(yarp::os::ConstString("yarprun --write ")+strStdioUUID).c_str(), // command line
                                YARP_NULLPTR,  // process security attributes
                                YARP_NULLPTR,  // primary thread security attributes
                                TRUE,          // handles are inherited
                                CREATE_NEW_PROCESS_GROUP, // creation flags
                                YARP_NULLPTR,  // use parent's environment
                                YARP_NULLPTR,  // use parent's current directory
                                &stdout_startup_info,   // STARTUPINFO pointer
                                &stdout_process_info);  // receives PROCESS_INFORMATION

    if (!bSuccess)
    {
        yarp::os::ConstString strError=yarp::os::ConstString("ABORTED: server=")+mPortName
                                      +yarp::os::ConstString(" alias=")+strAlias
                                      +yarp::os::ConstString(" cmd=stdout\n")
                                      +yarp::os::ConstString("Can't execute stdout because ")+lastError2String()
                                      +yarp::os::ConstString("\n");

        result.addInt(YARPRUN_ERROR);
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

    bSuccess=CreateProcess(YARP_NULLPTR,  // command name
                           (char*)(yarp::os::ConstString("yarprun --read ")+strStdioUUID).c_str(), // command line
                           YARP_NULLPTR,  // process security attributes
                           YARP_NULLPTR,  // primary thread security attributes
                           TRUE,          // handles are inherited
                           CREATE_NEW_PROCESS_GROUP, // creation flags
                           YARP_NULLPTR,  // use parent's environment
                           YARP_NULLPTR,  // use parent's current directory
                           &stdin_startup_info,   // STARTUPINFO pointer
                           &stdin_process_info);  // receives PROCESS_INFORMATION

    if (!bSuccess)
    {
        yarp::os::ConstString strError=yarp::os::ConstString("ABORTED: server=")+mPortName
                                      +yarp::os::ConstString(" alias=")+strAlias
                                      +yarp::os::ConstString(" cmd=stdin\n")
                                      +yarp::os::ConstString("Can't execute stdin because ")+lastError2String()
                                      +yarp::os::ConstString("\n");

        result.addInt(YARPRUN_ERROR);
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

    Bottle botCmd=msg.findGroup("cmd").tail();

    yarp::os::ConstString strCmd;
    for (int s=0; s<botCmd.size(); ++s)
    {
        strCmd+=botCmd.get(s).toString()+yarp::os::ConstString(" ");
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

    // adding new env variables
    yarp::os::ConstString cstrEnvName;
    if (msg.check("env"))
    {
        yarp::os::impl::SplitString ss(msg.find("env").asString().c_str(), ';');
        for(int i=0; i<ss.size(); i++) {
            lstrcpy(lpNew, (LPTCH) ss.get(i));
            lpNew += lstrlen(lpNew) + 1;
        }
    }

    // closing env block
    *lpNew = (TCHAR)0;

    bool bWorkdir=msg.check("workdir");
    yarp::os::ConstString strWorkdir=bWorkdir?msg.find("workdir").asString()+"\\":"";

    bSuccess=CreateProcess(YARP_NULLPTR,  // command name
                           (char*)(strWorkdir+strCmd).c_str(), // command line
                           YARP_NULLPTR,  // process security attributes
                           YARP_NULLPTR,  // primary thread security attributes
                           TRUE,          // handles are inherited
                           CREATE_NEW_PROCESS_GROUP, // creation flags
                           (LPVOID) chNewEnv,        // use new environment list
                           bWorkdir ? strWorkdir.c_str() : YARP_NULLPTR, // working directory
                           &cmd_startup_info,   // STARTUPINFO pointer
                           &cmd_process_info);  // receives PROCESS_INFORMATION

    if (!bSuccess && bWorkdir)
    {
        bSuccess=CreateProcess(YARP_NULLPTR,  // command name
                               (char*)(strCmd.c_str()), // command line
                               YARP_NULLPTR,  // process security attributes
                               YARP_NULLPTR,  // primary thread security attributes
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
        result.addInt(YARPRUN_ERROR);

        DWORD nBytes;
        yarp::os::ConstString line1=yarp::os::ConstString("ABORTED: server=")+mPortName
                                   +yarp::os::ConstString(" alias=")+strAlias
                                   +yarp::os::ConstString(" cmd=")+strCmd
                                   +yarp::os::ConstString("pid=")+int2String(cmd_process_info.dwProcessId)
                                   +yarp::os::ConstString("\n");

        WriteFile(write_to_pipe_cmd_to_stdout, line1.c_str(), line1.length(), &nBytes, 0);

        yarp::os::ConstString line2=yarp::os::ConstString("Can't execute command because ")+lastError2String()+yarp::os::ConstString("\n");
        WriteFile(write_to_pipe_cmd_to_stdout, line1.c_str(), line2.length(), &nBytes, 0);
        FlushFileBuffers(write_to_pipe_cmd_to_stdout);

        yarp::os::ConstString out=line1+line2;
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

    result.addInt(cmd_process_info.dwProcessId);
    yarp::os::ConstString out=yarp::os::ConstString("STARTED: server=")+mPortName
                             +yarp::os::ConstString(" alias=")+strAlias
                             +yarp::os::ConstString(" cmd=")+strCmd
                             +yarp::os::ConstString("pid=")+int2String(cmd_process_info.dwProcessId)
                             +yarp::os::ConstString("\n");

    result.addString(out.c_str());
    result.addString(strStdioUUID.c_str());
    fprintf(stderr, "%s", out.c_str());

    return cmd_process_info.dwProcessId;
}

int yarp::os::Run::executeCmdStdout(Bottle& msg, Bottle& result, yarp::os::ConstString& loggerName)
{
    yarp::os::ConstString strAlias=msg.find("as").asString();
    yarp::os::ConstString portName="/log";
    portName+=mPortName+"/";
    yarp::os::ConstString command=msg.findGroup("cmd").get(1).asString();
    int space=command.find(" ");
    if (space!=ConstString::npos) command=command.substr(0, space);
    portName+=command;

    // PIPES
    SECURITY_ATTRIBUTES pipe_sec_attr;
    pipe_sec_attr.nLength=sizeof(SECURITY_ATTRIBUTES);
    pipe_sec_attr.bInheritHandle=TRUE;
    pipe_sec_attr.lpSecurityDescriptor = YARP_NULLPTR;
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

    BOOL bSuccess=CreateProcess(YARP_NULLPTR,  // command name
                                (char*)(yarp::os::ConstString("yarprun --log ")+loggerName+yarp::os::ConstString(" --write ")+portName).c_str(), // command line
                                YARP_NULLPTR,  // process security attributes
                                YARP_NULLPTR,  // primary thread security attributes
                                TRUE,          // handles are inherited
                                CREATE_NEW_PROCESS_GROUP, // creation flags
                                YARP_NULLPTR,  // use parent's environment
                                YARP_NULLPTR,  // use parent's current directory
                                &stdout_startup_info,   // STARTUPINFO pointer
                                &stdout_process_info);  // receives PROCESS_INFORMATION

    if (!bSuccess)
    {
        yarp::os::ConstString strError=yarp::os::ConstString("ABORTED: server=")+mPortName
                                      +yarp::os::ConstString(" alias=")+strAlias
                                      +yarp::os::ConstString(" cmd=stdout\n")
                                      +yarp::os::ConstString("Can't execute stdout because ")+lastError2String()
                                      +yarp::os::ConstString("\n");

        result.addInt(YARPRUN_ERROR);
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

    Bottle botCmd=msg.findGroup("cmd").tail();

    yarp::os::ConstString strCmd;
    for (int s=0; s<botCmd.size(); ++s)
    {
        strCmd+=botCmd.get(s).toString()+yarp::os::ConstString(" ");
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

    // adding new env variables
    yarp::os::ConstString cstrEnvName;
    if (msg.check("env"))
    {
        lstrcpy(lpNew, (LPTCH) msg.find("env").asString().c_str());
        lpNew += lstrlen(lpNew) + 1;
    }

    // closing env block
    *lpNew = (TCHAR)0;

    bool bWorkdir=msg.check("workdir");
    yarp::os::ConstString strWorkdir=bWorkdir?msg.find("workdir").asString()+"\\":"";

    bSuccess=CreateProcess(YARP_NULLPTR,  // command name
                           (char*)(strWorkdir+strCmd).c_str(), // command line
                           YARP_NULLPTR,  // process security attributes
                           YARP_NULLPTR,  // primary thread security attributes
                           TRUE,          // handles are inherited
                           CREATE_NEW_PROCESS_GROUP, // creation flags
                           (LPVOID) chNewEnv,        // use new environment list
                           bWorkdir?strWorkdir.c_str():YARP_NULLPTR, // working directory
                           &cmd_startup_info,   // STARTUPINFO pointer
                           &cmd_process_info);  // receives PROCESS_INFORMATION

    if (!bSuccess && bWorkdir)
    {
        bSuccess=CreateProcess(YARP_NULLPTR,  // command name
                               (char*)(strCmd.c_str()), // command line
                               YARP_NULLPTR,  // process security attributes
                               YARP_NULLPTR,  // primary thread security attributes
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
        result.addInt(YARPRUN_ERROR);

        DWORD nBytes;
        yarp::os::ConstString line1=yarp::os::ConstString("ABORTED: server=")+mPortName
                                   +yarp::os::ConstString(" alias=")+strAlias
                                   +yarp::os::ConstString(" cmd=")+strCmd
                                   +yarp::os::ConstString("pid=")+int2String(cmd_process_info.dwProcessId)
                                   +yarp::os::ConstString("\n");

        WriteFile(write_to_pipe_cmd_to_stdout, line1.c_str(), line1.length(), &nBytes, 0);

        yarp::os::ConstString line2=yarp::os::ConstString("Can't execute command because ")+lastError2String()+yarp::os::ConstString("\n");
        WriteFile(write_to_pipe_cmd_to_stdout, line1.c_str(), line2.length(), &nBytes, 0);
        FlushFileBuffers(write_to_pipe_cmd_to_stdout);

        yarp::os::ConstString out=line1+line2;
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

    result.addInt(cmd_process_info.dwProcessId);
    yarp::os::ConstString out=yarp::os::ConstString("STARTED: server=")+mPortName
                             +yarp::os::ConstString(" alias=")+strAlias
                             +yarp::os::ConstString(" cmd=")+strCmd
                             +yarp::os::ConstString("pid=")+int2String(cmd_process_info.dwProcessId)
                             +yarp::os::ConstString("\n");

    result.addString(out.c_str());
    result.addString(portName.c_str());
    fprintf(stderr, "%s", out.c_str());

    return cmd_process_info.dwProcessId;
}


int yarp::os::Run::executeCmd(yarp::os::Bottle& msg, Bottle& result)
{
    yarp::os::ConstString strAlias=msg.find("as").asString().c_str();

    // RUN COMMAND
    PROCESS_INFORMATION cmd_process_info;
    ZeroMemory(&cmd_process_info, sizeof(PROCESS_INFORMATION));
    STARTUPINFO cmd_startup_info;
    ZeroMemory(&cmd_startup_info, sizeof(STARTUPINFO));

    cmd_startup_info.cb=sizeof(STARTUPINFO);

    Bottle botCmd=msg.findGroup("cmd").tail();

    yarp::os::ConstString strCmd;
    for (int s=0; s<botCmd.size(); ++s)
    {
        strCmd+=botCmd.get(s).toString()+yarp::os::ConstString(" ");
    }

    /*
    if (msg.check("env"))
    {
        int pos = msg.find("env").asString().find("=");
        if (pos)
        {
            yarp::os::ConstString cstrName = msg.find("env").asString().substr(0, pos);
            int nValue = msg.find("env").asString().length() -
                         cstrName.length() - 1;
            yarp::os::ConstString cstrValue = msg.find("env").asString().substr(pos+1, nValue);
            SetEnvironmentVariable(cstrName.c_str(), cstrValue.c_str());
        }

    }
    */

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

    // adding new env variables
    yarp::os::ConstString cstrEnvName;
    if (msg.check("env"))
    {
        lstrcpy(lpNew, (LPTCH) msg.find("env").asString().c_str());
        lpNew += lstrlen(lpNew) + 1;
    }

    // closing env block
    *lpNew = (TCHAR)0;

    bool bWorkdir=msg.check("workdir");
    yarp::os::ConstString strWorkdir=bWorkdir?msg.find("workdir").asString()+"\\":"";

    BOOL bSuccess=CreateProcess(YARP_NULLPTR,  // command name
                                (char*)(strWorkdir+strCmd).c_str(), // command line
                                YARP_NULLPTR,  // process security attributes
                                YARP_NULLPTR,  // primary thread security attributes
                                TRUE,          // handles are inherited
                                CREATE_NEW_PROCESS_GROUP, // creation flags
                                (LPVOID) chNewEnv, // use new environment
                                bWorkdir ? strWorkdir.c_str() : YARP_NULLPTR, // working directory
                                &cmd_startup_info,   // STARTUPINFO pointer
                                &cmd_process_info);  // receives PROCESS_INFORMATION

    if (!bSuccess && bWorkdir)
    {
        bSuccess=CreateProcess(YARP_NULLPTR,  // command name
                               (char*)(strCmd.c_str()), // command line
                               YARP_NULLPTR,  // process security attributes
                               YARP_NULLPTR,  // primary thread security attributes
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
        result.addInt(YARPRUN_ERROR);

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=")+strCmd
                                 +yarp::os::ConstString(" pid=")+int2String(cmd_process_info.dwProcessId)
                                 +yarp::os::ConstString("\nCan't execute command because ")+lastError2String()
                                 +yarp::os::ConstString("\n");

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

    result.addInt(cmd_process_info.dwProcessId);
    yarp::os::ConstString out=yarp::os::ConstString("STARTED: server=")+mPortName
                             +yarp::os::ConstString(" alias=")+strAlias
                             +yarp::os::ConstString(" cmd=")+strCmd
                             +yarp::os::ConstString("pid=")+int2String(cmd_process_info.dwProcessId)
                             +yarp::os::ConstString("\n");

    fprintf(stderr, "%s", out.c_str());

    return cmd_process_info.dwProcessId;
}

// STDIO SERVER
int yarp::os::Run::userStdio(Bottle& msg, Bottle& result)
{
    PROCESS_INFORMATION stdio_process_info;
    ZeroMemory(&stdio_process_info, sizeof(PROCESS_INFORMATION));

    STARTUPINFO stdio_startup_info;
    ZeroMemory(&stdio_startup_info, sizeof(STARTUPINFO));
    stdio_startup_info.cb=sizeof(STARTUPINFO);
    stdio_startup_info.wShowWindow=SW_SHOWNOACTIVATE;
    stdio_startup_info.dwFlags=STARTF_USESHOWWINDOW;

    yarp::os::ConstString strAlias=msg.find("as").asString();
    yarp::os::ConstString strUUID=msg.find("stdiouuid").asString();
    yarp::os::ConstString strCmd=yarp::os::ConstString("yarprun --readwrite ")+strUUID;
    if (msg.check("forward")) strCmd+=yarp::os::ConstString(" --forward ")+msg.findGroup("forward").get(1).asString()+yarp::os::ConstString(" ")+msg.findGroup("forward").get(2).asString();

    BOOL bSuccess=CreateProcess(YARP_NULLPTR,  // command name
                                (char*)strCmd.c_str(), // command line
                                YARP_NULLPTR,  // process security attributes
                                YARP_NULLPTR,  // primary thread security attributes
                                TRUE,          // handles are inherited
                                CREATE_NEW_CONSOLE, // creation flags
                                YARP_NULLPTR,  // use parent's environment
                                YARP_NULLPTR,  // use parent's current directory
                                &stdio_startup_info,   // STARTUPINFO pointer
                                &stdio_process_info);  // receives PROCESS_INFORMATION

    yarp::os::ConstString out;

    if (bSuccess)
    {
        mStdioVector.Add(new YarpRunProcInfo(strAlias,
                                             mPortName,
                                             stdio_process_info.dwProcessId,
                                             stdio_process_info.hProcess,
                                             false));

        out=yarp::os::ConstString("STARTED: server=")+mPortName
           +yarp::os::ConstString(" alias=")+strAlias
           +yarp::os::ConstString(" cmd=stdio pid=")+int2String(stdio_process_info.dwProcessId)
           +yarp::os::ConstString("\n");
    }
    else
    {
        stdio_process_info.dwProcessId=YARPRUN_ERROR;

        out=yarp::os::ConstString("ABORTED: server=")+mPortName
           +yarp::os::ConstString(" alias=")+strAlias
           +yarp::os::ConstString(" cmd=stdio\n")
           +yarp::os::ConstString("Can't open stdio window because ")+lastError2String()
           +yarp::os::ConstString("\n");
    }

    result.clear();
    result.addInt(stdio_process_info.dwProcessId);
    result.addString(out.c_str());
    fprintf(stderr, "%s", out.c_str());
    fflush(stderr);

    return stdio_process_info.dwProcessId;
}

////////////////
#else // LINUX
////////////////
/*
int CountArgs(char *str)
{
    int nargs=0;

    for (bool bSpace=true; *str; ++str)
    {
        if (bSpace)
        {
            if (*str!=' ')
            {
                ++nargs;
                bSpace=false;
            }
        }
        else
        {
            if (*str==' ')
            {
                bSpace=true;
            }
        }
    }

    return nargs;
}

void ParseCmd(char* cmd_str, char** arg_str)
{
    int nargs=0;

    for (bool bSpace=true; *cmd_str; ++cmd_str)
    {
        if (*cmd_str!=' ')
        {
            if (bSpace) arg_str[nargs++]=cmd_str;
            bSpace=false;
        }
        else
        {
            *cmd_str=0;
            bSpace=true;
        }
    }
}
*/
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
            pTmp = YARP_NULLPTR;
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

    while ((YARP_NULLPTR != pNext) && (*o_pArgc < C_MAXARGS)) {
        splitLine(pNext, &(o_pArgv[*o_pArgc]));
        pNext = o_pArgv[*o_pArgc];

        if (YARP_NULLPTR != o_pArgv[*o_pArgc]) {
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

void yarp::os::Run::CleanZombie(int pid)
{
    bool bFound=mProcessVector && mProcessVector->CleanZombie(pid);

    if (!bFound) if (mStdioVector)  mStdioVector->CleanZombie(pid);
}

/////////////////////////////////////////////////////////////////////////////////////////

int yarp::os::Run::executeCmdAndStdio(yarp::os::Bottle& msg, yarp::os::Bottle& result)
{
    yarp::os::ConstString strAlias=msg.find("as").asString();
    yarp::os::ConstString strCmd=msg.find("cmd").asString();
    yarp::os::ConstString strStdio=msg.find("stdio").asString();
    yarp::os::ConstString strStdioUUID=msg.find("stdiouuid").asString();

    int  pipe_stdin_to_cmd[2];
    int ret_stdin_to_cmd=yarp::os::impl::pipe(pipe_stdin_to_cmd);

    int  pipe_cmd_to_stdout[2];
    int  ret_cmd_to_stdout=yarp::os::impl::pipe(pipe_cmd_to_stdout);

    int  pipe_child_to_parent[2];
    int  ret_child_to_parent=yarp::os::impl::pipe(pipe_child_to_parent);

    if (ret_child_to_parent!=0 || ret_cmd_to_stdout!=0 || ret_stdin_to_cmd!=0)
    {
        int error=errno;

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=stdout\n")
                                 +yarp::os::ConstString("Can't create pipes ")+strerror(error)
                                 +yarp::os::ConstString("\n");

        result.addInt(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr, "%s", out.c_str());

        return YARPRUN_ERROR;
    }

    int pid_stdout=yarp::os::fork();

    if (IS_INVALID(pid_stdout))
    {
        int error=errno;

        CLOSE(pipe_stdin_to_cmd[WRITE_TO_PIPE]);
        CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
        CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);
        CLOSE(pipe_cmd_to_stdout[READ_FROM_PIPE]);
        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);
        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=stdout\n")
                                 +yarp::os::ConstString("Can't fork stdout process because ")+strerror(error)
                                 +yarp::os::ConstString("\n");

        result.addInt(YARPRUN_ERROR);
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

        int ret = yarp::os::impl::execlp("yarprun", "yarprun", "--write", strStdioUUID.c_str(), static_cast<char*>(YARP_NULLPTR));

        CLOSE(pipe_cmd_to_stdout[READ_FROM_PIPE]);

        if (ret==YARPRUN_ERROR)
        {
            int error=errno;

            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                     +yarp::os::ConstString(" alias=")+strAlias
                                     +yarp::os::ConstString(" cmd=stdout\n")
                                     +yarp::os::ConstString("Can't execute stdout because ")+strerror(error)
                                     +yarp::os::ConstString("\n");

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

        int pid_stdin=yarp::os::fork();

        if (IS_INVALID(pid_stdin))
        {
            int error=errno;

            CLOSE(pipe_stdin_to_cmd[WRITE_TO_PIPE]);
            CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
            CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);
            CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);
            CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                     +yarp::os::ConstString(" alias=")+strAlias
                                     +yarp::os::ConstString(" cmd=stdin\n")
                                     +yarp::os::ConstString("Can't fork stdin process because ")+strerror(error)
                                     +yarp::os::ConstString("\n");

            result.addInt(YARPRUN_ERROR);
            result.addString(out.c_str());
            fprintf(stderr, "%s", out.c_str());

            SIGNAL(pid_stdout, SIGTERM);
            fprintf(stderr, "TERMINATING stdout (%d)\n", pid_stdout);

            return YARPRUN_ERROR;
        }

        if (IS_NEW_PROCESS(pid_stdin)) // STDIN IMPLEMENTED HERE
        {
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

            int ret = yarp::os::impl::execlp("yarprun", "yarprun", "--read", strStdioUUID.c_str(), static_cast<char*>(YARP_NULLPTR));

            CLOSE(pipe_stdin_to_cmd[WRITE_TO_PIPE]);

            if (ret==YARPRUN_ERROR)
            {
                int error=errno;

                yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                         +yarp::os::ConstString(" alias=")+strAlias
                                         +yarp::os::ConstString(" cmd=stdin\n")
                                         +yarp::os::ConstString("Can't execute stdin because ")+strerror(error)
                                         +yarp::os::ConstString("\n");


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

            int pid_cmd=yarp::os::fork();

            if (IS_INVALID(pid_cmd))
            {
                int error=errno;

                CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
                CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);
                CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

                yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                         +yarp::os::ConstString(" alias=")+strAlias
                                         +yarp::os::ConstString(" cmd=")+strCmd
                                         +yarp::os::ConstString("\nCan't fork command process because ")+strerror(error)
                                         +yarp::os::ConstString("\n");

                result.addInt(YARPRUN_ERROR);
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
                arg_str[nargs]=YARP_NULLPTR;

                setvbuf(stdout, YARP_NULLPTR, _IONBF, 0);

                REDIRECT_TO(STDIN_FILENO, pipe_stdin_to_cmd[READ_FROM_PIPE]);
                REDIRECT_TO(STDOUT_FILENO, pipe_cmd_to_stdout[WRITE_TO_PIPE]);
                REDIRECT_TO(STDERR_FILENO, pipe_cmd_to_stdout[WRITE_TO_PIPE]);

                if (msg.check("env"))
                {
                    yarp::os::impl::SplitString ss(msg.find("env").asString().c_str(), ';');
                    for(int i=0; i<ss.size(); i++) {
                        char* szenv = new char[strlen(ss.get(i))+1];
                        strcpy(szenv, ss.get(i));
                        yarp::os::impl::putenv(szenv); // putenv doesn't make copy of the string
                    }
                    //delete [] szenv;
                }

                if (msg.check("workdir"))
                {
                    int ret = yarp::os::impl::chdir(msg.find("workdir").asString().c_str());

                    if (ret!=0)
                    {
                        int error=errno;

                        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                                 +yarp::os::ConstString(" alias=")+strAlias
                                                 +yarp::os::ConstString(" cmd=")+strCmd
                                                 +yarp::os::ConstString("\nCan't execute command, cannot set working directory ")+strerror(error)
                                                 +yarp::os::ConstString("\n");

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
                    for (int i=1; i<nargs; ++i) cwd_arg_str[i]=arg_str[i];
                    cwd_arg_str[nargs]=YARP_NULLPTR;
                    cwd_arg_str[0]=new char[strlen(currWorkDir)+strlen(arg_str[0])+16];

                    strcpy(cwd_arg_str[0], currWorkDir);
                    strcat(cwd_arg_str[0], "/");
                    strcat(cwd_arg_str[0], arg_str[0]);

                    //Why removing vectors and stop threads?
                    //exec* never returns and memory is claimed by the system
                    //furthermore after fork() only the thread which called fork() is forked!
                    //            cleanBeforeExec();

                    ret = yarp::os::impl::execvp(cwd_arg_str[0], cwd_arg_str);

                    delete [] cwd_arg_str[0];
                    delete [] cwd_arg_str;
                }

                if (ret==YARPRUN_ERROR)
                {
                    //Why removing vectors and stop threads?
                    //exec* never returns and memory is claimed by the system
                    //furthermore after fork() only the thread which called fork() is forked!
                    //            cleanBeforeExec();

                    ret = yarp::os::impl::execvp(arg_str[0], arg_str);
                }

                fflush(stdout);

                CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
                CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);

                if (ret==YARPRUN_ERROR)
                {
                    int error=errno;

                    yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                             +yarp::os::ConstString(" alias=")+strAlias
                                             +yarp::os::ConstString(" cmd=")+strCmd
                                             +yarp::os::ConstString("\nCan't execute command because ")+strerror(error)
                                             +yarp::os::ConstString("\n");

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

                YarpRunCmdWithStdioInfo* pInf = new YarpRunCmdWithStdioInfo(
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
                        YARP_NULLPTR,
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

                yarp::os::ConstString out;

                if (in_from_child)
                {
                    char buff[1024];

                    while(true)
                    {
                        if (!fgets(buff, 1024, in_from_child) || ferror(in_from_child) || feof(in_from_child)) break;

                        out+=yarp::os::ConstString(buff);
                    }

                    fclose(in_from_child);
                }

                if (out.length()>0)
                {
                    pid_cmd=YARPRUN_ERROR;
                }
                else
                {
                    out=yarp::os::ConstString("STARTED: server=")+mPortName
                       +yarp::os::ConstString(" alias=")+strAlias
                       +yarp::os::ConstString(" cmd=")+strCmd
                       +yarp::os::ConstString(" pid=")+int2String(pid_cmd)
                       +yarp::os::ConstString("\n");
                }

                result.addInt(pid_cmd);
                result.addString(out.c_str());
                result.addString(strStdioUUID.c_str());

                fprintf(stderr, "%s", out.c_str());

                CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

                return pid_cmd;
            }
        }
    }

    result.addInt(YARPRUN_ERROR);
    result.addString("I should never reach this point!!!\n");

    return YARPRUN_ERROR;
}

int yarp::os::Run::executeCmdStdout(yarp::os::Bottle& msg, yarp::os::Bottle& result, yarp::os::ConstString& loggerName)
{
    yarp::os::ConstString strAlias=msg.find("as").asString();
    yarp::os::ConstString strCmd=msg.find("cmd").asString();

    yarp::os::ConstString portName="/log";
    portName+=mPortName+"/";

    yarp::os::ConstString command=strCmd;
    size_t space=command.find(' ');
    if (space!=ConstString::npos) command=command.substr(0, space);
    portName+=command;



    int  pipe_cmd_to_stdout[2];
    int  ret_cmd_to_stdout=yarp::os::impl::pipe(pipe_cmd_to_stdout);

    int  pipe_child_to_parent[2];
    int  ret_child_to_parent=yarp::os::impl::pipe(pipe_child_to_parent);

    if (ret_child_to_parent!=0 || ret_cmd_to_stdout!=0)
    {
        int error=errno;

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=stdout\n")
                                 +yarp::os::ConstString("Can't create pipes ")+strerror(error)
                                 +yarp::os::ConstString("\n");

        result.addInt(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr, "%s", out.c_str());

        return YARPRUN_ERROR;
    }

    int pid_stdout=yarp::os::fork();

    if (IS_INVALID(pid_stdout))
    {
        int error=errno;

        CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);
        CLOSE(pipe_cmd_to_stdout[READ_FROM_PIPE]);
        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);
        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=stdout\n")
                                 +yarp::os::ConstString("Can't fork stdout process because ")+strerror(error)
                                 +yarp::os::ConstString("\n");

        result.addInt(YARPRUN_ERROR);
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

        int ret = yarp::os::impl::execlp("yarprun", "yarprun", "--write", portName.c_str(), "--log", loggerName.c_str(), static_cast<char*>(YARP_NULLPTR));

        CLOSE(pipe_cmd_to_stdout[READ_FROM_PIPE]);

        if (ret==YARPRUN_ERROR)
        {
            int error=errno;

            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                     +yarp::os::ConstString(" alias=")+strAlias
                                     +yarp::os::ConstString(" cmd=stdout\n")
                                     +yarp::os::ConstString("Can't execute stdout because ")+strerror(error)
                                     +yarp::os::ConstString("\n");

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
            int pid_cmd=yarp::os::fork();

            if (IS_INVALID(pid_cmd))
            {
                int error=errno;

                CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);
                CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

                yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                         +yarp::os::ConstString(" alias=")+strAlias
                                         +yarp::os::ConstString(" cmd=")+strCmd
                                         +yarp::os::ConstString("\nCan't fork command process because ")+strerror(error)
                                         +yarp::os::ConstString("\n");

                result.addInt(YARPRUN_ERROR);
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
                arg_str[nargs]=YARP_NULLPTR;

                setvbuf(stdout, YARP_NULLPTR, _IONBF, 0);

                REDIRECT_TO(STDOUT_FILENO, pipe_cmd_to_stdout[WRITE_TO_PIPE]);
                REDIRECT_TO(STDERR_FILENO, pipe_cmd_to_stdout[WRITE_TO_PIPE]);

                if (msg.check("env"))
                {
                    yarp::os::impl::SplitString ss(msg.find("env").asString().c_str(), ';');
                    for(int i=0; i<ss.size(); i++) {
                        char* szenv = new char[strlen(ss.get(i))+1];
                        strcpy(szenv, ss.get(i));
                        yarp::os::impl::putenv(szenv); // putenv doesn't make copy of the string
                    }
                    //delete [] szenv;
                }

                if (msg.check("workdir"))
                {
                    int ret = yarp::os::impl::chdir(msg.find("workdir").asString().c_str());

                    if (ret!=0)
                    {
                        int error=errno;

                        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                                 +yarp::os::ConstString(" alias=")+strAlias
                                                 +yarp::os::ConstString(" cmd=")+strCmd
                                                 +yarp::os::ConstString("\nCan't execute command, cannot set working directory ")+strerror(error)
                                                 +yarp::os::ConstString("\n");

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
                    for (int i=1; i<nargs; ++i) cwd_arg_str[i]=arg_str[i];
                    cwd_arg_str[nargs]=YARP_NULLPTR;
                    cwd_arg_str[0]=new char[strlen(currWorkDir)+strlen(arg_str[0])+16];

                    strcpy(cwd_arg_str[0], currWorkDir);
                    strcat(cwd_arg_str[0], "/");
                    strcat(cwd_arg_str[0], arg_str[0]);

                    //Why removing vectors and stop threads?
                    //exec* never returns and memory is claimed by the system
                    //furthermore after fork() only the thread which called fork() is forked!
                    //            cleanBeforeExec();

                    ret = yarp::os::impl::execvp(cwd_arg_str[0], cwd_arg_str);

                    delete [] cwd_arg_str[0];
                    delete [] cwd_arg_str;
                }

                if (ret==YARPRUN_ERROR)
                {
                    //Why removing vectors and stop threads?
                    //exec* never returns and memory is claimed by the system
                    //furthermore after fork() only the thread which called fork() is forked!
                    //            cleanBeforeExec();

                    ret = yarp::os::impl::execvp(arg_str[0], arg_str);
                }

                fflush(stdout);

                CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);

                if (ret==YARPRUN_ERROR)
                {
                    int error=errno;

                    yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                             +yarp::os::ConstString(" alias=")+strAlias
                                             +yarp::os::ConstString(" cmd=")+strCmd
                                             +yarp::os::ConstString("\nCan't execute command because ")+strerror(error)
                                             +yarp::os::ConstString("\n");

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

                YarpRunCmdWithStdioInfo* pInf = new YarpRunCmdWithStdioInfo(
                        strAlias,
                        mPortName,
                        portName,
                        pid_cmd,
                        pid_stdout,
                        pipe_cmd_to_stdout[READ_FROM_PIPE],
                        pipe_cmd_to_stdout[WRITE_TO_PIPE],
                        YARP_NULLPTR,
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

                yarp::os::ConstString out;

                if (in_from_child)
                {
                    char buff[1024];

                    while(true)
                    {
                        if (!fgets(buff, 1024, in_from_child) || ferror(in_from_child) || feof(in_from_child)) break;

                        out+=yarp::os::ConstString(buff);
                    }

                    fclose(in_from_child);
                }

                if (out.length()>0)
                {
                    pid_cmd=YARPRUN_ERROR;
                }
                else
                {
                    out=yarp::os::ConstString("STARTED: server=")+mPortName
                       +yarp::os::ConstString(" alias=")+strAlias
                       +yarp::os::ConstString(" cmd=")+strCmd
                       +yarp::os::ConstString(" pid=")+int2String(pid_cmd)
                       +yarp::os::ConstString("\n");
                }

                result.addInt(pid_cmd);
                result.addString(out.c_str());

                fprintf(stderr, "%s", out.c_str());

                CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

                return pid_cmd;
            }
        }
    }

    result.addInt(YARPRUN_ERROR);
    result.addString("I should never reach this point!!!\n");

    return YARPRUN_ERROR;
}

int yarp::os::Run::userStdio(yarp::os::Bottle& msg, yarp::os::Bottle& result)
{
    yarp::os::ConstString strAlias=msg.find("as").asString();
    yarp::os::ConstString strUUID=msg.find("stdiouuid").asString();

    yarp::os::ConstString strCmd;

    if (msg.check("forward"))
    {
        strCmd=yarp::os::ConstString("/bin/bash -l -c \"yarprun --readwrite ")+strUUID
              +yarp::os::ConstString(" --forward ")+msg.findGroup("forward").get(1).asString()+yarp::os::ConstString(" ")+msg.findGroup("forward").get(2).asString()+yarp::os::ConstString("\"");
    }
    else
    {
        strCmd=yarp::os::ConstString("/bin/bash -l -c \"yarprun --readwrite ")+strUUID+"\"";
    }

    int pipe_child_to_parent[2];

    if (yarp::os::impl::pipe(pipe_child_to_parent))
    {
        int error=errno;

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=stdio\nCan't create pipe ")+strerror(error)
                                 +yarp::os::ConstString("\n");

        result.clear();
        result.addInt(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr, "%s", out.c_str());

        return YARPRUN_ERROR;
    }

    int c=0;
    char *command[16];
    for (int i=0; i<16; ++i) {
        command[i] = YARP_NULLPTR;
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

    int pid_cmd=yarp::os::fork();

    if (IS_INVALID(pid_cmd))
    {
        int error=errno;

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=stdio\nCan't fork stdout process because ")+strerror(error)
                                 +yarp::os::ConstString("\n");

        result.clear();
        result.addInt(YARPRUN_ERROR);
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

        int ret = yarp::os::impl::execvp("xterm", command);

        cmdclean(command);

        if (ret==YARPRUN_ERROR)
        {
            int error=errno;

            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                     +yarp::os::ConstString(" alias=")+strAlias
                                     +yarp::os::ConstString(" cmd=xterm\nCan't execute command because ")+strerror(error)
                                     +yarp::os::ConstString("\n");

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

        mStdioVector->Add(new YarpRunProcInfo(strAlias, mPortName, pid_cmd, YARP_NULLPTR, msg.check("hold")));

        result.clear();

        cmdclean(command);

        yarp::os::SystemClock::delaySystem(0.01);

        FILE* in_from_child=fdopen(pipe_child_to_parent[READ_FROM_PIPE], "r");
        int flags=fcntl(pipe_child_to_parent[READ_FROM_PIPE], F_GETFL, 0);
        fcntl(pipe_child_to_parent[READ_FROM_PIPE], F_SETFL, flags|O_NONBLOCK);
        yarp::os::ConstString out;

        if (in_from_child)
        {
            char buff[1024];

            while(true)
            {
                if (!fgets(buff, 1024, in_from_child) || ferror(in_from_child) || feof(in_from_child)) break;

                out+=yarp::os::ConstString(buff);
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
            out=yarp::os::ConstString("STARTED: server=")+mPortName
               +yarp::os::ConstString(" alias=")+strAlias
               +yarp::os::ConstString(" cmd=xterm pid=")+int2String(pid_cmd)
               +yarp::os::ConstString("\n");

        }

        fprintf(stderr, "%s", out.c_str());

        result.addInt(pid_cmd);
        result.addString(out.c_str());

        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

        return pid_cmd;
    }

    result.clear();
    result.addInt(YARPRUN_ERROR);

    return YARPRUN_ERROR;
}

int yarp::os::Run::executeCmd(yarp::os::Bottle& msg, yarp::os::Bottle& result)
{
    yarp::os::ConstString strAlias(msg.find("as").asString());
    yarp::os::ConstString strCmd(msg.find("cmd").toString());

    int  pipe_child_to_parent[2];
    int ret_pipe_child_to_parent=yarp::os::impl::pipe(pipe_child_to_parent);

    if (ret_pipe_child_to_parent!=0)
    {
        int error=errno;

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=stdio\nCan't create pipe ")+strerror(error)
                                 +yarp::os::ConstString("\n");


        result.addInt(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr, "%s", out.c_str());

        return YARPRUN_ERROR;
    }

    int pid_cmd=yarp::os::fork();

    if (IS_INVALID(pid_cmd))
    {
        int error=errno;

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=")+strCmd
                                 +yarp::os::ConstString("\nCan't fork command process because ")+strerror(error)
                                 +yarp::os::ConstString("\n");

        result.addInt(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr, "%s", out.c_str());

        return YARPRUN_ERROR;
    }

    if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
    {
        int saved_stderr = yarp::os::impl::dup(STDERR_FILENO);
        int null_file=open("/dev/null", O_WRONLY);
        REDIRECT_TO(STDOUT_FILENO, null_file);
        REDIRECT_TO(STDERR_FILENO, null_file);
        close(null_file);

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
        arg_str[nargs]=YARP_NULLPTR;

        if (msg.check("env"))
        {
            yarp::os::impl::SplitString ss(msg.find("env").asString().c_str(), ';');
            for(int i=0; i<ss.size(); i++) {
                char* szenv = new char[strlen(ss.get(i))+1];
                strcpy(szenv, ss.get(i));
                yarp::os::impl::putenv(szenv); // putenv doesn't make copy of the string
            }
        }

        if (msg.check("workdir"))
        {
            int ret = yarp::os::impl::chdir(msg.find("workdir").asString().c_str());

            if (ret!=0)
            {
                int error=errno;

                yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                         +yarp::os::ConstString(" alias=")+strAlias
                                         +yarp::os::ConstString(" cmd=")+strCmd
                                         +yarp::os::ConstString("\nCan't execute command, cannot set working directory ")+strerror(error)
                                         +yarp::os::ConstString("\n");

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
            for (int i=1; i<nargs; ++i) cwd_arg_str[i]=arg_str[i];
            cwd_arg_str[nargs]=YARP_NULLPTR;
            cwd_arg_str[0]=new char[strlen(currWorkDir)+strlen(arg_str[0])+16];


            strcpy(cwd_arg_str[0], currWorkDir);
            strcat(cwd_arg_str[0], "/");
            strcat(cwd_arg_str[0], arg_str[0]);

            //Why removing vectors and stop threads?
            //exec* never returns and memory is claimed by the system
            //furthermore after fork() only the thread which called fork() is forked!
//            cleanBeforeExec();

            ret = yarp::os::impl::execvp(cwd_arg_str[0], cwd_arg_str);

            delete [] cwd_arg_str[0];
            delete [] cwd_arg_str;
        }

        if (ret==YARPRUN_ERROR)
        {
            //Why removing vectors and stop threads?
            //exec* never returns and memory is claimed by the system
            //furthermore after fork() only the thread which called fork() is forked!
            //            cleanBeforeExec();
            ret = yarp::os::impl::execvp(arg_str[0], arg_str);
        }

        if (ret==YARPRUN_ERROR)
        {
            int error=errno;

            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                     +yarp::os::ConstString(" alias=")+strAlias
                                     +yarp::os::ConstString(" cmd=")+strCmd
                                     +yarp::os::ConstString("\nCan't execute command because ")+strerror(error)
                                     +yarp::os::ConstString("\n");

            FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE], "w");
            fprintf(out_to_parent, "%s", out.c_str());
            fflush(out_to_parent);
            fclose(out_to_parent);

            REDIRECT_TO(STDERR_FILENO, saved_stderr);
            fprintf(stderr, "%s", out.c_str());
        }

        delete [] cmd_str;
        delete [] arg_str;

        std::exit(ret);
    }

    if (IS_PARENT_OF(pid_cmd))
    {
        YarpRunProcInfo* pInf = new YarpRunProcInfo(strAlias, mPortName, pid_cmd, YARP_NULLPTR, false);
        pInf->setCmd(strCmd);
        if (msg.check("env")) pInf->setEnv(msg.find("env").asString());
        mProcessVector->Add(pInf);
        char pidstr[16];
        sprintf(pidstr, "%d", pid_cmd);

        yarp::os::SystemClock::delaySystem(0.01);

        FILE* in_from_child=fdopen(pipe_child_to_parent[READ_FROM_PIPE], "r");
        int flags=fcntl(pipe_child_to_parent[READ_FROM_PIPE], F_GETFL, 0);
        fcntl(pipe_child_to_parent[READ_FROM_PIPE], F_SETFL, flags|O_NONBLOCK);

        yarp::os::ConstString out;

        if (in_from_child)
        {
            char buff[1024];

            while(true)
            {
                if (!fgets(buff, 1024, in_from_child) || ferror(in_from_child) || feof(in_from_child)) break;

                out+=yarp::os::ConstString(buff);
            }

            fclose(in_from_child);
        }

        if (out.length()>0)
        {
            pid_cmd=YARPRUN_ERROR;
        }
        else
        {
            out=yarp::os::ConstString("STARTED: server=")+mPortName
               +yarp::os::ConstString(" alias=")+strAlias
               +yarp::os::ConstString(" cmd=")+strCmd
               +yarp::os::ConstString(" pid=")+int2String(pid_cmd)
               +yarp::os::ConstString("\n");
        }

        fprintf(stderr, "%s", out.c_str());

        result.addInt(pid_cmd);
        result.addString(out.c_str());

        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);
        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

        return pid_cmd;
    }

    result.addInt(YARPRUN_ERROR);

    return YARPRUN_ERROR;
}

#endif

/////////////////////////////////////////////////////////////////
// API
/////////////////////////////////////////////////////////////////

int yarp::os::Run::start(const yarp::os::ConstString &node, Property &command, yarp::os::ConstString &keyv)
{
    yarp::os::Bottle msg, grp, response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    yarp::os::ConstString dest_srv=node;

    if (command.check("stdio"))
    {
        dest_srv=yarp::os::ConstString(command.find("stdio").asString());

        grp.clear();
        grp.addString("stdio");
        grp.addString(dest_srv.c_str());
        msg.addList()=grp;

        if (command.check("geometry"))
        {
            grp.clear();
            grp.addString("geometry");
            grp.addString(command.find("geometry").asString().c_str());
            msg.addList()=grp;
        }

        if (command.check("hold"))
        {
            grp.clear();
            grp.addString("hold");
            msg.addList()=grp;
        }
    }

    grp.clear();
    grp.addString("as");
    grp.addString(keyv.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("cmd");
    grp.addString(command.find("name").asString().c_str());
    grp.addString(command.find("parameters").asString().c_str());
    msg.addList()=grp;

    printf(":: %s\n", msg.toString().c_str());

    response=sendMsg(msg, dest_srv.c_str());

    char buff[16];
    sprintf(buff, "%d", response.get(0).asInt());
    keyv=yarp::os::ConstString(buff);

    return response.get(0).asInt()>0?0:YARPRUN_ERROR;
}

int yarp::os::Run::sigterm(const yarp::os::ConstString &node, const yarp::os::ConstString &keyv)
{
    yarp::os::Bottle msg, grp, response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("sigterm");
    grp.addString(keyv.c_str());
    msg.addList()=grp;

    printf(":: %s\n", msg.toString().c_str());

    response=sendMsg(msg, node.c_str());

    return response.get(0).asString()=="sigterm OK"?0:YARPRUN_ERROR;
}

int yarp::os::Run::sigterm(const yarp::os::ConstString &node)
{
    yarp::os::Bottle msg, grp, response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("sigtermall");
    msg.addList()=grp;

    printf(":: %s\n", msg.toString().c_str());

    response=sendMsg(msg, node.c_str());

    return response.get(0).asString()=="sigtermall OK"?0:YARPRUN_ERROR;
}

int yarp::os::Run::kill(const yarp::os::ConstString &node, const yarp::os::ConstString &keyv, int s)
{
    yarp::os::Bottle msg, grp, response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("kill");
    grp.addString(keyv.c_str());
    grp.addInt(s);
    msg.addList()=grp;

    printf(":: %s\n", msg.toString().c_str());

    response=sendMsg(msg, node.c_str());

    return response.get(0).asString()=="kill OK"?0:YARPRUN_ERROR;
}

bool yarp::os::Run::isRunning(const yarp::os::ConstString &node, yarp::os::ConstString &keyv)
{
    yarp::os::Bottle msg, grp, response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("isrunning");
    grp.addString(keyv.c_str());
    msg.addList()=grp;

    printf(":: %s\n", msg.toString().c_str());

    response=sendMsg(msg, node.c_str());

    if (!response.size()) return false;

    return response.get(0).asString()=="running";
}

// end API

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

////// adapted from libYARP_OS: ResourceFinder.cpp
namespace fs = yarp::conf::filesystem;
constexpr auto sep = yarp::conf::environment::path_separator;
constexpr fs::value_type slash = fs::preferred_separator;

void sigint_handler(int sig)
{
    YARP_UNUSED(sig);
    yarp::run::Run::mStresstest=false;

    if (yarp::run::Run::pServerPort)
    {
        yarp::os::RpcServer *pClose=yarp::run::Run::pServerPort;
        yarp::run::Run::pServerPort = nullptr;
        pClose->close();
    }
    //else
    //{
    //}
}

static yarp::os::Bottle parsePaths(const std::string& txt)
{
    yarp::os::Bottle result;
    const char *at = txt.c_str();
    int slash_tweak = 0;
    int len = 0;
    for (char ch : txt) {
        if (ch==sep) {
            result.addString(std::string(at, len-slash_tweak));
            at += len+1;
            len = 0;
            slash_tweak = 0;
            continue;
        }
        slash_tweak = (ch==slash && len>0)?1:0;
        len++;
    }
    if (len>0) {
        result.addString(std::string(at, len-slash_tweak));
    }
    return result;
}

static bool fileExists(const char *fname)
{
    FILE *fp = nullptr;
    fp = fopen(fname, "r");
    if (!fp) {
        return false;
    } else {
        fclose(fp);
        return true;
    }
}

///////////////////////////
// WINDOWS SERVER
#if defined(_WIN32)
int yarp::run::Run::server()
{
    //yarp::os::Semaphore serializer(1); ///?????unused

    yarp::os::RpcServer port;

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

    yInfo() << "Yarprun successfully started on port: " << mPortName.c_str();

    pServerPort=&port;

    yarp::os::impl::signal(SIGINT, sigint_handler);
    yarp::os::impl::signal(SIGTERM, sigint_handler);

    // Enabling cpu load collector on windows
    //yarp::os::impl::SystemInfo::enableCpuLoadCollector();

    while (pServerPort)
    {
        yarp::os::Bottle msg;

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
            std::string strOnPort=msg.find("on").asString();
            std::string strStdioPort=msg.find("stdio").asString();

            if (strOnPort==mPortName)
            {
                std::string strUUID=mPortName+"/"+int2String(getpid())+"/"+msg.find("as").asString()+"-"+int2String(mProcCNT++);
                yarp::os::Bottle botUUID;
                botUUID.addString("stdiouuid");
                botUUID.addString(strUUID.c_str());
                msg.addList()=botUUID;

                if (mLogged || msg.check("log"))
                {
                    std::string strAlias=msg.find("as").asString();
                    std::string portName="/log";
                    portName+=mPortName+"/";
                    std::string command = msg.findGroup("cmd").get(1).asString();
                    command = command.substr(0, command.find(' '));
                    command = command.substr(command.find_last_of("\\/") + 1);
                    portName+=command;

                    yarp::os::Bottle botFwd;
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

                yarp::os::Bottle cmdResult;
                if (executeCmdAndStdio(msg, cmdResult)>0)
                {
                    if (strStdioPort==mPortName)
                    {
                        yarp::os::Bottle stdioResult;
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
                yarp::os::Bottle stdioResult;
                userStdio(msg, stdioResult);
                port.reply(stdioResult);
            }

            continue;
        }

        // without stdio
        if (msg.check("cmd"))
        {
            yarp::os::Bottle cmdResult;

            if (msg.check("log"))
            {
                yarp::os::Bottle botLogger=msg.findGroup("log");

                if (botLogger.size()>1)
                {
                    std::string loggerName=botLogger.get(1).asString();
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
            std::string alias(msg.findGroup("kill").get(1).asString());
            int sig=msg.findGroup("kill").get(2).asInt32();
            yarp::os::Bottle result;
            result.addString(mProcessVector.Signal(alias, sig)?"kill OK":"kill FAILED");
            port.reply(result);
            continue;
        }

        if (msg.check("sigterm"))
        {
            std::string alias(msg.find("sigterm").asString());
            yarp::os::Bottle result;
            result.addString(mProcessVector.Signal(alias, SIGTERM)?"sigterm OK":"sigterm FAILED");
            port.reply(result);
            continue;
        }

        if (msg.check("sigtermall"))
        {
            mProcessVector.Killall(SIGTERM);
            yarp::os::Bottle result;
            result.addString("sigtermall OK");
            port.reply(result);
            continue;
        }

        if (msg.check("ps"))
        {
            yarp::os::Bottle result;
            result.append(mProcessVector.PS());
            port.reply(result);
            continue;
        }

        if (msg.check("isrunning"))
        {
            std::string alias(msg.find("isrunning").asString());
            yarp::os::Bottle result;
            result.addString(mProcessVector.IsRunning(alias)?"running":"not running");
            port.reply(result);
            continue;
        }

        if (msg.check("killstdio"))
        {
            std::string alias(msg.find("killstdio").asString());
            mStdioVector.Signal(alias, SIGTERM);
            yarp::os::Bottle result;
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
            std::string fileName=msg.find("which").asString();
            if (fileName!="")
            {
                yarp::os::Bottle possiblePaths = parsePaths(yarp::conf::environment::get_string("PATH"));
                for (int i=0; i<possiblePaths.size(); ++i)
                {
                    std::string guessString=possiblePaths.get(i).asString() +
                    std::string{slash} + fileName;
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
            yInfo() << "Yarprun server closing...";
            pServerPort=0;
            yarp::os::Bottle result;
            result.addString("exit OK");
            port.reply(result);
            port.close();
        }
    }


    Run::mStdioVector.Killall(SIGTERM);

    Run::mProcessVector.Killall(SIGTERM);

    yInfo() << "Yarprun server closed";
    return 0;
}

///////////////////////
#else // LINUX SERVER
///////////////////////

void yarp::run::Run::cleanBeforeExec()
{
    // zombie hunter stop

    //yarp::os::impl::signal(SIGPIPE, SIG_IGN);
    //yarp::os::impl::signal(SIGCHLD, SIG_DFL);
    //yarp::os::impl::signal(SIGINT, SIG_DFL);
    //yarp::os::impl::signal(SIGTERM, SIG_DFL);

    if (mProcessVector)
    {
        YarpRunInfoVector *p=mProcessVector;
        mProcessVector = nullptr;
        delete p;
    }
    if (mStdioVector)
    {
        YarpRunInfoVector *p=mStdioVector;
        mStdioVector = nullptr;
        delete p;
    }
    if (mBraveZombieHunter)
    {
        ZombieHunterThread *p=mBraveZombieHunter;
        mBraveZombieHunter = nullptr;
        p->stop();
        delete p;
    }

    //yarp::os::Network::fini();
}

void yarp::run::Run::writeToPipe(int fd, std::string str)
{
    int len = str.length() + 1;
    int ret;
    ret = write(fd, &len, sizeof(len));
    if (ret != sizeof(len)) {
        fprintf(stderr, "Warning: could not write string length to pipe.\n");
    }
    ret = write(fd, str.c_str(), len);
    if (ret != len) {
        fprintf(stderr, "Warning: could not write string to pipe.\n");
    }
}

int yarp::run::Run::readFromPipe(int fd, char* &data, int& buffsize)
{
    int len=0;
    char* buff=(char*)&len;

    for (int c=4, r=0; c>0; c-=r)
    {
        r=read(fd, buff, c);

        if (r < 1) {
            return -1;
        }

        buff+=r;
    }

    if (len <= 0) {
        return 0;
    }

    if (len>buffsize)
    {
        delete [] data;
        data=new char[buffsize=1024+(len/1024)*1024];
    }

    buff=data;

    for (int c=len, r=0; c>0; c-=r)
    {
        r=read(fd, buff, c);

        if (r < 1) {
            return -1;
        }

        buff+=r;
    }

    return len;
}

static void sigchld_handler(int sig)
{
    YARP_UNUSED(sig);
    if (yarp::run::Run::mBraveZombieHunter)
    {
        yarp::run::Run::mBraveZombieHunter->sigchldHandler();
    }
}

int yarp::run::Run::server()
{
    int pipe_server2manager[2];
    int pipe_manager2server[2];

    if (yarp::run::impl::pipe(pipe_server2manager))
    {
        fprintf(stderr, "Can't open pipe because %s\n", strerror(errno));
        fflush(stderr);

        return YARPRUN_ERROR;
    }

    if (yarp::run::impl::pipe(pipe_manager2server))
    {
        fprintf(stderr, "Can't open pipe because %s\n", strerror(errno));
        fflush(stderr);

        return YARPRUN_ERROR;
    }

    int pid_process_manager=yarp::run::impl::fork();

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

        yarp::os::RpcServer port;

        if (!port.open(mPortName))
        {
            yError() << "Yarprun failed to open port: " << mPortName.c_str();

            if (mPortName[0] != '/') {
                yError("Invalid port name '%s', it should start with '/'\n", mPortName.c_str());
            }
            return YARPRUN_ERROR;
        }
        yarp::os::Bottle cmd, reply;
        cmd.addString("set");
        cmd.addString(port.getName());
        cmd.addString("yarprun");
        cmd.addString("true");

        yarp::os::impl::NameClient::getNameClient().send(cmd, reply);

        yInfo() << "Yarprun successfully started on port: " << mPortName.c_str();

        pServerPort=&port;

        yarp::os::impl::signal(SIGINT, sigint_handler);
        yarp::os::impl::signal(SIGTERM, sigint_handler);

        int rsp_size=1024;
        char *rsp_str=new char[rsp_size];

        yarp::os::Bottle msg, response;

        while (pServerPort)
        {
            RUNLOG("<<<port.read(msg, true)")
            if (!port.read(msg, true)) {
                break;
            }
            RUNLOG(">>>port.read(msg, true)")

            if (!pServerPort) {
                break;
            }

            if (msg.check("sysinfo"))
            {
                yarp::os::SystemInfoSerializer sysinfo;
                port.reply(sysinfo);
                continue;
            }

            if (msg.check("which"))
            {
                std::string fileName=msg.find("which").asString();
                if (fileName!="")
                {
                    yarp::os::Bottle possiblePaths = parsePaths(yarp::conf::environment::get_string("PATH"));
                    for (size_t i=0; i<possiblePaths.size(); ++i)
                    {
                        std::string guessString=possiblePaths.get(i).asString() + slash + fileName;
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
                pServerPort = nullptr;
                yarp::os::Bottle result;
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

        yarp::os::Bottle msg;

        //while(readFromPipe(pipe_server2manager[READ_FROM_PIPE], msg_str, msg_size)>0)
        while (true)
        {
            RUNLOG("<<<readFromPipe")
            if (readFromPipe(pipe_server2manager[READ_FROM_PIPE], msg_str, msg_size) <= 0) {
                break;
            }
            RUNLOG(">>>readFromPipe")

            //printf("<<< %s >>>\n", msg_str);
            //fflush(stdout);

            msg.fromString(msg_str);

            // command with stdio management
            if (msg.check("stdio"))
            {
                std::string strOnPort=msg.find("on").asString();
                std::string strStdioPort=msg.find("stdio").asString();

                if (strOnPort==mPortName)
                {
                    std::string strUUID=mPortName+"/"+int2String(getpid())+"/"+msg.find("as").asString()+"-"+int2String(mProcCNT++);
                    yarp::os::Bottle botUUID;
                    botUUID.addString("stdiouuid");
                    botUUID.addString(strUUID.c_str());
                    msg.addList()=botUUID;

                    if (mLogged || msg.check("log"))
                    {
                        std::string strAlias=msg.find("as").asString();
                        std::string portName="/log";
                        portName+=mPortName+"/";
                        std::string command = msg.findGroup("cmd").get(1).asString();
                        command = command.substr(0, command.find(' '));
                        command = command.substr(command.find_last_of("\\/") + 1);
                        portName+=command;

                        yarp::os::Bottle botFwd;
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
                        yarp::os::Network::connect(portName, mLoggerPort, style);
                    }

                    yarp::os::Bottle cmdResult;
                    if (executeCmdAndStdio(msg, cmdResult)>0)
                    {
                        if (strStdioPort==mPortName)
                        {
                            yarp::os::Bottle stdioResult;
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
                    yarp::os::Bottle stdioResult;
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
                yarp::os::Bottle cmdResult;

                if (msg.check("log"))
                {
                    yarp::os::Bottle botLogger=msg.findGroup("log");

                    if (botLogger.size()>1)
                    {
                        std::string loggerName=botLogger.get(1).asString();
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
                std::string alias(msg.findGroup("kill").get(1).asString());
                int sig=msg.findGroup("kill").get(2).asInt32();
                yarp::os::Bottle result;
                result.addString(mProcessVector->Signal(alias, sig)?"kill OK":"kill FAILED");
                RUNLOG("<<<writeToPipe")
                writeToPipe(pipe_manager2server[WRITE_TO_PIPE], result.toString());
                RUNLOG(">>>writeToPipe")
                continue;
            }

            if (msg.check("sigterm"))
            {
                std::string alias(msg.find("sigterm").asString());
                yarp::os::Bottle result;
                result.addString(mProcessVector->Signal(alias, SIGTERM)?"sigterm OK":"sigterm FAILED");
                RUNLOG("<<<writeToPipe")
                writeToPipe(pipe_manager2server[WRITE_TO_PIPE], result.toString());
                RUNLOG(">>>writeToPipe")
                continue;
            }

            if (msg.check("sigtermall"))
            {
                mProcessVector->Killall(SIGTERM);
                yarp::os::Bottle result;
                result.addString("sigtermall OK");

                RUNLOG("<<<writeToPipe")
                writeToPipe(pipe_manager2server[WRITE_TO_PIPE], result.toString());
                RUNLOG(">>>writeToPipe")
                continue;
            }

            if (msg.check("ps"))
            {
                yarp::os::Bottle result;
                result.append(mProcessVector->PS());
                RUNLOG("<<<writeToPipe")
                writeToPipe(pipe_manager2server[WRITE_TO_PIPE], result.toString());
                RUNLOG(">>>writeToPipe")
                continue;
            }

            if (msg.check("isrunning"))
            {
                std::string alias(msg.find("isrunning").asString());
                yarp::os::Bottle result;
                result.addString(mProcessVector->IsRunning(alias)?"running":"not running");
                RUNLOG("<<<writeToPipe")
                writeToPipe(pipe_manager2server[WRITE_TO_PIPE], result.toString());
                RUNLOG(">>>writeToPipe")
                continue;
            }

            if (msg.check("killstdio"))
            {
                std::string alias(msg.find("killstdio").asString());
                mStdioVector->Signal(alias, SIGTERM);
                yarp::os::Bottle result;
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
            mBraveZombieHunter = nullptr;
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

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007-2009 RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <signal.h>
#include <string>
#include <yarp/os/impl/RunReadWrite.h>
#include <yarp/os/impl/RunProcManager.h>
#include <yarp/os/impl/SystemInfo.h>
#include <yarp/os/impl/SystemInfoSerializer.h>

#include <yarp/os/Run.h>

#if defined(WIN32)
#  if !defined(WIN32_LEAN_AND_MEAN)
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#else
#  include <unistd.h>
#endif

#include <yarp/os/impl/RunCheckpoints.h>

#if defined(WIN32)
    HANDLE  hZombieHunter=NULL;
    HANDLE* aHandlesVector=NULL;
#endif

#if defined(WIN32)
inline yarp::os::ConstString lastError2String()
{
    int error=GetLastError();
    char buff[1024];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,error,0,buff,1024,NULL);
    return yarp::os::ConstString(buff);
}
#endif

///////////////////////////
// OS INDEPENDENT FUNCTIONS
///////////////////////////

/*
void dontexit(int sig)
{
    signal(SIGINT,dontexit);
    signal(SIGTERM,dontexit);
}
*/

YarpRunInfoVector yarp::os::Run::mProcessVector;
YarpRunInfoVector yarp::os::Run::mStdioVector;
yarp::os::ConstString yarp::os::Run::mPortName;
yarp::os::Port* yarp::os::Run::pServerPort=0;
int yarp::os::Run::mProcCNT=0;
yarp::os::Semaphore yarp::os::Run::serializer(1);

int yarp::os::Run::main(int argc, char *argv[])
{
    CHECK_ENTER("yarp::os::Run::main")

    mPortName="";

    if (!NetworkBase::getLocalMode())
    {
        CHECKPOINT()

        if (!NetworkBase::checkNetwork())
        {
            fprintf(stderr,"ERROR: no yarp network found.\n");

            CHECK_EXIT()

            return YARPRUN_ERROR;
        }
    }

    CHECKPOINT()

    Property config;
    config.fromCommand(argc,argv,false);

    if (config.check("echo"))
    {
        CHECKPOINT()

        char line[1024];
        fprintf(stderr,"Program echo started.\n");
        fflush(stderr);

        while(true)
        {
            int ret=scanf("%s",line);

            if (ret!=0)
            {
                fprintf(stderr,"%s\n",line);
                fflush(stderr);
            }
        }

        CHECK_EXIT()

        return 0;
    }

    /*
    if (config.check("block"))
    {
        signal(SIGINT,dontexit);
        signal(SIGTERM,dontexit);

        for (unsigned int i=0; i<0xFFFFFFFF; ++i)
        {
            fprintf(stdout,"%d\n",i);
            fflush(stdout);
            yarp::os::Time::delay(1.0);
        }
    }
    if (config.check("segfault"))
    {
        fprintf(stderr,"writing to forbidden location\n");
        fflush(stderr);

        int *zero=NULL;

        *zero=0;

        return 0;
    }
    if (config.check("wait"))
    {
        yarp::os::Time::delay(config.find("wait").asDouble());

        fprintf(stderr,"Done.\n");
        fflush(stderr);

        return 0;
    }
    */

    // HELP
    if (config.check("help"))
    {
        Help();

        CHECK_EXIT()

        return 0;
    }

    // SERVER
    if (config.check("server"))
    {
        CHECKPOINT()

        mPortName=yarp::os::ConstString(config.find("server").asString());

        int ret=Server();

        CHECK_EXIT();

        return ret;
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
     || config.check("sysinfo"))
    {
        CHECKPOINT()

        int ret=sendToServer(config);

        CHECK_EXIT()

        return ret;
    }

    if (config.check("readwrite"))
    {
        CHECKPOINT()

        yarp::os::ConstString uuid=config.findGroup("readwrite").get(1).asString();
        RunReadWrite rw;

        int ret=rw.loop(uuid);

        CHECK_EXIT()

        return ret;
    }

    if (config.check("write"))
    {
        CHECKPOINT()

        yarp::os::ConstString uuid=config.findGroup("write").get(1).asString();
        RunWrite w;

        int ret=w.loop(uuid);

        CHECK_EXIT()

        return ret;
    }

    if (config.check("read"))
    {
        CHECKPOINT()

        yarp::os::ConstString uuid=config.findGroup("read").get(1).asString();
        RunRead r;

        int ret=r.loop(uuid);

        CHECK_EXIT()

        return ret;
    }

    Help();

    CHECK_EXIT()

    return 0;
}

yarp::os::Bottle yarp::os::Run::SendMsg(Bottle& msg,yarp::os::ConstString target)
{
    CHECK_ENTER("yarp::os::Run::SendMsg")

    Port port;
    if (!port.open("..."))
    {
        CHECKPOINT()

        Bottle response;
        response.addString("RESPONSE:\n=========\n");
        response.addString("Cannot open port, aborting...\n");
        fprintf(stderr,"%s\n",response.toString().c_str());

        CHECK_EXIT()

        return response;
    }

    CHECKPOINT()

    bool connected=yarp::os::NetworkBase::connect(port.getName(), target);

    if (!connected)
    {
        CHECKPOINT()

        Bottle response;
        response.addString("RESPONSE:\n=========\n");
        response.addString("Cannot connect to remote server, aborting...\n");
        fprintf(stderr,"%s\n",response.toString().c_str());
        port.close();

        CHECK_EXIT()

        return response;
    }

    CHECKPOINT()

    Bottle response;
    port.write(msg,response);

    CHECKPOINT()

    NetworkBase::disconnect(port.getName().c_str(),target.c_str());

    CHECKPOINT()

    port.close();

    CHECKPOINT()

    int size=response.size();
    fprintf(stderr,"RESPONSE:\n=========\n");
    for (int s=0; s<size; ++s)
    {
        //if (response.get(s).isString())
        {
            fprintf(stderr,"%s\n",response.get(s).toString().c_str());
        }
    }

    CHECK_EXIT()

    return response;
}

void sigint_handler(int sig)
{
    CHECK_ENTER("sigint_handler")

    if (yarp::os::Run::pServerPort)
    {
        CHECKPOINT()

        yarp::os::Port *pClose=yarp::os::Run::pServerPort;
        yarp::os::Run::pServerPort=0;
        pClose->close();
    }

    CHECK_EXIT()
}

int yarp::os::Run::Server()
{
    CHECK_ENTER("yarp::os::Run::Server")

    Port port;
    if (!port.open(mPortName.c_str()))
    {
        CHECK_EXIT()

        return YARPRUN_ERROR;
    }
    pServerPort=&port;

    CHECKPOINT()

    #if !defined(WIN32)
    
    mProcessVector.start();
    mStdioVector.start();
        
    signal(SIGCHLD,sigchild_handler); 
    #endif

    CHECKPOINT()

    signal(SIGINT,sigint_handler);

    CHECKPOINT()

    signal(SIGTERM,sigint_handler);

    CHECKPOINT()

    #if !defined(WIN32)
    signal(SIGCHLD,sigchild_handler);
    #endif

	// Enabling cpu load collector on windows
	#if defined(WIN32)
	yarp::os::impl::SystemInfo::enableCpuLoadCollector();
	#endif

    CHECKPOINT()

    while (pServerPort)
    {
        CHECKPOINT()

        Bottle msg;
        port.read(msg,true);

        CHECKPOINT()

        printf("%s\n",msg.toString().c_str());
        fflush(stdout);

        if (!pServerPort)
        {
            CHECKPOINT()

            break;
        }

        // command with stdio management
        if (msg.check("stdio") && msg.check("cmd"))
        {
            CHECKPOINT()

            yarp::os::ConstString strStdioPort=msg.find("stdio").asString();
            yarp::os::ConstString strOnPort=msg.find("on").asString();

            // AM I THE CMD OR/AND STDIO SERVER?
            if (mPortName==strStdioPort) // stdio
            {
                Bottle botStdioResult;
                yarp::os::ConstString strStdioUUID;

                CHECKPOINT()
                serializer.wait();
                CHECKPOINT()
                int pidStdio=UserStdio(msg,botStdioResult,strStdioUUID);
                CHECKPOINT()
                serializer.post();
                CHECKPOINT()

                Bottle botStdioUUID;
                botStdioUUID.addString("stdiouuid");
                botStdioUUID.addString(strStdioUUID.c_str());
                msg.addList()=botStdioUUID;

                if (pidStdio>0)
                {
                    CHECKPOINT()

                    Bottle cmdResult;
                    if (mPortName==strOnPort)
                    {
                        // execute command here
                        CHECKPOINT()
                        serializer.wait();
                        CHECKPOINT()
                        cmdResult=ExecuteCmdAndStdio(msg);
                        CHECKPOINT()
                        serializer.post();
                        CHECKPOINT()
                    }
                    else
                    {
                        // execute command on cmd server
                        CHECKPOINT()
                        cmdResult=SendMsg(msg,strOnPort);
                        CHECKPOINT()
                    }

                    cmdResult.append(botStdioResult);
                    //yarp::os::Time::delay(10.0);
                    CHECKPOINT()
                    port.reply(cmdResult);
                    CHECKPOINT()
                }
                else
                {
                    CHECKPOINT()
                    port.reply(botStdioResult);
                    CHECKPOINT()
                }
            }
            else if (mPortName==yarp::os::ConstString(strOnPort)) // cmd
            {
                CHECKPOINT()
                serializer.wait();
                CHECKPOINT()
                Bottle cmdResult=ExecuteCmdAndStdio(msg);
                CHECKPOINT()
                serializer.post();
                CHECKPOINT()
                port.reply(cmdResult);
                CHECKPOINT()
            }
            else // some error (should never happen)
            {
                Bottle botFailure;
                botFailure.addInt(-1);
                CHECKPOINT()
                port.reply(botFailure);
                CHECKPOINT()
            }

            CHECKPOINT()
            continue;
        }

        // without stdio
        if (msg.check("cmd"))
        {
            CHECKPOINT()
            serializer.wait();
            CHECKPOINT()
            Bottle cmdResult=ExecuteCmd(msg);
            CHECKPOINT()
            serializer.post();
            CHECKPOINT()
            port.reply(cmdResult);
            CHECKPOINT()
            continue;
        }

        if (msg.check("kill"))
        {
            yarp::os::ConstString alias(msg.findGroup("kill").get(1).asString());
            int sig=msg.findGroup("kill").get(2).asInt();
            Bottle result;
            CHECKPOINT()
            result.addString(mProcessVector.Signal(alias,sig)?"kill OK":"kill FAILED");
            CHECKPOINT()
            port.reply(result);
            CHECKPOINT()
            continue;
        }

        if (msg.check("sigterm"))
        {
            yarp::os::ConstString alias(msg.find("sigterm").asString());
            Bottle result;
            CHECKPOINT()
            result.addString(mProcessVector.Signal(alias,SIGTERM)?"sigterm OK":"sigterm FAILED");
            CHECKPOINT()
            port.reply(result);
            CHECKPOINT()
            continue;
        }

        if (msg.check("sigtermall"))
        {
            CHECKPOINT()
            mProcessVector.Killall(SIGTERM);
            CHECKPOINT()
            Bottle result;
            result.addString("sigtermall OK");
            port.reply(result);
            CHECKPOINT()
            continue;
        }

        if (msg.check("ps"))
        {
            Bottle result;
            result.append(mProcessVector.PS());
            CHECKPOINT()
            port.reply(result);
            CHECKPOINT()
            continue;
        }

        if (msg.check("isrunning"))
        {
            yarp::os::ConstString alias(msg.find("isrunning").asString());
            Bottle result;
            CHECKPOINT()
            result.addString(mProcessVector.IsRunning(alias)?"running":"not running");
            CHECKPOINT()
            port.reply(result);
            CHECKPOINT()
            continue;
        }

        if (msg.check("killstdio"))
        {
            fprintf(stderr,"Run::Server() killstdio(%s)\n",msg.find("killstdio").asString().c_str());
            yarp::os::ConstString alias(msg.find("killstdio").asString());
            CHECKPOINT()
            mStdioVector.Signal(alias,SIGTERM);
            CHECKPOINT()
            continue;
        }

        if (msg.check("sysinfo"))
        {
            yarp::os::impl::SystemInfoSerializer sysinfo;
            CHECKPOINT()
            port.reply(sysinfo);
            CHECKPOINT()
            continue;
        }

        if (msg.check("exit"))
        {
            Bottle result;
            result.addString("exit OK");
            CHECKPOINT()
            port.reply(result);
            CHECKPOINT()
            port.close();
            CHECKPOINT()
            pServerPort=0;
        }
    }

    CHECKPOINT()
    Run::mStdioVector.Killall(SIGTERM);
    CHECKPOINT()
    Run::mProcessVector.Killall(SIGTERM);

    #if defined(WIN32)
    CHECKPOINT()
    Run::mProcessVector.mutex.wait();
    CHECKPOINT()
    if (hZombieHunter) TerminateThread(hZombieHunter,0);
    CHECKPOINT()
    Run::mProcessVector.mutex.post();
    CHECKPOINT()
    if (aHandlesVector) delete [] aHandlesVector;
    #else
    mProcessVector.stop();
    mStdioVector.stop();
    #endif

    CHECK_EXIT()

    return 0;
}

// CLIENT
int yarp::os::Run::sendToServer(yarp::os::Property& config)
{
    CHECK_ENTER("yarp::os::Run::sendToServer")

    yarp::os::Bottle msg;

    // USE A YARP RUN SERVER TO MANAGE STDIO
    //
    // client -> stdio server -> cmd server
    //
    if (config.check("cmd") && config.check("stdio"))
    {
        CHECKPOINT()

        if (config.find("stdio")=="")
        {
            Help("SYNTAX ERROR: missing remote stdio server\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        if (config.find("cmd")=="")
        {
            Help("SYNTAX ERROR: missing command\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        if (!config.check("as") || config.find("as")=="")
        {
            Help("SYNTAX ERROR: missing tag\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        if (!config.check("on") || config.find("on")=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        msg.addList()=config.findGroup("stdio");
        msg.addList()=config.findGroup("cmd");
        msg.addList()=config.findGroup("as");
        msg.addList()=config.findGroup("on");

        if (config.check("workdir")) msg.addList()=config.findGroup("workdir");
        if (config.check("geometry")) msg.addList()=config.findGroup("geometry");
        if (config.check("hold")) msg.addList()=config.findGroup("hold");
        if (config.check("env")) msg.addList()=config.findGroup("env");

        CHECKPOINT()

        Bottle response=SendMsg(msg,config.find("stdio").asString());

        CHECK_EXIT()

        if (!response.size()) return YARPRUN_ERROR;
        return response.get(0).asInt()>0?0:2;
    }

    // DON'T USE A RUN SERVER TO MANAGE STDIO
    //
    // client -> cmd server
    //
    if (config.check("cmd"))
    {
        CHECKPOINT()

        if (config.find("cmd").asString()=="")
        {
            Help("SYNTAX ERROR: missing command\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        if (!config.check("as") || config.find("as").asString()=="")
        {
            Help("SYNTAX ERROR: missing tag\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        msg.addList()=config.findGroup("cmd");
        msg.addList()=config.findGroup("as");

        if (config.check("workdir")) msg.addList()=config.findGroup("workdir");
        if (config.check("env")) msg.addList()=config.findGroup("env");

        CHECKPOINT()

        Bottle response=SendMsg(msg,config.find("on").asString());

        CHECK_EXIT()

        if (!response.size()) return YARPRUN_ERROR;
        return response.get(0).asInt()>0?0:2;
    }

    // client -> cmd server
    if (config.check("kill"))
    {
        CHECKPOINT()

        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        if (config.findGroup("kill").get(1).asString()=="")
        {
            Help("SYNTAX ERROR: missing tag\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        if (config.findGroup("kill").get(2).asInt()==0)
        {
            Help("SYNTAX ERROR: missing signum\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        msg.addList()=config.findGroup("kill");

        CHECKPOINT()

        Bottle response=SendMsg(msg,config.find("on").asString());

        CHECK_EXIT()

        if (!response.size()) return YARPRUN_ERROR;
        return response.get(0).asString()=="kill OK"?0:2;
    }

    // client -> cmd server
    if (config.check("sigterm"))
    {
        CHECKPOINT()

        if (config.find("sigterm").asString()=="")
        {
            Help("SYNTAX ERROR: missing tag");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        msg.addList()=config.findGroup("sigterm");

        CHECKPOINT()

        Bottle response=SendMsg(msg,config.find("on").asString());

        CHECK_EXIT()

        if (!response.size()) return YARPRUN_ERROR;
        return response.get(0).asString()=="sigterm OK"?0:2;
    }

    // client -> cmd server
    if (config.check("sigtermall"))
    {
        CHECKPOINT()

        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        msg.addList()=config.findGroup("sigtermall");

        CHECKPOINT()

        Bottle response=SendMsg(msg,config.find("on").asString());

        CHECK_EXIT()

        if (!response.size()) return YARPRUN_ERROR;
        return 0;
    }

    if (config.check("ps"))
    {
        CHECKPOINT()

        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        msg.addList()=config.findGroup("ps");

        CHECKPOINT()

        Bottle response=SendMsg(msg,config.find("on").asString());

        CHECK_EXIT()

        if (!response.size()) return YARPRUN_ERROR;
        return 0;
    }

    if (config.check("isrunning"))
    {
        CHECKPOINT()

        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        if (config.find("isrunning").asString()=="")
        {
            Help("SYNTAX ERROR: missing tag\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        msg.addList()=config.findGroup("isrunning");

        CHECKPOINT()

        Bottle response=SendMsg(msg,config.find("on").asString());

        CHECK_EXIT()

        if (!response.size()) return YARPRUN_ERROR;
        return response.get(0).asString()=="running"?0:2;
    }

    if(config.check("sysinfo"))
    {
        CHECKPOINT()

        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        msg.addList()=config.findGroup("sysinfo");

        Port port;
        port.setTimeout(5.0);
        if (!port.open("..."))
        {
            fprintf(stderr, "RESPONSE:\n=========\n");
            fprintf(stderr, "Cannot open port, aborting...\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        CHECKPOINT()

        bool connected = yarp::os::NetworkBase::connect(port.getName(),config.find("on").asString());

        if(!connected)
        {
            CHECKPOINT()

            fprintf(stderr, "RESPONSE:\n=========\n");
            fprintf(stderr, "Cannot connect to remote server, aborting...\n");
            port.close();

            CHECK_EXIT()

            return YARPRUN_ERROR;
        }

        yarp::os::impl::SystemInfoSerializer info;

        CHECKPOINT()
        int ret = port.write(msg, info);
        CHECKPOINT()
        NetworkBase::disconnect(port.getName().c_str(),config.find("on").asString());
        CHECKPOINT()
        port.close();
        CHECKPOINT()

        fprintf(stdout, "RESPONSE:\n=========\n\n");

        if(!ret)
        {
            fprintf(stdout, "No response. (timeout)\n");

            CHECK_EXIT()
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

        //fprintf(stdout, "Network IP4 : %s\n", info.network.ip4.c_str());
        //fprintf(stdout, "Network IP6 : %s\n", info.network.ip6.c_str());
        //fprintf(stdout, "Network mac : %s\n\n", info.network.mac.c_str());

        CHECK_EXIT()

        return 0;
    }

    if (config.check("exit"))
    {
        CHECKPOINT()

        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            CHECK_EXIT()
            return YARPRUN_ERROR;
        }

        msg.addList()=config.findGroup("exit");

        CHECKPOINT()

        Bottle response=SendMsg(msg,config.find("on").asString());

        CHECK_EXIT()

        if (!response.size()) return YARPRUN_ERROR;
        return 0;
    }

    CHECK_EXIT()

    return 0;
}

void yarp::os::Run::Help(const char *msg)
{
    CHECK_ENTER("yarp::os::Run::Help")

    fprintf(stderr,"%s",msg);
    fprintf(stderr,"\nUSAGE:\n\n");
    fprintf(stderr,"yarp run --server SERVERPORT\nrun a server on the local machine\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --as TAG --cmd COMMAND [ARGLIST] [--workdir WORKDIR] [--env ENVIRONMENT]\nrun a command on SERVERPORT server\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --as TAG --stdio STDIOSERVERPORT [--hold] [--geometry WxH+X+Y] --cmd COMMAND [ARGLIST] [--workdir WORKDIR] [--env ENVIRONMENT]\n");
    fprintf(stderr,"run a command on SERVERPORT server sending I/O to STDIOSERVERPORT server\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --kill TAG SIGNUM\nsend SIGNUM signal to TAG command\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --sigterm TAG\nterminate TAG command\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --sigtermall\nterminate all commands\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --ps\nreport commands running on SERVERPORT\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --isrunning TAG\nTAG command is running?\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --sysinfo\nreport system information of SERVERPORT\n\n");
    fprintf(stderr,"yarp run --on SERVERPORT --exit\nstop SERVERPORT server\n\n");

    CHECK_EXIT()
}

/////////////////////////
// OS DEPENDENT FUNCTIONS
/////////////////////////

// WINDOWS

#if defined(WIN32)

// CMD SERVER
yarp::os::Bottle yarp::os::Run::ExecuteCmdAndStdio(Bottle& msg)
{
    CHECK_ENTER("yarp::os::Run::ExecuteCmdAndStdio")

    yarp::os::ConstString strAlias=msg.find("as").asString();
    yarp::os::ConstString strStdio=msg.find("stdio").asString();
    yarp::os::ConstString strStdioUUID=msg.find("stdiouuid").asString();
    //yarp::os::ConstString strCmdUUID=mPortName+"/"+int2String(GetCurrentProcessId())+"/"+strAlias+"-"+int2String(mProcCNT++);

    // PIPES
    SECURITY_ATTRIBUTES pipe_sec_attr;
    pipe_sec_attr.nLength=sizeof(SECURITY_ATTRIBUTES);
    pipe_sec_attr.bInheritHandle=TRUE;
    pipe_sec_attr.lpSecurityDescriptor=NULL;
    HANDLE read_from_pipe_stdin_to_cmd,write_to_pipe_stdin_to_cmd;
    CreatePipe(&read_from_pipe_stdin_to_cmd,&write_to_pipe_stdin_to_cmd,&pipe_sec_attr,0);
    HANDLE read_from_pipe_cmd_to_stdout,write_to_pipe_cmd_to_stdout;
    CreatePipe(&read_from_pipe_cmd_to_stdout,&write_to_pipe_cmd_to_stdout,&pipe_sec_attr,0);

    CHECKPOINT()

    // RUN STDOUT
    PROCESS_INFORMATION stdout_process_info;
    ZeroMemory(&stdout_process_info,sizeof(PROCESS_INFORMATION));
    STARTUPINFO stdout_startup_info;
    ZeroMemory(&stdout_startup_info,sizeof(STARTUPINFO));

    stdout_startup_info.cb=sizeof(STARTUPINFO);
    stdout_startup_info.hStdError=stderr;
    stdout_startup_info.hStdOutput=stdout;
    stdout_startup_info.hStdInput=read_from_pipe_cmd_to_stdout;
    stdout_startup_info.dwFlags|=STARTF_USESTDHANDLES;

    CHECKPOINT()

    BOOL bSuccess=CreateProcess(NULL,   // command name
                                (char*)(yarp::os::ConstString("yarprun --write ")+strStdioUUID).c_str(), // command line
                                NULL,          // process security attributes
                                NULL,          // primary thread security attributes
                                TRUE,          // handles are inherited
                                CREATE_NEW_PROCESS_GROUP, // creation flags
                                NULL,          // use parent's environment
                                NULL,          // use parent's current directory
                                &stdout_startup_info,   // STARTUPINFO pointer
                                &stdout_process_info);  // receives PROCESS_INFORMATION

    CHECKPOINT()

    if (!bSuccess)
    {
        CHECKPOINT()

        yarp::os::ConstString strError=yarp::os::ConstString("ABORTED: server=")+mPortName
                                      +yarp::os::ConstString(" alias=")+strAlias
                                      +yarp::os::ConstString(" cmd=stdout\n")
                                      +yarp::os::ConstString("Can't execute stdout because ")+lastError2String()
                                      +yarp::os::ConstString("\n");

        Bottle result;
        result.addInt(YARPRUN_ERROR);
        result.addString(strError.c_str());
        fprintf(stderr,"%s",strError.c_str());
        fflush(stderr);

        CloseHandle(write_to_pipe_stdin_to_cmd);
        CloseHandle(read_from_pipe_stdin_to_cmd);
        CloseHandle(write_to_pipe_cmd_to_stdout);
        CloseHandle(read_from_pipe_cmd_to_stdout);

        CHECK_EXIT()

        return result;
    }

    // RUN STDIN

    CHECKPOINT()

    PROCESS_INFORMATION stdin_process_info;
    ZeroMemory(&stdin_process_info,sizeof(PROCESS_INFORMATION));
    STARTUPINFO stdin_startup_info;
    ZeroMemory(&stdin_startup_info,sizeof(STARTUPINFO));

    stdin_startup_info.cb=sizeof(STARTUPINFO);
    stdin_startup_info.hStdError=write_to_pipe_stdin_to_cmd;
    stdin_startup_info.hStdOutput=write_to_pipe_stdin_to_cmd;
    stdin_startup_info.hStdInput=stdin;
    stdin_startup_info.dwFlags|=STARTF_USESTDHANDLES;

    CHECKPOINT()

    bSuccess=CreateProcess(NULL,    // command name
                           (char*)(yarp::os::ConstString("yarprun --read ")+strStdioUUID).c_str(), // command line
                           NULL,          // process security attributes
                           NULL,          // primary thread security attributes
                           TRUE,          // handles are inherited
                           CREATE_NEW_PROCESS_GROUP, // creation flags
                           NULL,          // use parent's environment
                           NULL,          // use parent's current directory
                           &stdin_startup_info,   // STARTUPINFO pointer
                           &stdin_process_info);  // receives PROCESS_INFORMATION

    CHECKPOINT()

    if (!bSuccess)
    {
        CHECKPOINT()

        yarp::os::ConstString strError=yarp::os::ConstString("ABORTED: server=")+mPortName
                                      +yarp::os::ConstString(" alias=")+strAlias
                                      +yarp::os::ConstString(" cmd=stdin\n")
                                      +yarp::os::ConstString("Can't execute stdin because ")+lastError2String()
                                      +yarp::os::ConstString("\n");

        Bottle result;
        result.addInt(YARPRUN_ERROR);
        result.addString(strError.c_str());
        fprintf(stderr,"%s",strError.c_str());
        fflush(stderr);

        CHECKPOINT()

        TerminateProcess(stdout_process_info.hProcess,YARPRUN_ERROR);

        CHECKPOINT()

        CloseHandle(stdout_process_info.hProcess);

        CHECKPOINT()

        CloseHandle(write_to_pipe_stdin_to_cmd);
        CloseHandle(read_from_pipe_stdin_to_cmd);
        CloseHandle(write_to_pipe_cmd_to_stdout);
        CloseHandle(read_from_pipe_cmd_to_stdout);

        CHECK_EXIT()

        return result;
    }

    // RUN COMMAND

    CHECKPOINT()

    PROCESS_INFORMATION cmd_process_info;
    ZeroMemory(&cmd_process_info,sizeof(PROCESS_INFORMATION));
    STARTUPINFO cmd_startup_info;
    ZeroMemory(&cmd_startup_info,sizeof(STARTUPINFO));

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

    CHECKPOINT()

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

    CHECKPOINT()

    // adding new env variables
    yarp::os::ConstString cstrEnvName;
    if(msg.check("env"))
    {
        lstrcpy(lpNew, (LPTCH) msg.find("env").asString().c_str());
        lpNew += lstrlen(lpNew) + 1;
    }

    // closing env block
    *lpNew = (TCHAR)0;

    bool bWorkdir=msg.check("workdir");
    yarp::os::ConstString strWorkdir=bWorkdir?msg.find("workdir").asString()+"\\":"";

    CHECKPOINT()

    bSuccess=CreateProcess(NULL,    // command name
                                (char*)(strWorkdir+strCmd).c_str(), // command line
                                NULL,          // process security attributes
                                NULL,          // primary thread security attributes
                                TRUE,          // handles are inherited
                                CREATE_NEW_PROCESS_GROUP, // creation flags
                                (LPVOID) chNewEnv,        // use new environemnt list
                                bWorkdir?strWorkdir.c_str():NULL, // working directory
                                &cmd_startup_info,   // STARTUPINFO pointer
                                &cmd_process_info);  // receives PROCESS_INFORMATION


    CHECKPOINT()

    if (!bSuccess && bWorkdir)
    {
        CHECKPOINT()

            bSuccess=CreateProcess(NULL,    // command name
                                    (char*)(strCmd.c_str()), // command line
                                    NULL,          // process security attributes
                                    NULL,          // primary thread security attributes
                                    TRUE,          // handles are inherited
                                    CREATE_NEW_PROCESS_GROUP, // creation flags
                                    (LPVOID) chNewEnv,        // use new environemnt list
                                    strWorkdir.c_str(), // working directory
                                    &cmd_startup_info,   // STARTUPINFO pointer
                                    &cmd_process_info);  // receives PROCESS_INFORMATION
    }

    CHECKPOINT()

    // deleting old environment variable
    FreeEnvironmentStrings(chOldEnv);

    CHECKPOINT()

    if (!bSuccess)
    {
        CHECKPOINT()

        Bottle result;
        result.addInt(YARPRUN_ERROR);

        DWORD nBytes;
        yarp::os::ConstString line1=yarp::os::ConstString("ABORTED: server=")+mPortName
                                   +yarp::os::ConstString(" alias=")+strAlias
                                   +yarp::os::ConstString(" cmd=")+strCmd
                                   +yarp::os::ConstString("pid=")+int2String(cmd_process_info.dwProcessId)
                                   +yarp::os::ConstString("\n");

        WriteFile(write_to_pipe_cmd_to_stdout,line1.c_str(),line1.length(),&nBytes,0);

        yarp::os::ConstString line2=yarp::os::ConstString("Can't execute command because ")+lastError2String()+yarp::os::ConstString("\n");
        WriteFile(write_to_pipe_cmd_to_stdout,line1.c_str(),line2.length(),&nBytes,0);
        FlushFileBuffers(write_to_pipe_cmd_to_stdout);

        CHECKPOINT()

        yarp::os::ConstString out=line1+line2;
        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);

        CloseHandle(write_to_pipe_stdin_to_cmd);
        CloseHandle(read_from_pipe_stdin_to_cmd);
        CloseHandle(write_to_pipe_cmd_to_stdout);
        CloseHandle(read_from_pipe_cmd_to_stdout);

        CHECKPOINT()

        TerminateProcess(stdout_process_info.hProcess,YARPRUN_ERROR);

        CHECKPOINT()

        CloseHandle(stdout_process_info.hProcess);

        CHECKPOINT()

        TerminateProcess(stdin_process_info.hProcess,YARPRUN_ERROR);

        CHECKPOINT()

        CloseHandle(stdin_process_info.hProcess);

        CHECK_EXIT()

        return result;
    }

    CHECKPOINT()

    FlushFileBuffers(write_to_pipe_cmd_to_stdout);

    CHECKPOINT()

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
    if(msg.check("env"))
    {
        pInf->setEnv(msg.find("env").asString());
    }
    mProcessVector.Add(pInf);

    CHECKPOINT()

    Bottle result;
    result.addInt(cmd_process_info.dwProcessId);
    yarp::os::ConstString out=yarp::os::ConstString("STARTED: server=")+mPortName
                             +yarp::os::ConstString(" alias=")+strAlias
                             +yarp::os::ConstString(" cmd=")+strCmd
                             +yarp::os::ConstString("pid=")+int2String(cmd_process_info.dwProcessId)
                             +yarp::os::ConstString("\n");

    result.addString(out.c_str());
    result.addString(strStdioUUID.c_str());
    fprintf(stderr,"%s",out.c_str());

    CHECK_EXIT()

    return result;
}

yarp::os::Bottle yarp::os::Run::ExecuteCmd(yarp::os::Bottle& msg)
{
    CHECK_ENTER("yarp::os::Run::ExecuteCmd")

    yarp::os::ConstString strAlias=msg.find("as").asString().c_str();

    // RUN COMMAND
    PROCESS_INFORMATION cmd_process_info;
    ZeroMemory(&cmd_process_info,sizeof(PROCESS_INFORMATION));
    STARTUPINFO cmd_startup_info;
    ZeroMemory(&cmd_startup_info,sizeof(STARTUPINFO));

    cmd_startup_info.cb=sizeof(STARTUPINFO);

    Bottle botCmd=msg.findGroup("cmd").tail();

    yarp::os::ConstString strCmd;
    for (int s=0; s<botCmd.size(); ++s)
    {
        strCmd+=botCmd.get(s).toString()+yarp::os::ConstString(" ");
    }

    CHECKPOINT()

    /*
    if(msg.check("env"))
    {
        int pos = msg.find("env").asString().find("=");
        if(pos)
        {
            yarp::os::ConstString cstrName = msg.find("env").asString().substr(0,pos);
            int nValue = msg.find("env").asString().length() -
                         cstrName.length() - 1;
            yarp::os::ConstString cstrValue = msg.find("env").asString().substr(pos+1,nValue);
            SetEnvironmentVariable(cstrName.c_str(), cstrValue.c_str());
        }

    } */

    /*
     * setting environment variable for child process
     */
    TCHAR chNewEnv[32767];

    // Get a pointer to the env block.
    LPTCH chOldEnv = GetEnvironmentStrings();

    CHECKPOINT()

    // copying parent env variables
    LPTSTR lpOld = (LPTSTR) chOldEnv;
    LPTSTR lpNew = (LPTSTR) chNewEnv;
    while (*lpOld)
    {
        lstrcpy(lpNew, lpOld);
        lpOld += lstrlen(lpOld) + 1;
        lpNew += lstrlen(lpNew) + 1;
    }

    CHECKPOINT()

    // adding new env variables
    yarp::os::ConstString cstrEnvName;
    if(msg.check("env"))
    {
        lstrcpy(lpNew, (LPTCH) msg.find("env").asString().c_str());
        lpNew += lstrlen(lpNew) + 1;
    }

    // closing env block
    *lpNew = (TCHAR)0;

    bool bWorkdir=msg.check("workdir");
    yarp::os::ConstString strWorkdir=bWorkdir?msg.find("workdir").asString()+"\\":"";

    CHECKPOINT()

    BOOL bSuccess=CreateProcess(NULL,   // command name
                                (char*)(strWorkdir+strCmd).c_str(), // command line
                                NULL,          // process security attributes
                                NULL,          // primary thread security attributes
                                TRUE,          // handles are inherited
                                CREATE_NEW_PROCESS_GROUP, // creation flags
                                (LPVOID) chNewEnv, // use new environment
                                bWorkdir?strWorkdir.c_str():NULL, // working directory
                                &cmd_startup_info,   // STARTUPINFO pointer
                                &cmd_process_info);  // receives PROCESS_INFORMATION

    CHECKPOINT()

    if (!bSuccess && bWorkdir)
    {
        CHECKPOINT()

            bSuccess=CreateProcess(NULL,    // command name
                                    (char*)(strCmd.c_str()), // command line
                                    NULL,          // process security attributes
                                    NULL,          // primary thread security attributes
                                    TRUE,          // handles are inherited
                                    CREATE_NEW_PROCESS_GROUP, // creation flags
                                    (LPVOID) chNewEnv, // use new environment
                                    strWorkdir.c_str(), // working directory
                                    &cmd_startup_info,   // STARTUPINFO pointer
                                    &cmd_process_info);  // receives PROCESS_INFORMATION
    }

    // deleting old environment variable
    FreeEnvironmentStrings(chOldEnv);

    CHECKPOINT()

    if (!bSuccess)
    {
        CHECKPOINT()

        Bottle result;
        result.addInt(YARPRUN_ERROR);

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=")+strCmd
                                 +yarp::os::ConstString(" pid=")+int2String(cmd_process_info.dwProcessId)
                                 +yarp::os::ConstString("\nCan't execute command because ")+lastError2String()
                                 +yarp::os::ConstString("\n");

        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);

        CHECK_EXIT()

        return result;
    }

    CHECKPOINT()

    // EVERYTHING IS ALL RIGHT
    YarpRunProcInfo* pInf = new YarpRunProcInfo(strAlias,
                                           mPortName,
                                           cmd_process_info.dwProcessId,
                                           cmd_process_info.hProcess,
                                           false);
    pInf->setCmd(strCmd);
    if(msg.check("env"))
        pInf->setEnv(msg.find("env").asString());

    mProcessVector.Add(pInf);

    CHECKPOINT()

    Bottle result;
    result.addInt(cmd_process_info.dwProcessId);
    yarp::os::ConstString out=yarp::os::ConstString("STARTED: server=")+mPortName
                             +yarp::os::ConstString(" alias=")+strAlias
                             +yarp::os::ConstString(" cmd=")+strCmd
                             +yarp::os::ConstString("pid=")+int2String(cmd_process_info.dwProcessId)
                             +yarp::os::ConstString("\n");

    fprintf(stderr,"%s",out.c_str());

    CHECK_EXIT()

    return result;
}

// STDIO SERVER
int yarp::os::Run::UserStdio(yarp::os::Bottle& msg,yarp::os::Bottle& result,yarp::os::ConstString& strStdioPortUUID)
{
    CHECK_ENTER("yarp::os::Run::UserStdio")

    PROCESS_INFORMATION stdio_process_info;
    ZeroMemory(&stdio_process_info,sizeof(PROCESS_INFORMATION));

    STARTUPINFO stdio_startup_info;
    ZeroMemory(&stdio_startup_info,sizeof(STARTUPINFO));
    stdio_startup_info.cb=sizeof(STARTUPINFO);
    stdio_startup_info.wShowWindow=SW_SHOWNOACTIVATE;
    stdio_startup_info.dwFlags=STARTF_USESHOWWINDOW;

    yarp::os::ConstString strAlias=msg.find("as").asString();
    strStdioPortUUID=mPortName+"/"+int2String(GetCurrentProcessId())+"/"+strAlias+"-"+int2String(mProcCNT++);
    yarp::os::ConstString strCmd=yarp::os::ConstString("yarprun --readwrite ")+strStdioPortUUID;

    CHECKPOINT()

    BOOL bSuccess=CreateProcess(NULL,   // command name
                                (char*)strCmd.c_str(), // command line
                                NULL,          // process security attributes
                                NULL,          // primary thread security attributes
                                TRUE,          // handles are inherited
                                CREATE_NEW_CONSOLE, // creation flags
                                NULL,          // use parent's environment
                                NULL,          // use parent's current directory
                                &stdio_startup_info,   // STARTUPINFO pointer
                                &stdio_process_info);  // receives PROCESS_INFORMATION

    CHECKPOINT()

    yarp::os::ConstString out;

    if (bSuccess)
    {
        CHECKPOINT()

        mStdioVector.Add(new YarpRunProcInfo(strAlias,
                                             mPortName,
                                             stdio_process_info.dwProcessId,
                                             stdio_process_info.hProcess,
                                             false));

        out=yarp::os::ConstString("STARTED: server=")+mPortName
           +yarp::os::ConstString(" alias=")+strAlias
           +yarp::os::ConstString(" cmd=stdio pid=")+int2String(stdio_process_info.dwProcessId)
           +yarp::os::ConstString("\n");

        CHECKPOINT()
    }
    else
    {
        CHECKPOINT()

        stdio_process_info.dwProcessId=YARPRUN_ERROR;

        out=yarp::os::ConstString("ABORTED: server=")+mPortName
           +yarp::os::ConstString(" alias=")+strAlias
           +yarp::os::ConstString(" cmd=stdio\n")
           +yarp::os::ConstString("Can't open stdio window because ")+lastError2String()
           +yarp::os::ConstString("\n");

        CHECKPOINT()
    }

    result.clear();
    result.addInt(stdio_process_info.dwProcessId);
    result.addString(out.c_str());
    fprintf(stderr,"%s",out.c_str());
    fflush(stderr);

    CHECK_EXIT()

    return stdio_process_info.dwProcessId;
}

////////////////
#else // LINUX
////////////////

#define READ_FROM_PIPE 0
#define WRITE_TO_PIPE  1
#define REDIRECT_TO(from,to) dup2(to,from)

static int CountArgs(char *str)
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

static void ParseCmd(char* cmd_str,char** arg_str)
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

void yarp::os::Run::CleanZombies()
{
    CHECK_ENTER("yarp::os::Run::CleanZombies")

    mProcessVector.CleanZombies();

    CHECKPOINT()

    mStdioVector.CleanZombies();

    CHECK_EXIT()
}

//////////////////////////////////////////////////////////////////////////////////////////

yarp::os::Bottle yarp::os::Run::ExecuteCmdAndStdio(yarp::os::Bottle& msg)
{
    CHECK_ENTER("yarp::os::Run::ExecuteCmdAndStdio")

    yarp::os::ConstString strAlias=msg.find("as").asString();
    yarp::os::ConstString strCmd=msg.find("cmd").asString();
    yarp::os::ConstString strStdio=msg.find("stdio").asString();
    yarp::os::ConstString strStdioUUID=msg.find("stdiouuid").asString();

    int  pipe_stdin_to_cmd[2];
    int ret_stdin_to_cmd=pipe(pipe_stdin_to_cmd);

    int  pipe_cmd_to_stdout[2];
    int  ret_cmd_to_stdout=pipe(pipe_cmd_to_stdout);

    int  pipe_child_to_parent[2];
    int  ret_child_to_parent=pipe(pipe_child_to_parent);

    CHECKPOINT()

    if (ret_child_to_parent!=0 || ret_cmd_to_stdout!=0 || ret_stdin_to_cmd!=0)
    {
        int error=errno;

        CHECKPOINT()

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=stdout\n")
                                 +yarp::os::ConstString("Can't create pipes ")+strerror(error)
                                 +yarp::os::ConstString("\n");

        Bottle result;
        result.addInt(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);

        CHECK_EXIT()

        return result;
    }

    CHECKPOINT()

    int pid_stdout=fork();

    CHECKPOINT()

    if (IS_INVALID(pid_stdout))
    {
        int error=errno;

        CHECKPOINT()

        CLOSE(pipe_stdin_to_cmd[WRITE_TO_PIPE]);
        CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
        CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);
        CLOSE(pipe_cmd_to_stdout[READ_FROM_PIPE]);
        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);
        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

        CHECKPOINT()

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=stdout\n")
                                 +yarp::os::ConstString("Can't fork stdout process because ")+strerror(error)
                                 +yarp::os::ConstString("\n");

        Bottle result;
        result.addInt(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);

        CHECK_EXIT()

        return result;
    }

    if (IS_NEW_PROCESS(pid_stdout)) // STDOUT IMPLEMENTED HERE
    {
        CHECKPOINT()

        REDIRECT_TO(STDIN_FILENO,pipe_cmd_to_stdout[READ_FROM_PIPE]);

        CLOSE(pipe_stdin_to_cmd[WRITE_TO_PIPE]);
        CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
        CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);
        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

        CHECKPOINT()

        int ret=execlp("yarprun","yarprun","--write",strStdioUUID.c_str(),(char*)NULL);

        CHECKPOINT()

        CLOSE(pipe_cmd_to_stdout[READ_FROM_PIPE]);

        CHECKPOINT()

        if (ret==YARPRUN_ERROR)
        {
            int error=errno;

            CHECKPOINT()

            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                     +yarp::os::ConstString(" alias=")+strAlias
                                     +yarp::os::ConstString(" cmd=stdout\n")
                                     +yarp::os::ConstString("Can't execute stdout because ")+strerror(error)
                                     +yarp::os::ConstString("\n");

            FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
            fprintf(out_to_parent,"%s",out.c_str());
            fflush(out_to_parent);
            fclose(out_to_parent);

            fprintf(stderr,"%s",out.c_str());
            fflush(stderr);

            CHECKPOINT()
        }

        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

        CHECK_EXIT()

        exit(ret);
    }

    if (IS_PARENT_OF(pid_stdout))
    {
        CHECKPOINT()

        CLOSE(pipe_cmd_to_stdout[READ_FROM_PIPE]);

        fprintf(stderr,"STARTED: server=%s alias=%s cmd=stdout pid=%d\n",mPortName.c_str(),strAlias.c_str(),pid_stdout);
        fflush(stderr);

        CHECKPOINT()

        int pid_stdin=fork();

        CHECKPOINT()

        if (IS_INVALID(pid_stdin))
        {
            int error=errno;

            CHECKPOINT()

            CLOSE(pipe_stdin_to_cmd[WRITE_TO_PIPE]);
            CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
            CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);
            CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);
            CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

            CHECKPOINT()

            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                     +yarp::os::ConstString(" alias=")+strAlias
                                     +yarp::os::ConstString(" cmd=stdin\n")
                                     +yarp::os::ConstString("Can't fork stdin process because ")+strerror(error)
                                     +yarp::os::ConstString("\n");

            Bottle result;
            result.addInt(YARPRUN_ERROR);
            result.addString(out.c_str());
            fprintf(stderr,"%s",out.c_str());
            fflush(stderr);

            CHECKPOINT()

            SIGNAL(pid_stdout,SIGTERM);
            fprintf(stderr,"TERMINATING stdout (%d)\n",pid_stdout);
            fflush(stderr);

            CHECK_EXIT()

            return result;
        }

        if (IS_NEW_PROCESS(pid_stdin)) // STDIN IMPLEMENTED HERE
        {
            CHECKPOINT()

            REDIRECT_TO(STDOUT_FILENO,pipe_stdin_to_cmd[WRITE_TO_PIPE]);
            REDIRECT_TO(STDERR_FILENO,pipe_stdin_to_cmd[WRITE_TO_PIPE]);

            CHECKPOINT()

            CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
            CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);
            CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

            CHECKPOINT()

            int ret=execlp("yarprun","yarprun","--read",strStdioUUID.c_str(),(char*)NULL);

            CHECKPOINT()

            CLOSE(pipe_stdin_to_cmd[WRITE_TO_PIPE]);

            CHECKPOINT()

            if (ret==YARPRUN_ERROR)
            {
                int error=errno;

                CHECKPOINT()

                yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                         +yarp::os::ConstString(" alias=")+strAlias
                                         +yarp::os::ConstString(" cmd=stdin\n")
                                         +yarp::os::ConstString("Can't execute stdin because ")+strerror(error)
                                         +yarp::os::ConstString("\n");


                FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
                fprintf(out_to_parent,"%s",out.c_str());
                fflush(out_to_parent);
                fclose(out_to_parent);
                fprintf(stderr,"%s",out.c_str());
                fflush(stderr);

                CHECKPOINT()
            }

            CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

            CHECK_EXIT()

            exit(ret);
        }

        if (IS_PARENT_OF(pid_stdin))
        {
            // connect yarp read and write
            CHECKPOINT()

            CLOSE(pipe_stdin_to_cmd[WRITE_TO_PIPE]);

            fprintf(stderr,"STARTED: server=%s alias=%s cmd=stdin pid=%d\n",mPortName.c_str(),strAlias.c_str(),pid_stdin);
            fflush(stderr);

            CHECKPOINT()

            int pid_cmd=fork();

            CHECKPOINT()

            if (IS_INVALID(pid_cmd))
            {
                int error=errno;

                CHECKPOINT()

                CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
                CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);
                CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

                yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                         +yarp::os::ConstString(" alias=")+strAlias
                                         +yarp::os::ConstString(" cmd=")+strCmd
                                         +yarp::os::ConstString("\nCan't fork command process because ")+strerror(error)
                                         +yarp::os::ConstString("\n");

                Bottle result;
                result.addInt(YARPRUN_ERROR);
                result.addString(out.c_str());
                fprintf(stderr,"%s",out.c_str());
                fflush(stderr);

                CHECKPOINT()

                FILE* to_yarp_stdout=fdopen(pipe_cmd_to_stdout[WRITE_TO_PIPE],"w");
                fprintf(to_yarp_stdout,"%s",out.c_str());
                fflush(to_yarp_stdout);
                fclose(to_yarp_stdout);

                CHECKPOINT()

                SIGNAL(pid_stdout,SIGTERM);
                fprintf(stderr,"TERMINATING stdout (%d)\n",pid_stdout);
                SIGNAL(pid_stdin,SIGTERM);
                fprintf(stderr,"TERMINATING stdin (%d)\n",pid_stdin);
                fflush(stderr);

                CHECKPOINT()

                CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);

                CHECK_EXIT()

                return result;
            }

            if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
            {
                CHECKPOINT()

                CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

                char *cmd_str=new char[strCmd.length()+1];
                strcpy(cmd_str,strCmd.c_str());
                int nargs=CountArgs(cmd_str);
                char **arg_str=new char*[nargs+1];
                ParseCmd(cmd_str,arg_str);
                arg_str[nargs]=0;

                CHECKPOINT()

                setvbuf(stdout,NULL,_IONBF,0);

                REDIRECT_TO(STDIN_FILENO, pipe_stdin_to_cmd[READ_FROM_PIPE]);
                REDIRECT_TO(STDOUT_FILENO,pipe_cmd_to_stdout[WRITE_TO_PIPE]);
                REDIRECT_TO(STDERR_FILENO,pipe_cmd_to_stdout[WRITE_TO_PIPE]);

                CHECKPOINT()

                if(msg.check("env"))
                {
                    CHECKPOINT()

                    char* szenv = new char[msg.find("env").asString().length()+1];
                    strcpy(szenv,msg.find("env").asString().c_str());
                    putenv(szenv);
                    //delete szenv;
                }

                CHECKPOINT()

                if (msg.check("workdir"))
                {
                    CHECKPOINT()

                    int ret=chdir(msg.find("workdir").asString().c_str());
                    if (ret!=0)
                    {
                        int error=errno;

                        CHECKPOINT()

                        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                                 +yarp::os::ConstString(" alias=")+strAlias
                                                 +yarp::os::ConstString(" cmd=")+strCmd
                                                 +yarp::os::ConstString("\nCan't execute command, cannot set working directory ")+strerror(error)
                                                 +yarp::os::ConstString("\n");

                        FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
                        fprintf(out_to_parent,"%s",out.c_str());
                        fflush(out_to_parent);
                        fclose(out_to_parent);
                        fprintf(stderr,"%s",out.c_str());
                        fflush(stderr);

                        CHECK_EXIT()

                        exit(ret);
                    }
                }

                CHECKPOINT()

                int ret=YARPRUN_ERROR;

                char currWorkDirBuff[1024];
                char *currWorkDir=getcwd(currWorkDirBuff,1024);

                CHECKPOINT()

                if (currWorkDir)
                {
                    CHECKPOINT()

                    char **cwd_arg_str=new char*[nargs+1];
                    for (int i=1; i<nargs; ++i) cwd_arg_str[i]=arg_str[i];
                    cwd_arg_str[nargs]=0;
                    cwd_arg_str[0]=new char[strlen(currWorkDir)+strlen(arg_str[0])+16];

                    strcpy(cwd_arg_str[0],currWorkDir);
                    strcat(cwd_arg_str[0],"/");
                    strcat(cwd_arg_str[0],arg_str[0]);

                    CHECKPOINT()

                    ret=execvp(cwd_arg_str[0],cwd_arg_str);

                    CHECKPOINT()

                    delete [] cwd_arg_str[0];
                    delete [] cwd_arg_str;
                }

                CHECKPOINT()

                if (ret==YARPRUN_ERROR)
                {
                    CHECKPOINT()

                    ret=execvp(arg_str[0],arg_str);

                    CHECKPOINT()
                }

                fflush(stdout);
                fflush(stderr);

                CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
                CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);

                CHECKPOINT()

                if (ret==YARPRUN_ERROR)
                {
                    int error=errno;

                    CHECKPOINT()

                    yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                             +yarp::os::ConstString(" alias=")+strAlias
                                             +yarp::os::ConstString(" cmd=")+strCmd
                                             +yarp::os::ConstString("\nCan't execute command because ")+strerror(error)
                                             +yarp::os::ConstString("\n");

                    FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
                    fprintf(out_to_parent,"%s",out.c_str());
                    fflush(out_to_parent);
                    fclose(out_to_parent);
                    fprintf(stderr,"%s",out.c_str());
                    fflush(stderr);

                    CHECKPOINT()
                }

                delete [] cmd_str;
                delete [] arg_str;

                CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

                CHECK_EXIT()

                exit(ret);
            }


            if (IS_PARENT_OF(pid_cmd))
            {
                CHECKPOINT()

                CLOSE(pipe_stdin_to_cmd[READ_FROM_PIPE]);
                CLOSE(pipe_cmd_to_stdout[WRITE_TO_PIPE]);
                CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

                CHECKPOINT()

                YarpRunCmdWithStdioInfo* pInf = new YarpRunCmdWithStdioInfo(
                        strAlias,
                        mPortName,
                        strStdio,
                        pid_cmd,
                        strStdioUUID,
                        &mStdioVector,
                        pid_stdin,
                        pid_stdout,
                        pipe_stdin_to_cmd[READ_FROM_PIPE],
                        pipe_stdin_to_cmd[WRITE_TO_PIPE],
                        pipe_cmd_to_stdout[READ_FROM_PIPE],
                        pipe_cmd_to_stdout[WRITE_TO_PIPE],
                        NULL,
                        false
                    );

                pInf->setCmd(strCmd);

                if(msg.check("env"))
                {
                    pInf->setEnv(msg.find("env").asString());
                }

                mProcessVector.Add(pInf);

                CHECKPOINT()

                FILE* in_from_child=fdopen(pipe_child_to_parent[READ_FROM_PIPE],"r");
                int flags=fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_GETFL,0);
                fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_SETFL,flags|O_NONBLOCK);

                CHECKPOINT()

                yarp::os::ConstString out;
                for (char buff[1024]; fgets(buff,1024,in_from_child);)
                {
                    out+=yarp::os::ConstString(buff);
                }
                fclose(in_from_child);

                Bottle result;
                if (out.length()>0)
                {
                    result.addInt(YARPRUN_ERROR);
                }
                else
                {
                    result.addInt(pid_cmd);

                    out=yarp::os::ConstString("STARTED: server=")+mPortName
                       +yarp::os::ConstString(" alias=")+strAlias
                       +yarp::os::ConstString(" cmd=")+strCmd
                       +yarp::os::ConstString(" pid=")+int2String(pid_cmd)
                       +yarp::os::ConstString("\n");
                }

                result.addString(out.c_str());
                result.addString(strStdioUUID.c_str());

                fprintf(stderr,"%s",out.c_str());
                fflush(stderr);

                CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

                CHECK_EXIT()

                return result;
            }
        }
    }

    yarp::os::Bottle result;
    result.addInt(YARPRUN_ERROR);
    result.addString("I should never reach this point!!!\n");

    CHECK_EXIT()

    return result;
}

int yarp::os::Run::UserStdio(yarp::os::Bottle& msg,yarp::os::Bottle& result,yarp::os::ConstString& strStdioPortUUID)
{
    CHECK_ENTER("yarp::os::Run::UserStdio")

    yarp::os::ConstString strAlias=msg.find("as").asString();
    strStdioPortUUID=mPortName+"/"+int2String(getpid())+"/"+strAlias+"-"+int2String(mProcCNT++);
    yarp::os::ConstString strCmd=yarp::os::ConstString("/bin/bash -l -c \"yarprun --readwrite ")+strStdioPortUUID+"\"";

    int pipe_child_to_parent[2];

    if (pipe(pipe_child_to_parent))
    {
        int error=errno;

        CHECKPOINT()

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=stdio\nCan't create pipe ")+strerror(error)
                                 +yarp::os::ConstString("\n");

        result.clear();
        result.addInt(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);

        CHECK_EXIT()

        return YARPRUN_ERROR;
    }

    CHECKPOINT()

    int c=0;
    char *command[16];
    for (int i=0; i<16; ++i) command[i]=NULL;

    cmdcpy(command[c++],"xterm");
    cmdcpy(command[c++],msg.check("hold")?"-hold":"+hold");

    if (msg.check("geometry"))
    {
        cmdcpy(command[c++],"-geometry");
        cmdcpy(command[c++],msg.find("geometry").asString().c_str());
    }

    cmdcpy(command[c++],"-title");
    cmdcpy(command[c++],strAlias.c_str());

    cmdcpy(command[c++],"-e");
    cmdcpy(command[c++],strCmd.c_str());

    CHECKPOINT()

    int pid_cmd=fork();

    CHECKPOINT()

    if (IS_INVALID(pid_cmd))
    {
        int error=errno;

        CHECKPOINT()

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=stdio\nCan't fork stdout process because ")+strerror(error)
                                 +yarp::os::ConstString("\n");

        result.clear();
        result.addInt(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);

        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);
        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

        CHECK_EXIT()

        cmdclean(command);

        return YARPRUN_ERROR;
    }

    if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
    {
        //setvbuf(stdout,NULL,_IONBF,0);
        //REDIRECT_TO(STDOUT_FILENO,pipe_child_to_parent[WRITE_TO_PIPE]);

        CHECKPOINT()

        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

        REDIRECT_TO(STDERR_FILENO,pipe_child_to_parent[WRITE_TO_PIPE]);

        CHECKPOINT()

        int ret=execvp("xterm",command);

        CHECKPOINT()

        //fflush(stdout);
        fflush(stderr);

        cmdclean(command);

        if (ret==YARPRUN_ERROR)
        {
            int error=errno;

            CHECKPOINT()

            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                     +yarp::os::ConstString(" alias=")+strAlias
                                     +yarp::os::ConstString(" cmd=xterm\nCan't execute command because ")+strerror(error)
                                     +yarp::os::ConstString("\n");

            FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");

            fprintf(out_to_parent,"%s",out.c_str());
            fflush(out_to_parent);
            fclose(out_to_parent);

            fprintf(stdout,"%s",out.c_str());
            fflush(stdout);

            CHECKPOINT()
        }

        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

        CHECK_EXIT()

        exit(ret);
    }

    if (IS_PARENT_OF(pid_cmd))
    {
        CHECKPOINT()

        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

        mStdioVector.Add(new YarpRunProcInfo(strAlias,mPortName,pid_cmd,NULL,msg.check("hold")));

        result.clear();

        //yarp::os::Time::delay(0.5);
        cmdclean(command);

        FILE* in_from_child=fdopen(pipe_child_to_parent[READ_FROM_PIPE],"r");
        int flags=fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_GETFL,0);
        fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_SETFL,flags|O_NONBLOCK);

        yarp::os::ConstString out;
        for (char buff[1024]; fgets(buff,1024,in_from_child);)
        {
            out+=yarp::os::ConstString(buff);
        }
        fclose(in_from_child);

        CHECKPOINT()

        result.clear();
        int ret;
        //if (out.length()>0)
        if (out.substr(0,14)=="xterm Xt error" || out.substr(0,7)=="ABORTED")
        {
            result.addInt(ret=YARPRUN_ERROR);
        }
        else
        {
            result.addInt(ret=pid_cmd);
            out=yarp::os::ConstString("STARTED: server=")+mPortName
               +yarp::os::ConstString(" alias=")+strAlias
               +yarp::os::ConstString(" cmd=xterm pid=")+int2String(pid_cmd)
               +yarp::os::ConstString("\n");
        }

        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);

        result.addString(out.c_str());

        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);

        CHECK_EXIT()

        return ret;
    }

    result.clear();
    result.addInt(YARPRUN_ERROR);

    CHECK_EXIT()

    return YARPRUN_ERROR;
}

yarp::os::Bottle yarp::os::Run::ExecuteCmd(yarp::os::Bottle& msg)
{
    CHECK_ENTER("yarp::os::Run::ExecuteCmd")

    yarp::os::ConstString strAlias(msg.find("as").asString());
    yarp::os::ConstString strCmd(msg.find("cmd").toString());

    int  pipe_child_to_parent[2];
    int ret_pipe_child_to_parent=pipe(pipe_child_to_parent);

    CHECKPOINT()

    if (ret_pipe_child_to_parent!=0)
    {
        int error=errno;

        CHECKPOINT()

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=stdio\nCan't create pipe ")+strerror(error)
                                 +yarp::os::ConstString("\n");

        Bottle result;
        result.addInt(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);

        CHECK_EXIT()

        return result;
    }

    CHECKPOINT()

    int pid_cmd=fork();

    CHECKPOINT()

    if (IS_INVALID(pid_cmd))
    {
        int error=errno;

        CHECKPOINT()

        yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                 +yarp::os::ConstString(" alias=")+strAlias
                                 +yarp::os::ConstString(" cmd=")+strCmd
                                 +yarp::os::ConstString("\nCan't fork command process because ")+strerror(error)
                                 +yarp::os::ConstString("\n");

        Bottle result;
        result.addInt(YARPRUN_ERROR);
        result.addString(out.c_str());
        fprintf(stderr,"%s",out.c_str());
        fflush(stderr);

        CHECK_EXIT()

        return result;
    }

    if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
    {
        CHECKPOINT()

        int saved_stderr=dup(STDERR_FILENO);
        int null_file=open("/dev/null",O_WRONLY);
        REDIRECT_TO(STDOUT_FILENO,null_file);
        REDIRECT_TO(STDERR_FILENO,null_file);
        close(null_file);

        CHECKPOINT()

        char *cmd_str=new char[strCmd.length()+1];
        strcpy(cmd_str,strCmd.c_str());
        int nargs=CountArgs(cmd_str);
        char **arg_str=new char*[nargs+1];
        ParseCmd(cmd_str,arg_str);
        arg_str[nargs]=0;

        CHECKPOINT()

        if(msg.check("env"))
        {
            CHECKPOINT()

            char* szenv = new char[msg.find("env").asString().length()+1];
            strcpy(szenv,msg.find("env").asString().c_str());
            putenv(szenv);
            //delete szenv;

            CHECKPOINT()
        }

        CHECKPOINT()

        if (msg.check("workdir"))
        {
            CHECKPOINT()

            int ret=chdir(msg.find("workdir").asString().c_str());
            if (ret!=0)
            {
                int error=errno;

                CHECKPOINT()

                yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                         +yarp::os::ConstString(" alias=")+strAlias
                                         +yarp::os::ConstString(" cmd=")+strCmd
                                         +yarp::os::ConstString("\nCan't execute command, cannot set working directory ")+strerror(error)
                                         +yarp::os::ConstString("\n");

                FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
                fprintf(out_to_parent,"%s",out.c_str());
                fflush(out_to_parent);
                fclose(out_to_parent);

                REDIRECT_TO(STDERR_FILENO,saved_stderr);
                fprintf(stderr,"%s",out.c_str());
                fflush(stderr);

                CHECKPOINT()
            }
        }

        CHECKPOINT()

        int ret=YARPRUN_ERROR;

        char currWorkDirBuff[1024];
        char *currWorkDir=getcwd(currWorkDirBuff,1024);

        if (currWorkDir)
        {
            CHECKPOINT()

            char **cwd_arg_str=new char*[nargs+1];
            for (int i=1; i<nargs; ++i) cwd_arg_str[i]=arg_str[i];
            cwd_arg_str[nargs]=0;
            cwd_arg_str[0]=new char[strlen(currWorkDir)+strlen(arg_str[0])+16];

            strcpy(cwd_arg_str[0],currWorkDir);
            strcat(cwd_arg_str[0],"/");
            strcat(cwd_arg_str[0],arg_str[0]);


            ret=execvp(cwd_arg_str[0],cwd_arg_str);

            delete [] cwd_arg_str[0];
            delete [] cwd_arg_str;

            CHECKPOINT()
        }

        CHECKPOINT()

        if (ret==YARPRUN_ERROR)
        {
            ret=execvp(arg_str[0],arg_str);
        }

        CHECKPOINT()

        if (ret==YARPRUN_ERROR)
        {
            int error=errno;

            CHECKPOINT()

            yarp::os::ConstString out=yarp::os::ConstString("ABORTED: server=")+mPortName
                                     +yarp::os::ConstString(" alias=")+strAlias
                                     +yarp::os::ConstString(" cmd=")+strCmd
                                     +yarp::os::ConstString("\nCan't execute command because ")+strerror(error)
                                     +yarp::os::ConstString("\n");

            FILE* out_to_parent=fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
            fprintf(out_to_parent,"%s",out.c_str());
            fflush(out_to_parent);
            fclose(out_to_parent);

            REDIRECT_TO(STDERR_FILENO,saved_stderr);
            fprintf(stderr,"%s",out.c_str());
            fflush(stderr);

            CHECKPOINT()
        }

        delete [] cmd_str;
        delete [] arg_str;

        CHECK_EXIT()

        exit(ret);
    }

    CHECKPOINT()

    if (IS_PARENT_OF(pid_cmd))
    {
        CHECKPOINT()

        YarpRunProcInfo* pInf = new YarpRunProcInfo(strAlias,mPortName,pid_cmd,NULL,false);
        pInf->setCmd(strCmd);
        if(msg.check("env"))
            pInf->setEnv(msg.find("env").asString());
        mProcessVector.Add(pInf);

        char pidstr[16];
        sprintf(pidstr,"%d",pid_cmd);

        CHECKPOINT()

        FILE* in_from_child=fdopen(pipe_child_to_parent[READ_FROM_PIPE],"r");
        int flags=fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_GETFL,0);
        fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_SETFL,flags|O_NONBLOCK);

        yarp::os::ConstString out;
        for (char buff[1024]; fgets(buff,1024,in_from_child);)
        {
            out+=yarp::os::ConstString(buff);
        }
        fclose(in_from_child);

        CHECKPOINT()

        Bottle result;
        if (out.length()>0)
        {
            result.addInt(YARPRUN_ERROR);
        }
        else
        {
            result.addInt(pid_cmd);
            out=yarp::os::ConstString("STARTED: server=")+mPortName
               +yarp::os::ConstString(" alias=")+strAlias
               +yarp::os::ConstString(" cmd=")+strCmd
               +yarp::os::ConstString(" pid=")+int2String(pid_cmd)
               +yarp::os::ConstString("\n");

            fprintf(stderr,"%s",out.c_str());
            fflush(stderr);
        }

        result.addString(out.c_str());

        CLOSE(pipe_child_to_parent[READ_FROM_PIPE]);
        CLOSE(pipe_child_to_parent[WRITE_TO_PIPE]);

        CHECK_EXIT()

        return result;
    }

    Bottle result;
    result.addInt(YARPRUN_ERROR);

    CHECK_EXIT()

    return result;
}

#endif


bool yarp::os::Run::IS_PARENT_OF(int pid) {
    return pid>0;
}

bool yarp::os::Run::IS_NEW_PROCESS(int pid) {
    return !pid;
}

bool yarp::os::Run::IS_INVALID(int pid) {
    return pid<0;
}

void yarp::os::Run::cmdcpy(char* &dst,const char* src) {
    dst=new char[(strlen(src)/8+2)*16];
    strcpy(dst,src);
}

void yarp::os::Run::cmdclean(char **cmd) {
    while (*cmd) {
        delete [] *cmd++;
    }
}


/////////////////////////////////////////////////////////////////
// API
/////////////////////////////////////////////////////////////////

int yarp::os::Run::start(const yarp::os::ConstString &node,Property &command,yarp::os::ConstString &keyv)
{
    CHECK_ENTER("yarp::os::Run::start")

    yarp::os::Bottle msg,grp,response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    yarp::os::ConstString dest_srv=node;

    CHECKPOINT()

    if (command.check("stdio"))
    {
        CHECKPOINT()

        dest_srv=yarp::os::ConstString(command.find("stdio").asString());

        grp.clear();
        grp.addString("stdio");
        grp.addString(dest_srv.c_str());
        msg.addList()=grp;

        if (command.check("geometry"))
        {
            CHECKPOINT()

            grp.clear();
            grp.addString("geometry");
            grp.addString(command.find("geometry").asString().c_str());
            msg.addList()=grp;
        }

        if (command.check("hold"))
        {
            CHECKPOINT()

            grp.clear();
            grp.addString("hold");
            msg.addList()=grp;
        }
    }

    CHECKPOINT()

    grp.clear();
    grp.addString("as");
    grp.addString(keyv.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("cmd");
    grp.addString(command.find("name").asString().c_str());
    grp.addString(command.find("parameters").asString().c_str());
    msg.addList()=grp;

    printf(":: %s\n",msg.toString().c_str());

    CHECKPOINT()

    response=SendMsg(msg,dest_srv.c_str());

    char buff[16];
    sprintf(buff,"%d",response.get(0).asInt());
    keyv=yarp::os::ConstString(buff);

    CHECK_EXIT()

    return response.get(0).asInt()>0?0:YARPRUN_ERROR;
}

int yarp::os::Run::sigterm(const yarp::os::ConstString &node, const yarp::os::ConstString &keyv)
{
    CHECK_ENTER("yarp::os::Run::sigterm")

    yarp::os::Bottle msg,grp,response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("sigterm");
    grp.addString(keyv.c_str());
    msg.addList()=grp;

    printf(":: %s\n",msg.toString().c_str());

    CHECKPOINT()

    response=SendMsg(msg,node.c_str());

    CHECK_EXIT()

    return response.get(0).asString()=="sigterm OK"?0:YARPRUN_ERROR;
}

int yarp::os::Run::sigterm(const yarp::os::ConstString &node)
{
    CHECK_ENTER("yarp::os::Run::sigterm")

    yarp::os::Bottle msg,grp,response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("sigtermall");
    msg.addList()=grp;

    printf(":: %s\n",msg.toString().c_str());

    CHECKPOINT()

    response=SendMsg(msg,node.c_str());

    CHECK_EXIT()

    return response.get(0).asString()=="sigtermall OK"?0:YARPRUN_ERROR;
}

int yarp::os::Run::kill(const yarp::os::ConstString &node, const yarp::os::ConstString &keyv,int s)
{
    CHECK_ENTER("yarp::os::Run::kill")

    yarp::os::Bottle msg,grp,response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("kill");
    grp.addString(keyv.c_str());
    grp.addInt(s);
    msg.addList()=grp;

    printf(":: %s\n",msg.toString().c_str());

    CHECKPOINT()

    response=SendMsg(msg,node.c_str());

    CHECK_EXIT()

    return response.get(0).asString()=="kill OK"?0:YARPRUN_ERROR;
}

bool yarp::os::Run::isRunning(const yarp::os::ConstString &node, yarp::os::ConstString &keyv)
{
    CHECK_ENTER("yarp::os::Run::isRunning")

    yarp::os::Bottle msg,grp,response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("isrunning");
    grp.addString(keyv.c_str());
    msg.addList()=grp;

    printf(":: %s\n",msg.toString().c_str());

    CHECKPOINT()

    Port port;
    if (!port.open("..."))
    {
        CHECK_EXIT()

        return false;
    }

    CHECKPOINT()

    bool connected=yarp::os::NetworkBase::connect(port.getName(), node);
    if (!connected)
    {
        CHECKPOINT()

        port.close();

        CHECK_EXIT()

        return 0;
    }

    CHECKPOINT()

    port.write(msg,response);

    CHECKPOINT()

    NetworkBase::disconnect(port.getName().c_str(),node.c_str());

    CHECKPOINT()

    port.close();

    CHECKPOINT()

    if (!response.size())
    {
        CHECK_EXIT()

        return false;
    }

    CHECK_EXIT()

    return response.get(0).asString()=="running";
}

// end API

